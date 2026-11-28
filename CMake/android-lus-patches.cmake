# Android-only performance patches for libultraship's Fast3D renderer.
# libultraship is a pinned submodule, so instead of forking it the source is patched in place at
# configure time. Every patch checks for a marker first, so configuring again is harmless.

set(_lus_gfx_pc ${CMAKE_CURRENT_SOURCE_DIR}/libultraship/src/graphic/Fast3D/gfx_pc.cpp)
file(READ ${_lus_gfx_pc} _src)
set(_orig "${_src}")

function(_lus_replace anchor replacement what)
    string(FIND "${_src}" "${anchor}" _pos)
    if(_pos EQUAL -1)
        message(FATAL_ERROR "android-lus-patches: anchor for '${what}' not found in gfx_pc.cpp")
    endif()
    string(REPLACE "${anchor}" "${replacement}" _new "${_src}")
    set(_src "${_new}" PARENT_SCOPE)
endfunction()

# 1) Bigger texture cache: HD texture packs use more than 500 textures in big areas, and evicting
#    them makes every camera turn re-upload textures.
string(REPLACE "#define TEXTURE_CACHE_MAX_SIZE 500" "#define TEXTURE_CACHE_MAX_SIZE 2000" _src "${_src}")

# 2) Off-screen vertex batch culling.
#    Fast3D transforms and lights every vertex on the CPU. Before doing that for a batch (one G_VTX
#    command), transform only the 8 corners of the batch's bounding box. If they are all outside the
#    same screen edge (or past the far plane), the whole batch is invisible: its vertices are marked
#    rejected and left untransformed. A triangle that still needs such a vertex (it connects to
#    visible geometry) transforms it on demand with the matrix that was active when it was loaded.
string(FIND "${_src}" "gfx_vtx_cull_enabled" _already)
if(_already EQUAL -1)
    _lus_replace(
"static void gfx_sp_vertex(size_t n_vertices, size_t dest_index, const F3DVtx* vertices) {
    for (size_t i = 0; i < n_vertices; i++, dest_index++) {
"
"// ---- Android: off-screen vertex batch culling ----
static bool gfx_vtx_cull_enabled = true;
static const F3DVtx* gfx_vtx_pending_src[MAX_VERTICES + 4];
static uint8_t gfx_vtx_pending_mtx[MAX_VERTICES + 4];
#define GFX_VTX_CULL_SAVED_MTX 64
static float gfx_vtx_saved_mtx[GFX_VTX_CULL_SAVED_MTX][4][4];
static uint8_t gfx_vtx_saved_mtx_next = 0;

static void gfx_sp_vertex_impl(size_t n_vertices, size_t dest_index, const F3DVtx* vertices);

static void gfx_resolve_pending_vertex(size_t idx) {
    if (idx >= MAX_VERTICES || gfx_vtx_pending_src[idx] == nullptr) {
        return;
    }
    float saved[4][4];
    memcpy(saved, g_rsp.MP_matrix, sizeof(saved));
    memcpy(g_rsp.MP_matrix, gfx_vtx_saved_mtx[gfx_vtx_pending_mtx[idx]], sizeof(saved));
    gfx_sp_vertex_impl(1, idx, gfx_vtx_pending_src[idx]); // also clears the pending entry
    memcpy(g_rsp.MP_matrix, saved, sizeof(saved));
}

static void gfx_sp_vertex(size_t n_vertices, size_t dest_index, const F3DVtx* vertices) {
    if (gfx_vtx_cull_enabled && n_vertices >= 4 && vertices != nullptr && dest_index + n_vertices <= MAX_VERTICES) {
        float mn[3] = { 1e30f, 1e30f, 1e30f };
        float mx[3] = { -1e30f, -1e30f, -1e30f };
        for (size_t i = 0; i < n_vertices; i++) {
            for (int k = 0; k < 3; k++) {
                float c = vertices[i].v.ob[k];
                mn[k] = c < mn[k] ? c : mn[k];
                mx[k] = c > mx[k] ? c : mx[k];
            }
        }
        // Left, right, bottom, top, far. The planes are linear, so if all corners of the box are
        // outside one of them, every vertex inside the box is too.
        uint8_t outside = 1 | 2 | 4 | 8 | 32;
        for (int c = 0; c < 8 && outside != 0; c++) {
            float px = (c & 1) ? mx[0] : mn[0];
            float py = (c & 2) ? mx[1] : mn[1];
            float pz = (c & 4) ? mx[2] : mn[2];
            float x = px * g_rsp.MP_matrix[0][0] + py * g_rsp.MP_matrix[1][0] + pz * g_rsp.MP_matrix[2][0] +
                      g_rsp.MP_matrix[3][0];
            float y = px * g_rsp.MP_matrix[0][1] + py * g_rsp.MP_matrix[1][1] + pz * g_rsp.MP_matrix[2][1] +
                      g_rsp.MP_matrix[3][1];
            float z = px * g_rsp.MP_matrix[0][2] + py * g_rsp.MP_matrix[1][2] + pz * g_rsp.MP_matrix[2][2] +
                      g_rsp.MP_matrix[3][2];
            float w = px * g_rsp.MP_matrix[0][3] + py * g_rsp.MP_matrix[1][3] + pz * g_rsp.MP_matrix[2][3] +
                      g_rsp.MP_matrix[3][3];
            x = gfx_adjust_x_for_aspect_ratio(x);
            uint8_t bits = 0;
            if (x < -w) {
                bits |= 1;
            }
            if (x > w) {
                bits |= 2;
            }
            if (y < -w) {
                bits |= 4;
            }
            if (y > w) {
                bits |= 8;
            }
            if (z > w) {
                bits |= 32;
            }
            outside &= bits;
        }
        if (outside != 0) {
            uint8_t slot = gfx_vtx_saved_mtx_next;
            gfx_vtx_saved_mtx_next = (uint8_t)((slot + 1) % GFX_VTX_CULL_SAVED_MTX);
            memcpy(gfx_vtx_saved_mtx[slot], g_rsp.MP_matrix, sizeof(gfx_vtx_saved_mtx[slot]));
            for (size_t i = 0; i < n_vertices; i++) {
                g_rsp.loaded_vertices[dest_index + i].clip_rej = outside;
                gfx_vtx_pending_src[dest_index + i] = &vertices[i];
                gfx_vtx_pending_mtx[dest_index + i] = slot;
            }
            return;
        }
    }
    gfx_sp_vertex_impl(n_vertices, dest_index, vertices);
}

static void gfx_sp_vertex_impl(size_t n_vertices, size_t dest_index, const F3DVtx* vertices) {
    for (size_t i = 0; i < n_vertices; i++, dest_index++) {
        if (dest_index < MAX_VERTICES + 4) {
            gfx_vtx_pending_src[dest_index] = nullptr;
        }
"
        "vertex culling: gfx_sp_vertex")

    # Triangles: the rejection test works with the conservative bits; if the triangle survives,
    # transform any vertex that was skipped, then test again with the real bits.
    _lus_replace(
"    if (v1->clip_rej & v2->clip_rej & v3->clip_rej) {
        // The whole triangle lies outside the visible area
        return;
    }
"
"    if (v1->clip_rej & v2->clip_rej & v3->clip_rej) {
        // The whole triangle lies outside the visible area
        return;
    }
    if (vtx1_idx < MAX_VERTICES && vtx2_idx < MAX_VERTICES && vtx3_idx < MAX_VERTICES &&
        (gfx_vtx_pending_src[vtx1_idx] || gfx_vtx_pending_src[vtx2_idx] || gfx_vtx_pending_src[vtx3_idx])) {
        gfx_resolve_pending_vertex(vtx1_idx);
        gfx_resolve_pending_vertex(vtx2_idx);
        gfx_resolve_pending_vertex(vtx3_idx);
        if (v1->clip_rej & v2->clip_rej & v3->clip_rej) {
            return;
        }
    }
"
        "vertex culling: gfx_sp_tri1")

    # Other readers of loaded vertices need the real data.
    _lus_replace(
"    struct LoadedVertex* v = &g_rsp.loaded_vertices[vtx_idx];
    v->u = s;
"
"    gfx_resolve_pending_vertex(vtx_idx);
    struct LoadedVertex* v = &g_rsp.loaded_vertices[vtx_idx];
    v->u = s;
"
        "vertex culling: gfx_sp_modify_vertex")
    _lus_replace(
"    if (g_rsp.loaded_vertices[vbidx].z <= zval ||"
"    gfx_resolve_pending_vertex(vbidx);
    if (g_rsp.loaded_vertices[vbidx].z <= zval ||"
        "vertex culling: branch_z")

    # Per frame: read the setting and forget vertices from the previous frame.
    _lus_replace(
"void gfx_run(Gfx* commands, const std::unordered_map<Mtx*, MtxF>& mtx_replacements) {
    gfx_sp_reset();
"
"void gfx_run(Gfx* commands, const std::unordered_map<Mtx*, MtxF>& mtx_replacements) {
    gfx_sp_reset();
    gfx_vtx_cull_enabled = CVarGetInteger(\"gEnhancements.CullOffscreenVertices\", 1) != 0;
    memset(gfx_vtx_pending_src, 0, sizeof(gfx_vtx_pending_src));
"
        "vertex culling: gfx_run")
endif()

if(NOT "${_src}" STREQUAL "${_orig}")
    file(WRITE ${_lus_gfx_pc} "${_src}")
endif()
