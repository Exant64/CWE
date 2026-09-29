#include "ninja_functions.h"
#include "stdafx.h"
#include "al_emotion.h"
#include "al_parameter.h"
#include "alg_kinder_ortho.h"
#include "al_garden_info.h"
#include "al_behavior/albhv.h"
#include "ChaoMain.h"
#include <cstring>
#include <filesystem>
#include <vector>
#include <al_stage.h>
#include <Chao.h>
#include <chaofile.h>
#include <minwindef.h>
#include <windows.h>
#include <FunctionHook.h>
#include <code_system/cwe_transpile.h>

#ifdef IMGUIDEBUG
#include "imgui/imgui.h"
#endif

#define GUEST_CHAO_MAX 40

struct GuestInfo {
    bool m_occupied;
    bool m_hasSpawnedEverYet;
    CHAO_SAVE_INFO m_saveInfo;
    size_t m_pathIndex;

    struct BackupData {
        Uint8 Exp[8];
        char Abl[8];
        char Lev[8];
        __int16 Skill[8];
        
        Sint8 place;
        __int16 like;
        __int16 age;
        __int16 old;
        __int16 life;
        __int16 LifeMax;
        __int16 nbSucceed;
        int LifeTimer;

        AL_BODY_INFO body;
        Uint16 EmotionBreed;

        AL_KNOWLEDGE_PLAYER KnowledgePlayer[6];
        AL_KNOWLEDGE_CHAO KnowledgeChao[20];

        char ShinyJewelMonotone;
        char LobbyTextureValue;
        char EyeAlignment;
	    char EyeColor;
        bool DCWings;
	    bool Negative;
    } m_backup;
};

static int GuestMin, GuestMax, RotateCount;

static int RotationWindowIndex = 0;
static CHAO_SAVE_INFO* pLastHoldingChaoSaveInfo = NULL;

static int VisitCounter;
static GuestInfo GuestInfoList[GUEST_CHAO_MAX];
static std::vector<size_t> GuestChaoFilePathIndices;
static std::vector<std::wstring> GuestChaoFilePaths;

#ifdef IMGUIDEBUG
void Guest_Debug(bool& open) {
    if(open && ImGui::Begin("Guest", &open)) {
        ImGui::InputInt("RotationWindow", &RotationWindowIndex);
        ImGui::InputInt("VisitCounter", &VisitCounter);

        if(ImGui::TreeNode("Paths")) {
            for(size_t i = 0; i < GuestChaoFilePaths.size(); ++i) {
                ImGui::Text("%d: %ls", int(i), GuestChaoFilePaths[i].c_str());
            }

            ImGui::TreePop();
        }

        if(ImGui::TreeNode("Indices")) {
            for(size_t i = 0; i < GuestChaoFilePathIndices.size(); ++i) {
                ImGui::Text("%d: %d", int(i), int(GuestChaoFilePathIndices[i]));
            }
            
            ImGui::TreePop();
        }

        if(ImGui::TreeNode("Info")) {
            for(size_t i = 0; i < _countof(GuestInfoList); ++i) {
                const auto& info = GuestInfoList[i];

                ImGui::Text("%d: occupied = %d, pathIndex = %d", int(i), int(info.m_occupied), int(info.m_pathIndex));
            }
            
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Config")) {
            ImGui::Checkbox("GuestSave", &gConfigVal.GuestSave);
            
            ImGui::InputInt("GuestRollType", (int*)&gConfigVal.GuestRollType);
            ImGui::InputInt("GuestVisitCounter", (int*)&gConfigVal.GuestVisitCounter);
            ImGui::InputInt("GuestMin (config)", (int*)&gConfigVal.GuestMin);
            ImGui::InputInt("GuestMax (config)", (int*)&gConfigVal.GuestMax);
            ImGui::InputInt("GuestRotateCount (config)", (int*)&gConfigVal.GuestRotateCount);
            ImGui::InputInt("GuestMin", (int*)&GuestMin);
            ImGui::InputInt("GuestMax", (int*)&GuestMax);
            ImGui::InputInt("GuestRotateCount", (int*)&RotateCount);
            ImGui::Checkbox("GuestRandomizeEmotions", &gConfigVal.GuestRandomizeEmotions);
            ImGui::Checkbox("GuestBlockStatChanges", &gConfigVal.GuestBlockStatChanges);
            ImGui::Checkbox("GuestBlockNameChange", &gConfigVal.GuestBlockNameChange);
            ImGui::Checkbox("GuestBlockBodyChanges", &gConfigVal.GuestBlockBodyChanges);
            ImGui::Checkbox("GuestBlockWearableChanges", &gConfigVal.GuestBlockWearableChanges);
            ImGui::Checkbox("GuestBlockSocialRelations", &gConfigVal.GuestBlockSocialRelations);
            ImGui::Checkbox("GuestBlockBreeding", &gConfigVal.GuestBlockBreeding);
            ImGui::Checkbox("GuestBlockLifeChanges", &gConfigVal.GuestBlockLifeChanges);
            ImGui::Checkbox("GuestBlockMinimalFlagChanges", &gConfigVal.GuestBlockMinimalFlagChanges);
            ImGui::Checkbox("GuestBlockMinimalPartChanges", &gConfigVal.GuestBlockMinimalPartChanges);

            ImGui::TreePop();
        }

        ImGui::End();
    }
}
#endif

static void ShuffleIndices() {
    for(size_t i = GuestChaoFilePathIndices.size() - 1; i != 0; --i) {
        size_t randIndex = size_t(njRandom() * (float(i) - 0.001f));

        std::swap(GuestChaoFilePathIndices[i], GuestChaoFilePathIndices[randIndex]);
    }
}

static bool ValidateChaoFile(const wchar_t* path) {
    const size_t headerSize = 64;
    FILE* chaoFile;

    _wfopen_s(&chaoFile, path, L"rb");

    if(!chaoFile) {
        return false;
    }

    fseek(chaoFile, 0, SEEK_END);
    const size_t fileSize = ftell(chaoFile);
    
    if (fileSize != headerSize + sizeof(CHAO_SAVE_INFO)) {
        fclose(chaoFile);
        return false;
    }
    
    Uint32 magic;
    fseek(chaoFile, 0, SEEK_SET);
    fread(&magic, 1, sizeof(magic), chaoFile);

    Uint8 chaoType;
    fseek(chaoFile, headerSize + offsetof(CHAO_PARAM_GC, type), SEEK_SET);
    fread(&chaoType, 1, sizeof(chaoType), chaoFile);

    fclose(chaoFile);

    // .chao file first 4 bytes
    return magic == 0x52B72814 && chaoType >= TYPE_CHILD;
}

static void SaveGuestChao(size_t infoIndex) {
    auto& info = GuestInfoList[infoIndex];
    CHAO_PARAM_GC* pParam = &info.m_saveInfo.param;
    CHAO_PARAM_CWE* pCweParam = GET_CWEPARAM(pParam);

    assert(info.m_occupied);

    info.m_occupied = false;
    pParam->place = info.m_backup.place;

    if (gConfigVal.GuestBlockLifeChanges) {
        pParam->age = info.m_backup.age;
        pParam->old = info.m_backup.old;
        pParam->life = info.m_backup.life;
        pParam->LifeMax = info.m_backup.LifeMax;
        pParam->nbSucceed = info.m_backup.nbSucceed;
        
        pParam->LifeTimer = info.m_backup.LifeTimer;
    }

    if (gConfigVal.GuestBlockPlayerRelations) {
        pParam->like = info.m_backup.like;
        memcpy(&pParam->knowledge.player, &info.m_backup.KnowledgePlayer, sizeof(pParam->knowledge.player));
    }

    if (gConfigVal.GuestBlockSocialRelations) {
        memcpy(&pParam->knowledge.chao, &info.m_backup.KnowledgeChao, sizeof(pParam->knowledge.chao));
    }

    if (gConfigVal.GuestBlockBreeding) {
        pParam->emotion.State[EM_ST_BREED - EM_ST_SLEEPY] = info.m_backup.EmotionBreed;
    }

    if (gConfigVal.GuestBlockBodyChanges) {
        pParam->body.HPos = info.m_backup.body.HPos;
        pParam->body.VPos = info.m_backup.body.VPos;
        pParam->body.APos = info.m_backup.body.APos;
        pParam->body.growth = info.m_backup.body.growth;
    }

    if (gConfigVal.GuestBlockVisualChanges) {
        pParam->body.EyeColor = info.m_backup.body.EyeColor;
        pParam->body.HonbuNum = info.m_backup.body.HonbuNum;
        pParam->body.HonbuColorNum = info.m_backup.body.HonbuColorNum;
        pParam->body.ObakeBody = info.m_backup.body.ObakeBody;
        pParam->body.ColorNum = info.m_backup.body.ColorNum;
        pParam->body.NonTex = info.m_backup.body.NonTex;
        pParam->body.JewelNum = info.m_backup.body.JewelNum;
        pParam->body.MultiNum = info.m_backup.body.MultiNum;
        pParam->body.EggColor = info.m_backup.body.EggColor;
        pParam->body.FormNum = info.m_backup.body.FormNum;

        pCweParam->ShinyJewelMonotone = info.m_backup.ShinyJewelMonotone;
        pCweParam->LobbyTextureValue = info.m_backup.LobbyTextureValue;
        pCweParam->EyeAlignment = info.m_backup.EyeAlignment;
        pCweParam->EyeColor = info.m_backup.EyeColor;
        pCweParam->DCWings = info.m_backup.DCWings;
        pCweParam->Negative = info.m_backup.Negative;
    }

    GET_CWEPARAM(&info.m_saveInfo)->IsGuest = FALSE;

    const auto path = GuestChaoFilePaths[info.m_pathIndex].c_str();
    if(!SaveChaoFile(path, &info.m_saveInfo) || true) {
        wchar_t* pMsgBuf = new wchar_t[wcslen(path) + 21 + 1];
        wsprintfW(pMsgBuf, L"\"%s\" failed to save!", path);

        MessageBoxW(0, pMsgBuf, L"Chao World Extended: Guest", MB_ICONERROR);

        delete[] pMsgBuf;
    }
}

static void LoadGuestChao(size_t infoIndex, size_t pathIndex) {
    auto& info = GuestInfoList[infoIndex];

    info.m_occupied = true;
    info.m_pathIndex = pathIndex;

    info.m_saveInfo = LoadChaoFile(GuestChaoFilePaths[pathIndex].c_str());
    CHAO_PARAM_GC* pParam = &info.m_saveInfo.param;
    CHAO_PARAM_CWE* pCweParam = GET_CWEPARAM(pParam);

    info.m_backup.place = pParam->place;

    if (gConfigVal.GuestBlockLifeChanges) {
        info.m_backup.age = pParam->age;
        info.m_backup.old = pParam->old;
        info.m_backup.life = pParam->life;
        info.m_backup.LifeMax = pParam->LifeMax;
        info.m_backup.nbSucceed = pParam->nbSucceed;

        info.m_backup.LifeTimer = pParam->LifeTimer;
    }

    if (gConfigVal.GuestBlockPlayerRelations) {
        info.m_backup.like = pParam->like;
        memcpy(&info.m_backup.KnowledgePlayer, &pParam->knowledge.player, sizeof(pParam->knowledge.player));
    }

    if (gConfigVal.GuestBlockSocialRelations) {
        memcpy(&info.m_backup.KnowledgeChao, &pParam->knowledge.chao, sizeof(pParam->knowledge.chao));
    }

    info.m_backup.body = pParam->body;

    for(size_t s = 0; s < 8; ++s) {
        info.m_backup.Exp[s] = pParam->Exp[s];
        info.m_backup.Abl[s] = pParam->Abl[s];
        info.m_backup.Lev[s] = pParam->Lev[s];
        info.m_backup.Skill[s] = pParam->Skill[s];
    }

    if (gConfigVal.GuestBlockBreeding) {
        info.m_backup.EmotionBreed = pParam->emotion.State[EM_ST_BREED - EM_ST_SLEEPY];
    }

    if (gConfigVal.GuestBlockVisualChanges) {
        info.m_backup.ShinyJewelMonotone = pCweParam->ShinyJewelMonotone;
        info.m_backup.LobbyTextureValue = pCweParam->LobbyTextureValue;
        info.m_backup.EyeAlignment = pCweParam->EyeAlignment;
        info.m_backup.EyeColor = pCweParam->EyeColor;
        info.m_backup.DCWings = pCweParam->DCWings;
        info.m_backup.Negative = pCweParam->Negative;
    }

    GET_CWEPARAM(&info.m_saveInfo)->IsGuest = TRUE;

    info.m_hasSpawnedEverYet = false;
}

void Guest_SaveAllChao() {
    for(size_t i = 0; i < GuestMax; ++i) {
        if (!GuestInfoList[i].m_occupied) continue;

        SaveGuestChao(i);
    }
}

static void LoadFirstGuestChao() {
    for(size_t i = 0; i < GuestMax; ++i) {
        LoadGuestChao(i, GuestChaoFilePathIndices[i]);
    }
}

static void RerollGuestChao() {
    if (gConfigVal.GuestRollType == GUEST_ROLL_RANDOM) {
        if (gConfigVal.GuestSave) {
           Guest_SaveAllChao();
        }

        ShuffleIndices();
        LoadFirstGuestChao();

        return;
    }

    // rotation modes start here

    /* we load the first GuestMax number of chao if this is on first load */
    if(!RotationWindowIndex) {
        LoadFirstGuestChao();
        RotationWindowIndex += GuestMax;

        return;
    }

    // rotate out stale chao
    for(size_t i = RotateCount; i < GuestMax; ++i) {
        GuestInfoList[i - RotateCount] = GuestInfoList[i];

        if (gConfigVal.GuestSave) {
            SaveGuestChao(i);
        }
    }

    // rotate in new chao
    const size_t rotateInStart = GuestMax - RotateCount;
    for(size_t i = rotateInStart; i < GuestMax; ++i) {
        const size_t rotationIndex = (RotationWindowIndex + i - rotateInStart) % GuestChaoFilePathIndices.size();

        LoadGuestChao(i, GuestChaoFilePathIndices[rotationIndex]);
    }

    RotationWindowIndex += RotateCount;
}

extern "C" void OnChaoData(CHAO_PARAM_GC& info);

static void GuestManagerExecutor(task* tp) {
    taskwk* wk = tp->twp;

    for (size_t i = 0; i < GuestMax; ++i) {
        ChaoTranspiledMainCode(GuestInfoList[i].m_saveInfo);
        OnChaoData(GuestInfoList[i].m_saveInfo.param);
    }

    switch(wk->mode) {
        case 0: {
            const size_t count = GuestMin + size_t(njRandom() * float(GuestMax - GuestMin + 1));
            for(size_t i = 0; i < count; ++i) {
                auto& info = GuestInfoList[i];

                if(pLastHoldingChaoSaveInfo == &info.m_saveInfo) {
                    continue;
                }
                
                assert(info.m_occupied);

                NJS_POINT3& pos = ProbablyChaoSpawnPoints[(AL_GetStageNumber() - 1) * 16 + (int)(njRandom() * 15.f)];
                task* pChaoTask = CreateChaoExtra(&info.m_saveInfo.param, 0, NULL, &pos, Angle(njRandom() * 360.f));

                if (!info.m_hasSpawnedEverYet && gConfigVal.GuestRandomizeEmotions) {
                    auto pParam = GET_CHAOPARAM(pChaoTask);

                    const auto lifeTimer = pParam->LifeTimer;
                    const auto moodTimer = pParam->emotion.MoodTimer;
                    const auto timer = pParam->emotion.Timer;
                    const auto illTimer = pParam->emotion.IllTimer;
                    const auto kwTimer = pParam->knowledge.KwTimer;

                    const size_t calcCount = 4 + size_t(njRandom() * 12.f);
                    for(size_t p = 0; p < calcCount; ++p) {
                        // prevents these from ticking
                        pParam->emotion.IllTimer = 0;
                        pParam->knowledge.KwTimer = 0;
                        pParam->LifeTimer = 0;

                        pParam->emotion.MoodTimer = GET_GLOBAL()->ChaoTimerMoodInterval;
                        pParam->emotion.Timer = GET_GLOBAL()->ChaoTimerEmotionInterval;

                        AL_CalcParameter(pChaoTask);
                    }

                    pParam->LifeTimer = lifeTimer;
                    pParam->emotion.MoodTimer = moodTimer;
                    pParam->emotion.Timer = timer;
                    pParam->emotion.IllTimer = illTimer;
                    pParam->knowledge.KwTimer = kwTimer;
                }

                if (AL_EmotionGetValue(pChaoTask, EM_ST_SLEEP_DEPTH)) {
                    AL_SetBehavior(pChaoTask, ALBHV_Sleep);
                }

                info.m_hasSpawnedEverYet = true;
            }

            wk->mode++;
        }
        case 1:
            for(size_t i = 0; i < GuestMax; ++i) {
                auto& info = GuestInfoList[i];
                CHAO_PARAM_GC* pParam = &info.m_saveInfo.param;
                
                if(gConfigVal.GuestBlockStatChanges) {
                    for(size_t s = 0; s < 8; ++s) {
                        pParam->Exp[s] = info.m_backup.Exp[s];
                        pParam->Abl[s] = info.m_backup.Abl[s];
                        pParam->Lev[s] = info.m_backup.Lev[s];
                        pParam->Skill[s] = info.m_backup.Skill[s];
                    }
                }

                if (gConfigVal.GuestBlockPlayerRelations) {
                    pParam->like = info.m_backup.like;
                    memcpy(&pParam->knowledge.player, &info.m_backup.KnowledgePlayer, sizeof(pParam->knowledge.player));
                }

                if (gConfigVal.GuestBlockSocialRelations) {
                    memcpy(&pParam->knowledge.chao, &info.m_backup.KnowledgeChao, sizeof(pParam->knowledge.chao));
                }

                if (gConfigVal.GuestBlockLifeChanges) {
                    pParam->age = info.m_backup.age;
                    pParam->old = info.m_backup.old;
                    pParam->life = info.m_backup.life;
                    pParam->LifeMax = info.m_backup.LifeMax;
                    pParam->nbSucceed = info.m_backup.nbSucceed;
                }

                if (gConfigVal.GuestBlockBodyChanges) {
                    pParam->body.HPos = info.m_backup.body.HPos;
                    pParam->body.VPos = info.m_backup.body.VPos;
                    pParam->body.APos = info.m_backup.body.APos;
                    pParam->body.growth = info.m_backup.body.growth;
                }

                if (gConfigVal.GuestBlockVisualChanges) {
                    CHAO_PARAM_CWE* pCweParam = GET_CWEPARAM(pParam);

                    pParam->body.EyeColor = info.m_backup.body.EyeColor;
                    pParam->body.HonbuNum = info.m_backup.body.HonbuNum;
                    pParam->body.HonbuColorNum = info.m_backup.body.HonbuColorNum;
                    pParam->body.ObakeBody = info.m_backup.body.ObakeBody;
                    pParam->body.ColorNum = info.m_backup.body.ColorNum;
                    pParam->body.NonTex = info.m_backup.body.NonTex;
                    pParam->body.JewelNum = info.m_backup.body.JewelNum;
                    pParam->body.MultiNum = info.m_backup.body.MultiNum;
                    pParam->body.EggColor = info.m_backup.body.EggColor;
                    pParam->body.FormNum = info.m_backup.body.FormNum;

                    pCweParam->ShinyJewelMonotone = info.m_backup.ShinyJewelMonotone;
                    pCweParam->LobbyTextureValue = info.m_backup.LobbyTextureValue;
                    pCweParam->EyeAlignment = info.m_backup.EyeAlignment;
                    pCweParam->EyeColor = info.m_backup.EyeColor;
                    pCweParam->DCWings = info.m_backup.DCWings;
                    pCweParam->Negative = info.m_backup.Negative;
                }
            }
            break;
    }
}

void GuestManagerCreate() {
    task* tp = CreateElementalTask(IM_TWK, LEV_4, GuestManagerExecutor, "GuestManagerExecutor");
    tp->twp->mode = 0;
}

static void CWE_ScanForGuestChao() {
    const auto &guestFolder = std::filesystem::path("resource/gd_PC/SAVEDATA") / "Guest";
    DWORD dwAttrib = GetFileAttributesW(guestFolder.c_str());

    if (dwAttrib == INVALID_FILE_ATTRIBUTES || !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY)) {
        return;
    }
    
    const auto findSearchPath = (guestFolder / "*.chao");

    _WIN32_FIND_DATAW newdata;
    HANDLE newhfind = FindFirstFileW(findSearchPath.c_str(), &newdata);
    if (newhfind != INVALID_HANDLE_VALUE) {
        do {
            const auto& path = (guestFolder / newdata.cFileName);

            if(!ValidateChaoFile(path.c_str())) {
                wchar_t msgBuf[sizeof(newdata.cFileName) + 128];
                swprintf_s(msgBuf, L"\"%s\" is not a valid .chao file, will be skipped.", newdata.cFileName);

                MessageBoxW(0, msgBuf, L"Chao World Extended: Guest", MB_ICONWARNING);

                continue;
            }

            GuestChaoFilePaths.push_back(path);
        } while (FindNextFileW(newhfind, &newdata) != 0);
        FindClose(newhfind);
    }

    GuestChaoFilePathIndices.resize(GuestChaoFilePaths.size());
    for(size_t i = 0; i < GuestChaoFilePaths.size(); ++i) {
        GuestChaoFilePathIndices[i] = i;
    }
}

static FunctionHook<task*> AL_CreateHoldingChao_t(0x531E10);
static task* AL_CreateHoldingChao_r() {
    if (AL_IsGarden() && !--VisitCounter) {
        VisitCounter = gConfigVal.GuestVisitCounter;
        RerollGuestChao();

        if(pHoldingChaoSaveInfo) {
            switch(gConfigVal.GuestRollType) {
                case GUEST_ROLL_RANDOM:
                    for(size_t i = 0; i < GuestMax; ++i) {
                        if (pHoldingChaoSaveInfo == &GuestInfoList[i].m_saveInfo) {
                            pHoldingChaoSaveInfo = NULL;
                            break;
                        }
                    }
                    break;

                case GUEST_ROLL_ROTATE:
                case GUEST_ROLL_ROTATE_RANDOM:
                    CHAO_SAVE_INFO* pInfo = NULL;

                    for(size_t i = RotateCount; i < GuestMax; ++i) {
                        if(pHoldingChaoSaveInfo == &GuestInfoList[i].m_saveInfo) {
                            pInfo = &GuestInfoList[i - RotateCount].m_saveInfo;
                            break;
                        }
                    }

                    pHoldingChaoSaveInfo = pInfo;
                    break;
            }
        }
    }

    /* we store it because the original func zeroes it out */
    pLastHoldingChaoSaveInfo = pHoldingChaoSaveInfo;

    return AL_CreateHoldingChao_t.Original();
}

static FunctionHook<int, task*> ALBHV_Thrown_t(0x564320);
static int ALBHV_Thrown_r (task* tp) {
    const bool isGuest = AL_ParameterIsGuest(tp);
    Uint8 obakeHead;
    
    if (isGuest) {
        obakeHead = GET_CHAOPARAM(tp)->body.ObakeHead;
        GET_CHAOPARAM(tp)->body.ObakeHead = 0;
    }

    int ret = ALBHV_Thrown_t.Original(tp);

    if (isGuest) {
        GET_CHAOPARAM(tp)->body.ObakeHead = obakeHead;
    }

    return ret;
}

static ASM_FUNC int AL_CheckDamage(task* tp) {
    // save regs
    ASM_PUSH( ebx );

    // arguments
    ASM_MOVE( ebx, ASM_ESP(1+0+1) ); // a2

    // call
    ASM_CALL_R( edx, 0x563D30 );

    // restore regs
    ASM_POP( ebx );

    // return
    ASM_RET( 0 );
}

static int AL_CheckDamage_r (task* tp) {
    const bool isGuest = AL_ParameterIsGuest(tp);
    Uint8 obakeHead;
    
    if (isGuest) {
        obakeHead = GET_CHAOPARAM(tp)->body.ObakeHead;
        GET_CHAOPARAM(tp)->body.ObakeHead = 0;
    }

    int ret = AL_CheckDamage(tp);

    if (isGuest) {
        GET_CHAOPARAM(tp)->body.ObakeHead = obakeHead;
    }

    return ret;
}

static void ASM_FUNC AL_CheckDamage_t() {
    ASM_PUSH(ebx);

	ASM_PUSH(ebx);
	ASM_CALL(AL_CheckDamage_r);
	ASM_ESP_ADD(1);

	ASM_POP(ebx);
	ASM_RET(0);
}

void GuestIndicatorDraw(task* tp) {
    if(!AL_ParameterIsGuest(tp)) return;
    if(!playerpwp[0] || playerpwp[0]->htp != tp) return;

    static NJS_TEXTURE_VTX tex[4] = {
        { -1, -1.0, 0, 0, 0, 0xFFFFFFFF },
        { -1,  1.0, 0, 0,  1,   0xFFFFFFFF },
        { 1, -1.0, 0, 1, 0, 0xFFFFFFFF },
        { 1, 1.0, 0, 1, 1, 0xFFFFFFFF }
    };

    NJS_POINT3 screenPos;
    sub_426CC0(_nj_current_matrix_ptr_, &screenPos, &tp->twp->pos, 0);

    njPushUnitMatrix();

    njTranslateEx(&screenPos);
    njTranslate(NULL, 3, 2.5f, 0);
    njScale(NULL, 1, -1, 1);
    
    njSetTexture(&AL_ODE_GUEST_TEXLIST);
    njSetTextureNum(1);

    njDrawTexture3DExSetData(tex, 4);
    njPopMatrixEx();
}

void Guest_ForceReroll() {
    VisitCounter = 1;
    RotationWindowIndex = 0;
}

void CWE_GuestInit() {
    CWE_ScanForGuestChao();

    if(GuestChaoFilePathIndices.empty()) {
        gConfigVal.GuestChao = false;
        return;
    }

    Guest_ForceReroll();
    GuestMin = NJM_MIN(gConfigVal.GuestMin, GuestChaoFilePathIndices.size());
    GuestMax = NJM_MIN(gConfigVal.GuestMax, GuestChaoFilePathIndices.size());
    RotateCount = NJM_MIN(gConfigVal.GuestRotateCount, GuestChaoFilePathIndices.size());

    GuestMin = NJM_MIN(GuestMin, GUEST_CHAO_MAX);
    GuestMax = NJM_MIN(GuestMax, GUEST_CHAO_MAX);
    RotateCount = NJM_MIN(RotateCount, GUEST_CHAO_MAX);

    GuestMin = NJM_MIN(GuestMin, GuestMax);

    if(gConfigVal.GuestRollType == GUEST_ROLL_ROTATE_RANDOM) {
        ShuffleIndices();
    }

    AL_CreateHoldingChao_t.Hook(AL_CreateHoldingChao_r);

    if (gConfigVal.GuestBlockWearableChanges) {
        ALBHV_Thrown_t.Hook(ALBHV_Thrown_r);
        WriteCall((void*)0x53DBF0, (void*)AL_CheckDamage_t);
    }
}