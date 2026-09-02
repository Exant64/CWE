#include "stdafx.h"
#include "al_garden_info.h"
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

#ifdef IMGUIDEBUG
#include "imgui/imgui.h"
#endif

#define GUEST_CHAO_MAX 8

struct GuestInfo {
    bool m_occupied;
    CHAO_SAVE_INFO m_saveInfo;
    size_t m_pathIndex;

    struct BackupData {
        Uint8 Exp[8];
        char Abl[8];
        char Lev[8];
        __int16 Skill[8];
        
        Sint8 place;
    } m_backup;
};

static int RotationWindowIndex;
static bool RerollFlag = false;

static int VisitCounter;
static GuestInfo GuestInfoList[GUEST_CHAO_MAX];
static std::vector<size_t> GuestChaoFilePathIndices;
static std::vector<std::string> GuestChaoFilePaths;

#ifdef IMGUIDEBUG
void Guest_Debug(bool& open) {
    if(open && ImGui::Begin("Guest", &open)) {
        ImGui::InputInt("RotationWindow", &RotationWindowIndex);
        ImGui::InputInt("VisitCounter", &VisitCounter);

        if(ImGui::TreeNode("Paths")) {
            for(size_t i = 0; i < GuestChaoFilePaths.size(); ++i) {
                ImGui::Text("%d: %s", int(i), GuestChaoFilePaths[i].c_str());
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

            ImGui::InputInt("GuestVisitCounter", (int*)&gConfigVal.GuestVisitCounter);
            ImGui::InputInt("GuestMin", (int*)&gConfigVal.GuestMin);
            ImGui::InputInt("GuestMax", (int*)&gConfigVal.GuestMax);
            ImGui::InputInt("GuestRollType", (int*)&gConfigVal.GuestRollType);
            ImGui::InputInt("GuestRotateCount", (int*)&gConfigVal.GuestRotateCount);

            ImGui::Checkbox("GuestRandomizeEmotions", &gConfigVal.GuestRandomizeEmotions);
            ImGui::Checkbox("GuestBlockStatChanges", &gConfigVal.GuestBlockStatChanges);

            ImGui::TreePop();
        }

        ImGui::End();
    }
}
#endif

static void ShuffleIndices() {
    for(size_t i = GuestChaoFilePathIndices.size(); i != 0; --i) {
        size_t randIndex = size_t(njRandom() * (float(i) - 0.001f));

        std::swap(GuestChaoFilePathIndices[i], GuestChaoFilePathIndices[randIndex]);
    }
}

static bool ValidateChaoFile(const char* path) {
    const size_t headerSize = 64;
    FILE* chaoFile;

    fopen_s(&chaoFile, path, "rb");

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

    assert(info.m_occupied);

    info.m_saveInfo.param.place = info.m_backup.place;
    GET_CWEPARAM(&info.m_saveInfo)->IsGuest = FALSE;

    const auto path = GuestChaoFilePaths[info.m_pathIndex].c_str();
    if(!SaveChaoFile(path, &info.m_saveInfo)) {
        char* pMsgBuf = new char[strlen(path) + 21 + 1];
        sprintf(pMsgBuf, "\"%s\" failed to save!", path);

        MessageBoxA(0, pMsgBuf, "Chao World Extended: Guest", MB_ICONERROR);

        delete[] pMsgBuf;
    }
}

static void SaveAllGuestChao() {
    for(size_t i = 0; i < gConfigVal.GuestMax; ++i) {
        SaveGuestChao(i);
    }
}

void LoadGuestChao(size_t infoIndex, size_t pathIndex) {
    auto& info = GuestInfoList[infoIndex];

    info.m_occupied = true;
    info.m_pathIndex = pathIndex;
    info.m_saveInfo = LoadChaoFile(GuestChaoFilePaths[pathIndex].c_str());

    info.m_backup.place = info.m_saveInfo.param.place;

    for(size_t s = 0; s < 8; ++s) {
        info.m_backup.Exp[s] = info.m_saveInfo.param.Exp[s];
        info.m_backup.Abl[s] = info.m_saveInfo.param.Abl[s];
        info.m_backup.Lev[s] = info.m_saveInfo.param.Lev[s];
        info.m_backup.Skill[s] = info.m_saveInfo.param.Skill[s];
    }

    GET_CWEPARAM(&info.m_saveInfo)->IsGuest = TRUE;
}

static void LoadFirstGuestChao() {
    for(size_t i = 0; i < gConfigVal.GuestMax; ++i) {
        LoadGuestChao(i, GuestChaoFilePathIndices[i]);
    }
}

static void RerollGuestChao() {
    RerollFlag = true;

    if (gConfigVal.GuestRollType == GUEST_ROLL_RANDOM) {
        if (gConfigVal.GuestSave) {
           SaveAllGuestChao();
        }

        ShuffleIndices();
        LoadFirstGuestChao();

        return;
    }

    // rotation modes start here

    /* we load the first GuestMax number of chao if this is on first load */
    if(!RotationWindowIndex) {
        LoadFirstGuestChao();
        RotationWindowIndex += gConfigVal.GuestMax;

        return;
    }

    // rotate out stale chao
    for(size_t i = gConfigVal.GuestRotateCount; i < gConfigVal.GuestMax; ++i) {
        if (gConfigVal.GuestSave) {
            SaveGuestChao(i);
        }

        GuestInfoList[i - gConfigVal.GuestRotateCount] = GuestInfoList[i];
    }

    // rotate in new chao
    const size_t rotateInStart = gConfigVal.GuestMax - gConfigVal.GuestRotateCount;
    for(size_t i = rotateInStart; i < gConfigVal.GuestMax; ++i) {
        const size_t rotationIndex = (RotationWindowIndex + i - rotateInStart) % GuestChaoFilePathIndices.size();

        LoadGuestChao(i, rotationIndex);
    }

    RotationWindowIndex += gConfigVal.GuestRotateCount;
}

static void GuestManagerExecutor(task* tp) {
    taskwk* wk = tp->twp;

    switch(wk->mode) {
        case 0: {
            if(!--VisitCounter) {
                VisitCounter = gConfigVal.GuestVisitCounter - 1;
                RerollGuestChao();
            }

            const size_t count = gConfigVal.GuestMin + size_t(njRandom() * float(gConfigVal.GuestMax - gConfigVal.GuestMin + 1));
            for(size_t i = 0; i < count; ++i) {
                auto& info = GuestInfoList[i];
                NJS_POINT3& pos = ProbablyChaoSpawnPoints[(AL_GetStageNumber() - 1) * 16 + (int)(njRandom() * 15.f)];

                assert(info.m_occupied);
                
                CreateChaoExtra(&info.m_saveInfo.param, 0, NULL, &pos, Angle(njRandom() * 360.f));
            }

            wk->mode++;
        }
        case 1:
            if(gConfigVal.GuestBlockStatChanges) {
                for(size_t i = 0; i < gConfigVal.GuestMax; ++i) {
                    auto& info = GuestInfoList[i];

                    for(size_t s = 0; s < 8; ++s) {
                        info.m_saveInfo.param.Exp[s] = info.m_backup.Exp[s];
                        info.m_saveInfo.param.Abl[s] = info.m_backup.Abl[s];
                        info.m_saveInfo.param.Lev[s] = info.m_backup.Lev[s];
                        info.m_saveInfo.param.Skill[s] = info.m_backup.Skill[s];
                    }
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
    
    const auto findSearchPath = (guestFolder / "*.chao").generic_string();

    _WIN32_FIND_DATAA newdata;
    HANDLE newhfind = FindFirstFileA(findSearchPath.c_str(), &newdata);
    if (newhfind != INVALID_HANDLE_VALUE) {
        do {
            const auto& path = (guestFolder / newdata.cFileName).generic_string();

            if(!ValidateChaoFile(path.c_str())) {
                char msgBuf[sizeof(newdata.cFileName) + 128];
                sprintf_s(msgBuf, "\"%s\" is not a valid .chao file, will be skipped.", newdata.cFileName);

                MessageBoxA(0, msgBuf, "Chao World Extended: Guest", MB_ICONWARNING);

                continue;
            }

            GuestChaoFilePaths.push_back(path);
        } while (FindNextFileA(newhfind, &newdata) != 0);
        FindClose(newhfind);
    }

    GuestChaoFilePathIndices.resize(GuestChaoFilePaths.size());
    for(size_t i = 0; i < GuestChaoFilePaths.size(); ++i) {
        GuestChaoFilePathIndices[i] = i;
    }
}

static FunctionHook<task*> AL_CreateHoldingChao_t(0x531E10);
static task* AL_CreateHoldingChao_r() {
    if(RerollFlag) {
        RerollFlag = false;

        switch(gConfigVal.GuestRollType) {
            case GUEST_ROLL_RANDOM:
                for(size_t i = 0; i < gConfigVal.GuestMax; ++i) {
                    if (pHoldingChaoSaveInfo == &GuestInfoList[i].m_saveInfo) {
                        pHoldingChaoSaveInfo = NULL;
                        break;
                    }
                }
                break;

            case GUEST_ROLL_ROTATE:
            case GUEST_ROLL_ROTATE_RANDOM:
                CHAO_SAVE_INFO* pInfo = NULL;

                for(size_t i = gConfigVal.GuestRotateCount; i < gConfigVal.GuestMax; ++i) {
                    if(pHoldingChaoSaveInfo == &GuestInfoList[i].m_saveInfo) {
                        pInfo = &GuestInfoList[i - gConfigVal.GuestRotateCount].m_saveInfo;
                        break;
                    }
                }

                pHoldingChaoSaveInfo = pInfo;
                break;
        }
    }

    return AL_CreateHoldingChao_t.Original();
}

void CWE_GuestInit() {
    CWE_ScanForGuestChao();

    VisitCounter = 1;

    if(gConfigVal.GuestRollType == GUEST_ROLL_ROTATE_RANDOM) {
        ShuffleIndices();
    }

    AL_CreateHoldingChao_t.Hook(AL_CreateHoldingChao_r);
}