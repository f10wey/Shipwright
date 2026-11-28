#include "ItemWheel.h"

#include <algorithm>
#include <cmath>
#include <vector>
#include <imgui.h>
#include "soh/OTRGlobals.h"
#include "soh/cvar_prefixes.h"
#include "soh/SohGui/ImGuiUtils.h"
#include "soh/SohGui/Localization.h"

extern "C" {
#include "z64.h"
#include "macros.h"
#include "functions.h"
#include "variables.h"

extern PlayState* gPlayState;
extern u8 gEquipAgeReqs[][4];
#ifdef __ANDROID__
extern volatile int touchWheelHeld;
#endif
}

#define WHEEL_AGE_REQ_NONE 9

namespace {

enum class EntryKind { Item, Equipment };

struct Entry {
    EntryKind kind;
    u8 item;       // item id to put on the C button / icon to show
    u8 iconItem;   // item id used for the icon
    u8 slot;       // inventory slot for items
    u8 equipType;  // for equipment
    u8 equipValue; // for equipment (1..3)
    bool allowed;  // age restrictions
    bool active;   // equipped (equipment) / on a C button (items)
    s8 cButton;    // which C button the item is on (0..2), -1 if none
    float x, y;    // position relative to the wheel center, in units of the wheel radius
};

bool sOpen = false;
bool sCloseRequested = false;
int sSelected = -1;
int sClicked = -1;
std::vector<Entry> sEntries;

bool CanOpen(PlayState* play) {
    if (play == nullptr || play->pauseCtx.state != 0 || play->msgCtx.msgMode != 0 || play->transitionTrigger != 0 ||
        play->gameOverCtx.state != 0 || Player_InCsMode(play)) {
        return false;
    }
    Player* player = GET_PLAYER(play);
    if (player == nullptr) {
        return false;
    }
    if (player->stateFlags1 & (PLAYER_STATE1_LOADING | PLAYER_STATE1_INPUT_DISABLED | PLAYER_STATE1_TALKING |
                               PLAYER_STATE1_DEAD | PLAYER_STATE1_GETTING_ITEM | PLAYER_STATE1_IN_ITEM_CS |
                               PLAYER_STATE1_IN_CUTSCENE)) {
        return false;
    }
    if (player->stateFlags2 & PLAYER_STATE2_OCARINA_PLAYING) {
        return false;
    }
    return true;
}

bool SlotAllowed(u8 slot) {
    return CVarGetInteger(CVAR_CHEAT("TimelessEquipment"), 0) || gSlotAgeReqs[slot] == WHEEL_AGE_REQ_NONE ||
           gSlotAgeReqs[slot] == gSaveContext.linkAge;
}

bool EquipAllowed(u8 type, u8 value) {
    return CVarGetInteger(CVAR_CHEAT("TimelessEquipment"), 0) || gEquipAgeReqs[type][value] == WHEEL_AGE_REQ_NONE ||
           gEquipAgeReqs[type][value] == gSaveContext.linkAge;
}

void BuildEntries() {
    sEntries.clear();

    // Inner ring: equipment
    std::vector<Entry> equips;
    for (u8 type = 0; type < 4; type++) {
        for (u8 value = 1; value <= 3; value++) {
            bool owned = CHECK_OWNED_EQUIP(type, value - 1);
            u8 icon = ITEM_SWORD_KOKIRI + type * 3 + (value - 1);
            if (type == EQUIP_TYPE_SWORD && value == 3) {
                bool broken = CHECK_OWNED_EQUIP_ALT(EQUIP_TYPE_SWORD, EQUIP_INV_SWORD_BROKENGIANTKNIFE);
                owned = owned || broken;
                if (!gSaveContext.bgsFlag && broken) {
                    icon = ITEM_SWORD_KNIFE;
                }
            }
            if (!owned) {
                continue;
            }
            Entry e = {};
            e.kind = EntryKind::Equipment;
            e.item = icon;
            e.iconItem = icon;
            e.equipType = type;
            e.equipValue = value;
            e.allowed = EquipAllowed(type, value);
            e.active = CUR_EQUIP_VALUE(type) == value;
            e.cButton = -1;
            equips.push_back(e);
        }
    }

    // Outer ring: items from the item screen
    std::vector<Entry> items;
    for (u8 slot = 0; slot < 24; slot++) {
        u8 item = gSaveContext.inventory.items[slot];
        if (item == ITEM_NONE || item == ITEM_SOLD_OUT) {
            continue;
        }
        Entry e = {};
        e.kind = EntryKind::Item;
        e.iconItem = item;
        e.item = item;
        e.slot = slot;
        // Magic arrows are put on the button as "bow + arrow"
        if (item == ITEM_ARROW_FIRE) {
            e.item = ITEM_BOW_ARROW_FIRE;
        } else if (item == ITEM_ARROW_ICE) {
            e.item = ITEM_BOW_ARROW_ICE;
        } else if (item == ITEM_ARROW_LIGHT) {
            e.item = ITEM_BOW_ARROW_LIGHT;
        }
        if (e.item != item && !CVarGetInteger(CVAR_ENHANCEMENT("SeparateArrows"), 0)) {
            e.slot = SLOT_BOW;
        }
        e.allowed = SlotAllowed(slot);
        e.cButton = -1;
        for (int c = 0; c < 3; c++) {
            if (gSaveContext.equips.buttonItems[c + 1] == e.item && gSaveContext.equips.cButtonSlots[c] == e.slot) {
                e.cButton = c;
            }
        }
        e.active = e.cButton >= 0;
        items.push_back(e);
    }

    const float kPi = 3.14159265f;
    for (size_t i = 0; i < equips.size(); i++) {
        float a = -kPi / 2 + 2 * kPi * i / equips.size();
        equips[i].x = 0.45f * cosf(a);
        equips[i].y = 0.45f * sinf(a);
        sEntries.push_back(equips[i]);
    }
    for (size_t i = 0; i < items.size(); i++) {
        float a = -kPi / 2 + 2 * kPi * i / items.size();
        items[i].x = cosf(a);
        items[i].y = sinf(a);
        sEntries.push_back(items[i]);
    }
}

void PlaySfx(u16 sfx) {
    Audio_PlaySoundGeneral(sfx, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale,
                           &gSfxDefaultReverb);
}

// Same steps as the equipment screen of the pause menu
void ApplyEquipment(PlayState* play, const Entry& e) {
    if (!e.allowed) {
        PlaySfx(NA_SE_SY_ERROR);
        return;
    }
    Inventory_ChangeEquipment(e.equipType, e.equipValue);
    if (e.equipType == EQUIP_TYPE_SWORD) {
        gSaveContext.infTable[29] = 0;
        u8 bItem = ITEM_SWORD_KOKIRI + (e.equipValue - 1);
        if (e.equipValue == 3) {
            if (gSaveContext.bgsFlag != 0) {
                bItem = ITEM_SWORD_BGS;
                gSaveContext.swordHealth = 8;
            } else if (CHECK_OWNED_EQUIP_ALT(EQUIP_TYPE_SWORD, EQUIP_INV_SWORD_BROKENGIANTKNIFE)) {
                bItem = ITEM_SWORD_KNIFE;
            }
        }
        gSaveContext.equips.buttonItems[0] = bItem;
        Interface_LoadItemIcon1(play, 0);
    }
    Player_SetEquipmentData(play, GET_PLAYER(play));
    PlaySfx(NA_SE_SY_DECIDE);
}

// Same steps as putting an item on a C button in the pause menu, including swapping with another C button
void AssignItem(PlayState* play, const Entry& e, int cIndex) {
    if (!e.allowed) {
        PlaySfx(NA_SE_SY_ERROR);
        return;
    }
    int targetButton = cIndex + 1;
    for (int other = 0; other < 3; other++) {
        if (other == cIndex) {
            continue;
        }
        int otherButton = other + 1;
        if (gSaveContext.equips.cButtonSlots[other] == e.slot) {
            if (gSaveContext.equips.buttonItems[targetButton] != ITEM_NONE) {
                gSaveContext.equips.buttonItems[otherButton] = gSaveContext.equips.buttonItems[targetButton];
                gSaveContext.equips.cButtonSlots[other] = gSaveContext.equips.cButtonSlots[cIndex];
                Interface_LoadItemIcon2(play, otherButton);
            } else {
                gSaveContext.equips.buttonItems[otherButton] = ITEM_NONE;
                gSaveContext.equips.cButtonSlots[other] = SLOT_NONE;
            }
        }
        // Bow on one button and bow + magic arrow on another would duplicate the bow
        if (e.item == ITEM_BOW && gSaveContext.equips.buttonItems[otherButton] >= ITEM_BOW_ARROW_FIRE &&
            gSaveContext.equips.buttonItems[otherButton] <= ITEM_BOW_ARROW_LIGHT &&
            !CVarGetInteger(CVAR_ENHANCEMENT("SeparateArrows"), 0)) {
            gSaveContext.equips.buttonItems[otherButton] = gSaveContext.equips.buttonItems[targetButton];
            gSaveContext.equips.cButtonSlots[other] = gSaveContext.equips.cButtonSlots[cIndex];
            Interface_LoadItemIcon2(play, otherButton);
        }
    }
    gSaveContext.equips.buttonItems[targetButton] = e.item;
    gSaveContext.equips.cButtonSlots[cIndex] = e.slot;
    Interface_LoadItemIcon1(play, targetButton);
    PlaySfx(NA_SE_SY_DECIDE);
}

bool sPrevMouseCapture = false;

void Open() {
    // Mouse/touch input must reach the wheel (captured mouse blocks ImGui windows)
    auto window = Ship::Context::GetInstance()->GetWindow();
    sPrevMouseCapture = window->IsMouseCaptured();
    if (sPrevMouseCapture) {
        window->SetMouseCapture(false);
    }
    sOpen = true;
    sSelected = -1;
    sClicked = -1;
    sCloseRequested = false;
    BuildEntries();
    PlaySfx(NA_SE_SY_WIN_OPEN);
}

void Close() {
    if (sPrevMouseCapture) {
        Ship::Context::GetInstance()->GetWindow()->SetMouseCapture(true);
        sPrevMouseCapture = false;
    }
    sOpen = false;
    sSelected = -1;
    sClicked = -1;
    sCloseRequested = false;
    PlaySfx(NA_SE_SY_WIN_CLOSE);
}

} // namespace

extern "C" int ItemWheel_IsOpen() {
    return sOpen ? 1 : 0;
}

extern "C" int ItemWheel_Update(PlayState* play) {
    static int sPrevHeld = 0;
#ifdef __ANDROID__
    int held = touchWheelHeld;
#else
    int held = 0;
#endif
    int pressed = held && !sPrevHeld;
    sPrevHeld = held;

    if (!CVarGetInteger(CVAR_ENHANCEMENT("ItemWheel"), 1)) {
        if (sOpen) {
            Close();
        }
        return 0;
    }

    if (!sOpen) {
        if (pressed && CanOpen(play)) {
            Open();
            return 1;
        }
        return 0;
    }

    // Something took over (cutscene, death, scene change): give control back to the game
    if (play == nullptr || play->transitionTrigger != 0 || play->gameOverCtx.state != 0) {
        Close();
        return 0;
    }

    Input* input = &play->state.input[0];

    if (pressed || sCloseRequested || CHECK_BTN_ALL(input->press.button, BTN_B)) {
        Close();
        // Keep this frame paused so the closing tap/button doesn't reach the game
        input->press.button = 0;
        return 1;
    }

    // Taps on the wheel
    if (sClicked >= 0 && sClicked < (int)sEntries.size()) {
        Entry e = sEntries[sClicked];
        if (e.kind == EntryKind::Equipment) {
            ApplyEquipment(play, e);
            BuildEntries();
        } else {
            sSelected = sClicked;
            PlaySfx(NA_SE_SY_CURSOR);
        }
    }
    sClicked = -1;

    // Stick: highlight the entry nearest to where the stick points
    float sx = input->rel.stick_x;
    float sy = input->rel.stick_y;
    if (sx * sx + sy * sy > 40.0f * 40.0f) {
        float tx = sx / 70.0f;
        float ty = -sy / 70.0f;
        int best = -1;
        float bestD = 1e9f;
        for (size_t i = 0; i < sEntries.size(); i++) {
            float dx = sEntries[i].x - tx;
            float dy = sEntries[i].y - ty;
            float d = dx * dx + dy * dy;
            if (d < bestD) {
                bestD = d;
                best = (int)i;
            }
        }
        if (best != sSelected && best >= 0) {
            sSelected = best;
            PlaySfx(NA_SE_SY_CURSOR);
        }
    }

    if (sSelected >= 0 && sSelected < (int)sEntries.size()) {
        Entry e = sEntries[sSelected];
        if (e.kind == EntryKind::Item) {
            int c = -1;
            if (CHECK_BTN_ALL(input->press.button, BTN_CLEFT)) {
                c = 0;
            } else if (CHECK_BTN_ALL(input->press.button, BTN_CDOWN)) {
                c = 1;
            } else if (CHECK_BTN_ALL(input->press.button, BTN_CRIGHT)) {
                c = 2;
            }
            if (c >= 0) {
                AssignItem(play, e, c);
                BuildEntries();
            }
        } else if (CHECK_BTN_ALL(input->press.button, BTN_A)) {
            ApplyEquipment(play, e);
            BuildEntries();
        }
    }

    // The game is paused: nothing of this frame's input may reach it
    input->press.button = 0;
    return 1;
}

void ItemWheelWindow::Draw() {
    if (!sOpen) {
        return;
    }

    ImGuiViewport* vp = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(vp->Pos);
    ImGui::SetNextWindowSize(vp->Size);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::Begin("##ItemWheel", nullptr,
                 ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
                     ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoDocking);

    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 center(vp->Pos.x + vp->Size.x * 0.5f, vp->Pos.y + vp->Size.y * 0.5f);
    float shortSide = std::min(vp->Size.x, vp->Size.y);
    float radius = shortSide * 0.38f;
    float icon = shortSide * 0.11f;

    // Dim the game behind the wheel
    dl->AddRectFilled(vp->Pos, ImVec2(vp->Pos.x + vp->Size.x, vp->Pos.y + vp->Size.y), IM_COL32(0, 0, 0, 120));
    dl->AddCircleFilled(center, radius + icon * 0.8f, IM_COL32(20, 30, 20, 150), 64);
    dl->AddCircle(center, radius + icon * 0.8f, IM_COL32(200, 180, 90, 200), 64, 3.0f);
    dl->AddCircle(center, radius * 0.45f + icon * 0.75f, IM_COL32(200, 180, 90, 120), 64, 2.0f);

    static const char* sCNames[] = { "C<", "Cv", "C>" };
    bool anyHovered = false;

    for (size_t i = 0; i < sEntries.size(); i++) {
        const Entry& e = sEntries[i];
        ImVec2 p(center.x + e.x * radius, center.y + e.y * radius);
        ImVec2 tl(p.x - icon * 0.5f, p.y - icon * 0.5f);
        ImVec2 br(p.x + icon * 0.5f, p.y + icon * 0.5f);

        bool selected = (int)i == sSelected;
        ImU32 bg = selected ? IM_COL32(230, 200, 60, 230) : (e.active ? IM_COL32(60, 120, 60, 220) : IM_COL32(0, 0, 0, 170));
        dl->AddCircleFilled(p, icon * 0.62f, bg, 32);
        if (e.active) {
            dl->AddCircle(p, icon * 0.62f, IM_COL32(120, 230, 120, 255), 32, 2.0f);
        }

        const char* tex = GetTextureForItemId(e.iconItem);
        if (tex != nullptr) {
            ImTextureID id = Ship::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(tex);
            dl->AddImage(id, tl, br, ImVec2(0, 0), ImVec2(1, 1),
                         e.allowed ? IM_COL32(255, 255, 255, 255) : IM_COL32(255, 255, 255, 80));
        }
        if (e.cButton >= 0) {
            dl->AddText(ImVec2(br.x - icon * 0.25f, br.y - icon * 0.2f), IM_COL32(255, 230, 80, 255),
                        sCNames[e.cButton]);
        }

        ImGui::SetCursorScreenPos(ImVec2(p.x - icon * 0.62f, p.y - icon * 0.62f));
        ImGui::PushID((int)i);
        if (ImGui::InvisibleButton("##wheelItem", ImVec2(icon * 1.24f, icon * 1.24f))) {
            sClicked = (int)i;
        }
        anyHovered = anyHovered || ImGui::IsItemHovered();
        ImGui::PopID();
    }

    // Hint in the middle
    const char* hint = SohLoc::Text("Tap an item, then press a C button");
    if (sSelected >= 0 && sSelected < (int)sEntries.size() && sEntries[sSelected].kind == EntryKind::Item) {
        hint = SohLoc::Text("Press C-Left, C-Down or C-Right");
    }
    ImVec2 ts = ImGui::CalcTextSize(hint);
    dl->AddText(ImVec2(center.x - ts.x * 0.5f, center.y - ts.y * 0.5f), IM_COL32(255, 255, 255, 255), hint);

    // Tapping outside the wheel closes it
    if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) && !anyHovered) {
        ImVec2 m = ImGui::GetMousePos();
        float dx = m.x - center.x;
        float dy = m.y - center.y;
        float outer = radius + icon * 0.8f;
        if (dx * dx + dy * dy > outer * outer) {
            sCloseRequested = true;
        }
    }

    ImGui::End();
    ImGui::PopStyleVar(2);
}
