#include "stdafx.h"

#include <cwe_api.h>

#include <api/api_customchao.h>
#include <api/api_accessory.h>
#include <api/api_texture.h>
#include <api/api_msg.h>
#include <ChaoMain.h>

static CWE_API_REGISTER CWE_API_Register = {
    .Version = CWE_API_REGISTER_VER,

    .pChao = &AL_ModAPI_Chao,
    .pAccessory = &AL_ModAPI_Accessory,
    .pTexture = &AL_ModAPI_Texture,
    .pMsg = &AL_ModAPI_Msg
};

CWE_API CWE_API_Main = {
    .Version = CWE_API_VER,

    .pLegacy = &CWE_API_Legacy,
    .pRegister = &CWE_API_Register
};

// a lot of AL_ModApi.cpp stuff will end up here too
// and we'll hopefully remove it, very legacy file

struct CWE_API_MOD_ENTRY {
    using InitPtr = void(*)();
    using APILoadPtr = void(*)(const CWE_API const* pAPI);

    // todo: i'm not sure honestly if we can just use the pointers from the Mod struct or not
    std::string m_id;
    std::string m_name;

    InitPtr m_earlyInit;
    InitPtr m_lateInit;

    APILoadPtr m_earlyLoad;
    APILoadPtr m_load;

public:
    static std::optional<CWE_API_MOD_ENTRY> Create(const Mod* pMod) {
        CWE_API_MOD_ENTRY entry;

        entry.m_earlyInit = reinterpret_cast<InitPtr>(GetProcAddress(pMod->DLLHandle, "CWEAPI_EarlyInit"));
        entry.m_lateInit = reinterpret_cast<InitPtr>(GetProcAddress(pMod->DLLHandle, "CWEAPI_LateInit"));

        entry.m_earlyLoad = reinterpret_cast<APILoadPtr>(GetProcAddress(pMod->DLLHandle, "CWEAPI_EarlyLoad"));
        entry.m_load = reinterpret_cast<APILoadPtr>(GetProcAddress(pMod->DLLHandle, "CWEAPI_Load"));
    
        if(!entry.m_earlyInit && !entry.m_lateInit && !entry.m_earlyLoad && !entry.m_load) {
            return std::nullopt;
        }

        // todo: can this occur? rn this isn't read anyways so i'll safety check to be safe
        if(pMod->ID) {
            entry.m_id = {pMod->ID};
        }

        entry.m_name = {pMod->Name};

        return entry;
    }
};

static std::vector<CWE_API_MOD_ENTRY> ModEntries;

void CWE_API_FindMods() {
    for(size_t i = 0; i < g_HelperFunctions->Mods->size(); ++i) {
        const auto& mod = g_HelperFunctions->Mods->at(i);

        const auto result = CWE_API_MOD_ENTRY::Create(&mod);
        if(result) {
            ModEntries.push_back(*result);
        }
    }
}

void CWE_API_EarlyInit() {
    for(const auto& mod : ModEntries) {
        mod.m_earlyInit();
    }
}

void CWE_API_LateInit() {
    for(const auto& mod : ModEntries) {
        mod.m_lateInit();
    }
}

void CWE_API_EarlyLoad() {
    for(const auto& mod : ModEntries) {
        mod.m_earlyLoad(&CWE_API_Main);
    }
}

void CWE_API_Load() {
    for(const auto& mod : ModEntries) {
        mod.m_load(&CWE_API_Main);
    }
}