#pragma once
#include <AL_ModAPI.h>
#include <vector>

//the internal data we actually use
struct CustomChaoEntry {
	NJS_TEXLIST BodyTexture; //the load texlist 
	size_t StartIndex; //the texname entry index in AL_BODY_TEXNAMES
	CWE_API_CHAO_DATA Data;
};

extern std::vector<CustomChaoEntry> CustomChaoTypeEntries;

void AL_ModAPI_CharacterChao_Init();
void AL_ModAPI_CharacterChao_Update();

size_t AddChaoType(CWE_API_CHAO_DATA const* pData);