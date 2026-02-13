#pragma once

#include "cwe_api_common.h"

#define CWE_API_REGISTER_CHAO_VER 1

#define ICON_TYPE_BALL 0
#define ICON_TYPE_HALO 1
#define ICON_TYPE_SPIKY 2
#define ICON_TYPE_CUSTOM 3 //don't use this for now, since pIconData isnt finished

#define CUSTOM_CHAO_FLAG_SECOND_EVO BIT_0 //only for advanced users for now since we don't have safety checks in place for it yet
#define CUSTOM_CHAO_FLAG_BALD_HIDE_PARTS_NON_ADJACENT BIT_1

struct CWE_API_CHAO_DATA {
	NJS_OBJECT* pObject; //chunk model format (sa2mdl)
	NJS_OBJECT* pSecondEvoList[5];

	const char* TextureName; //we don't support texlists because we need to "inject" the loaded textures into al_body
	int TextureCount;
	Uint32 IconColor; // emotion ball color
	Uint32 IconType; // use the ICON_TYPE defines for setting this
	void* pIconData; //placeholder for custom emotion balls in the future

	bool (*pEvolveFunc) (ObjectMaster* tp); //if function returns true during evolution, chao will evolve into this type

	Uint32 Flags;
	const char* Name; //we use it for the health center for now, also useful for KCE, ex. "Spartoi"
	char ID[16]; //type id, HAS TO BE NULL TERMINATED, ex. "cwe_spartoi"

	Sint32 reserved[4];
};

struct CWE_API_REGISTER_CHAO {
	int Version;

	size_t(*AddChaoType)(CWE_API_CHAO_DATA const* pData);
};