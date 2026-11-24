#include "OTRGlobals.h"
#include "ResourceManagerHelpers.h"
#include <libultraship/libultraship.h>
#include "soh/resource/type/Scene.h"
#include "soh/cvar_prefixes.h"
#include <utils/StringHelper.h>
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "global.h"
#include "vt.h"
#include <Vertex.h>
#include <atomic>
#include <thread>

extern "C" void Play_InitScene(PlayState* play, s32 spawn);
extern "C" void Play_InitEnvironment(PlayState* play, s16 skyboxId);
void OTRPlay_InitScene(PlayState* play, s32 spawn);
s32 OTRScene_ExecuteCommands(PlayState* play, SOH::Scene* scene);

// LUS::OTRResource* OTRPlay_LoadFile(PlayState* play, RomFile* file) {
Ship::IResource* OTRPlay_LoadFile(PlayState* play, const char* fileName) {
    auto res = Ship::Context::GetInstance()->GetResourceManager()->LoadResource(fileName);
    return res.get();
}

// Background preload of everything in the scene folder (rooms, display lists, textures; the alternate/HD
// versions are picked automatically when Alternate Assets is on). Without this, big HD textures are read and
// decoded the first time they appear on screen, which causes stutter right after entering a large area.
static std::atomic<uint32_t> sScenePrefetchGeneration{ 0 };

static void PrefetchSceneResources(const std::string& sceneDir) {
    if (!CVarGetInteger(CVAR_ENHANCEMENT("PreloadSceneAssets"), 1)) {
        return;
    }
    uint32_t generation = ++sScenePrefetchGeneration;
    std::thread([sceneDir, generation]() {
        auto resourceManager = Ship::Context::GetInstance()->GetResourceManager();
        auto files = resourceManager->GetArchiveManager()->ListFiles(sceneDir + "/*");
        if (files == nullptr) {
            return;
        }
        for (const auto& file : *files) {
            // Stop if the player already moved to another scene
            if (sScenePrefetchGeneration.load() != generation) {
                return;
            }
            resourceManager->LoadResource(file);
        }
    }).detach();
}

extern "C" void OTRPlay_SpawnScene(PlayState* play, s32 sceneId, s32 spawn) {
    SceneTableEntry* scene = &gSceneTable[sceneId];

    scene->unk_13 = 0;
    play->loadedScene = scene;
    play->sceneNum = sceneId;
    play->sceneConfig = scene->config;

    // osSyncPrintf("\nSCENE SIZE %fK\n", (scene->sceneFile.vromEnd - scene->sceneFile.vromStart) / 1024.0f);

    // Scenes considered "dungeon" with a MQ variant
    int16_t inNonSharedScene = (sceneId >= SCENE_DEKU_TREE && sceneId <= SCENE_ICE_CAVERN) ||
                               sceneId == SCENE_GERUDO_TRAINING_GROUND || sceneId == SCENE_INSIDE_GANONS_CASTLE;

    std::string sceneVersion = "shared";
    if (inNonSharedScene) {
        sceneVersion = ResourceMgr_IsGameMasterQuest() ? "mq" : "nonmq";
    }
    std::string scenePath = StringHelper::Sprintf("scenes/%s/%s/%s", sceneVersion.c_str(), scene->sceneFile.fileName,
                                                  scene->sceneFile.fileName);

    PrefetchSceneResources(StringHelper::Sprintf("scenes/%s/%s", sceneVersion.c_str(), scene->sceneFile.fileName));

    play->sceneSegment = OTRPlay_LoadFile(play, scenePath.c_str());

    // Failed to load scene... default to doodongs cavern
    if (play->sceneSegment == nullptr) {
        lusprintf(__FILE__, __LINE__, 2, "Unable to load scene %s... Defaulting to Doodong's Cavern!\n",
                  scenePath.c_str());
        OTRPlay_SpawnScene(play, 0x01, 0);
        return;
    }

    scene->unk_13 = 0;

    // gSegments[2] = VIRTUAL_TO_PHYSICAL(play->sceneSegment);

    OTRPlay_InitScene(play, spawn);
    auto roomSize = func_80096FE8(play, &play->roomCtx);

    osSyncPrintf("ROOM SIZE=%fK\n", roomSize / 1024.0f);

    GameInteractor_ExecuteOnSceneInit(play->sceneNum);
    SPDLOG_INFO("Scene Init - sceneNum: {0:#x}, entranceIndex: {1:#x}", play->sceneNum, gSaveContext.entranceIndex);
}

void OTRPlay_InitScene(PlayState* play, s32 spawn) {
    play->curSpawn = spawn;
    play->linkActorEntry = nullptr;
    play->unk_11DFC = nullptr;
    play->setupEntranceList = nullptr;
    play->setupExitList = nullptr;
    play->cUpElfMsgs = nullptr;
    play->setupPathList = nullptr;
    play->numSetupActors = 0;
    Object_InitBank(play, &play->objectCtx);
    LightContext_Init(play, &play->lightCtx);
    TransitionActor_InitContext(&play->state, &play->transiActorCtx);
    func_80096FD4(play, &play->roomCtx.curRoom);
    YREG(15) = 0;
    gSaveContext.worldMapArea = 0;
    OTRScene_ExecuteCommands(play, (SOH::Scene*)play->sceneSegment);
    GameInteractor_ExecuteAfterSceneCommands(play->sceneNum);
    Play_InitEnvironment(play, play->skyboxId);
    /* auto data = static_cast<LUS::Vertex*>(Ship::Context::GetInstance()
                                               ->GetResourceManager()
                                               ->ResourceLoad("object_link_child\\object_link_childVtx_01FE08")
                                               .get());

    auto data2 = ResourceMgr_LoadVtxByCRC(0x68d4ea06044e228f);*/

    volatile int a = 0;
}
