#include "stdafx.h"

#include <unordered_map>

#include <cwe_api.h>
#include <api/api_util.h>

static std::unordered_map<std::string, NJS_TEXLIST*> TextureLoadPairs;
std::vector<std::pair<const char*, NJS_TEXLIST*>> TexlistLoads;

FunctionPointer(int, njReleaseTexture, (NJS_TEXLIST* arg0), 0x0077F9F0);

extern "C" __declspec(dllexport) void RegisterChaoTexlistLoad(const char* name, NJS_TEXLIST* load)
{
	static APIErrorUtil error("RegisterChaoTexlistLoad error:");

	if (!name) {
		error.print("name is nullptr");
		return;
	}

	if (!load) {
		error.print("\"%s\"'s texlist is nullptr", name);
		return;
	}

	TexlistLoads.push_back(std::make_pair(name, load));
}

NJS_TEXLIST* AddAutoTextureLoad(const char* pTextureName) {
	if (TextureLoadPairs.contains(pTextureName)) {
		return TextureLoadPairs[pTextureName];
	}

	NJS_TEXLIST* pTexlist = LoadCharTextures((char*)pTextureName);
	njReleaseTexture(pTexlist);

	TextureLoadPairs[pTextureName] = pTexlist;

	RegisterChaoTexlistLoad(pTextureName, pTexlist);

	return pTexlist;
}

CWE_API_REGISTER_TEXTURE AL_ModAPI_Texture = {
	.Version = CWE_API_REGISTER_TEXTURE_VER,

	.AddChaoTexlistLoad = RegisterChaoTexlistLoad,
	.AddAutoTextureLoad = AddAutoTextureLoad 
};