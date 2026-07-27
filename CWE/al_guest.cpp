#include "stdafx.h"
#include <filesystem>
#include <al_stage.h>
#include <Chao.h>

#define GUEST_CHAO_MAX 8

static CHAO_SAVE_INFO GuestChaoSaves[GUEST_CHAO_MAX];
static std::vector<std::filesystem::path> GuestChaoFilePaths;

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

static CHAO_SAVE_INFO LoadChaoFile(const char* path) {
    CHAO_SAVE_INFO chaoInfo;
    FILE* chaoFile;

    fopen_s(&chaoFile, path, "rb");
    fseek(chaoFile, 64, SEEK_SET);
    fread(&chaoInfo, 1, sizeof(chaoInfo), chaoFile);
    fclose(chaoFile);

    return chaoInfo;
}

void CWE_SpawnRandomGuestChao() {
    const auto chosenIndex = size_t(njRandom() * (float(GuestChaoFilePaths.size()) - 0.001f));
    const auto& chaoFilePath = GuestChaoFilePaths[chosenIndex];
    
    CHAO_SAVE_INFO chaoInfo = LoadChaoFile(chaoFilePath.generic_string().c_str());

    NJS_POINT3& pos = ProbablyChaoSpawnPoints[(AL_GetStageNumber() - 1) * 16 + (int)(njRandom() * 15.f)];
    CreateChaoExtra(&chaoInfo.param, 1, NULL, &pos, Angle(njRandom() * 360.f));
}

static void CWE_ScanForGuestChao() {
    const auto &guestFolder = std::filesystem::path("resource/gd_PC/SAVEDATA") / "Guest";
    DWORD dwAttrib = GetFileAttributes(guestFolder.c_str());

    if (dwAttrib == INVALID_FILE_ATTRIBUTES || !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY)) {
        return;
    }
    
    const auto findSearchPath = guestFolder / "*.chao";

    _WIN32_FIND_DATAA newdata;
    HANDLE newhfind = FindFirstFileA(findSearchPath.generic_string().c_str(), &newdata);
    if (newhfind != INVALID_HANDLE_VALUE)
    {
        do
        {
            const auto& path = guestFolder / newdata.cFileName;
            const auto pathCStr = path.generic_string().c_str();

            if(!ValidateChaoFile(pathCStr)) {
                char msgBuf[sizeof(newdata.cFileName) + 128];
                sprintf_s(msgBuf, "\"%s\" is not a valid .chao file, will be skipped.", newdata.cFileName);

                MessageBoxA(0, msgBuf, "Chao World Extended: Guest", MB_ICONWARNING);

                continue;
            }

            GuestChaoFilePaths.push_back(path);
        } while (FindNextFileA(newhfind, &newdata) != 0);
        FindClose(newhfind);
    }
}

void CWE_GuestInit() {
    CWE_ScanForGuestChao();
}