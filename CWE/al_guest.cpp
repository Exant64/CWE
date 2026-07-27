#include "stdafx.h"
#include <filesystem>
#include <al_stage.h>
#include <Chao.h>

static std::vector<std::filesystem::path> GuestChaoFilePaths;

void CWE_SpawnRandomGuestChao() {
    const auto chosenIndex = size_t(njRandom() * (float(GuestChaoFilePaths.size()) - 0.001f));
    const auto& chaoFilePath = GuestChaoFilePaths[chosenIndex];
    
    CHAO_SAVE_INFO chaoInfo;

    {
        FILE* chaoFile;
        fopen_s(&chaoFile, chaoFilePath.generic_string().c_str(), "rb");
        fseek(chaoFile, 64, 0);
        fread(&chaoInfo, 1, sizeof(chaoInfo), chaoFile);
        fclose(chaoFile);
    }

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
            GuestChaoFilePaths.push_back(guestFolder / newdata.cFileName);
        } while (FindNextFileA(newhfind, &newdata) != 0);
        FindClose(newhfind);
    }
}

void CWE_GuestInit() {
    CWE_ScanForGuestChao();
}