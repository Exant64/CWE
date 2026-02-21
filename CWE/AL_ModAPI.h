#pragma once

#include "alg_kinder_bl.h"
#include <vector>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include "alo_accessory.h"
#include "al_minimal.h"

#include "cwe_api.h"

#define PLACEHOLDER_ITEM_COUNT 50

#define ICON_FLAG_HALO BIT_0

struct IconData {
	Uint32 Flags;
	Uint32 BallIndex; //if halo flag is on, it needs all 35 angle textures (top to bottom)
	Uint32 reserved; //animated emotion ball tex count in the future
	Uint32 ExclamationIndex;
	Uint32 QuestionIndex;
	Uint32 HeartIndex;
	Uint32 SwirlIndex;
};

using RegisterDataFuncPtr = void(__cdecl*)(CWE_REGAPI* cwe_api);

extern CWE_REGAPI CWE_API_Legacy;

extern std::unordered_set<int> ModAPI_SpecialAction;
extern std::vector<std::pair<SpecialItemFuncPtr, SpecialConditionFuncPtr>> specialItemFuncs;

extern std::vector<const char*> MsgAlItem;

extern std::vector<LastBiteFruitFuncPtr> lastBiteFruit;

extern std::unordered_map<int, int> ModAPI_LensColorMap;
extern std::vector<NJS_TEXLIST*> ModAPI_EyeColors;

extern "C" 
{
	__declspec(dllexport) void Chao_RegAnimation(ObjectMaster* a1, const std::string& name);
	__declspec(dllexport) void Chao_RegAnimationTbl(MotionTableData* a1, const std::string& name);
	__declspec(dllexport) void RegisterDataFunc(RegisterDataFuncPtr ptr);
}

void AL_ModAPI_Init();
