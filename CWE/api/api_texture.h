#pragma once
#include "stdafx.h"
#include <vector>

extern std::vector<std::pair<const char*, NJS_TEXLIST*>> TexlistLoads;

extern "C" __declspec(dllexport) void RegisterChaoTexlistLoad(const char* name, NJS_TEXLIST* load);
NJS_TEXLIST* AddAutoTextureLoad(const char* pTextureName);