#include "stdafx.h"

#include <cwe_api.h>

#include <api/api_customchao.h>
#include <api/api_accessory.h>
#include <api/api_texture.h>
#include <api/api_msg.h>

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