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

extern CWE_REGAPI cweAPI;

extern std::vector<std::pair<const char*, NJS_TEXLIST*>> TexlistLoads;

extern std::unordered_set<int> ModAPI_SpecialAction;
extern std::vector<std::pair<SpecialItemFuncPtr, SpecialConditionFuncPtr>> specialItemFuncs;

extern std::vector<const char*> MsgAlItem;

extern std::vector<MotionTableAction> chaoAnimations;
extern std::map<std::string, int> registeredAnimations;

extern std::vector<LastBiteFruitFuncPtr> lastBiteFruit;

extern std::unordered_map<int, int> ModAPI_LensColorMap;
extern std::vector<NJS_TEXLIST*> ModAPI_EyeColors;

extern "C" 
{
	__declspec(dllexport) void Chao_RegAnimation(ObjectMaster* a1, const std::string& name);
	__declspec(dllexport) void Chao_RegAnimationTbl(MotionTableData* a1, const std::string& name);
	__declspec(dllexport) void RegisterDataFunc(RegisterDataFuncPtr ptr);
}

/*
extern "C" {
	int RegisterFoName(char* str);
	int RegisterAlItemString(const char* str);
	__declspec(dllexport) void SetRebuyFlag(int Category, int ID, bool rebuy);
	__declspec(dllexport) void SetFruitTexlist(int ID, NJS_TEXLIST* texlist);
	__declspec(dllexport) void RegisterBlackMarketGeneralFruit(int ID, int chance);
	__declspec(dllexport) void RegisterBlackMarketRareFruit(int ID, int chance);
	__declspec(dllexport) int RegisterChaoTree(NJS_OBJECT* seed, NJS_OBJECT* tree1, NJS_OBJECT* tree2, NJS_OBJECT* tree3, NJS_TEXLIST* texlist, BlackMarketItemAttributes* attrib, const char* name, const char* description);
	__declspec(dllexport) int RegisterChaoFruit(NJS_OBJECT* model, NJS_TEXLIST* texlist, ChaoItemStats* stats, BlackMarketItemAttributes* attrib, LastBiteFruitFuncPtr funcPtr = 0, const char* name = 0, const char* description = 0);
	__declspec(dllexport) int RegisterChaoHat(NJS_OBJECT* model, NJS_TEXLIST* texlist, BlackMarketItemAttributes* attrib, const char* name = 0, const char* description = 0, bool hideHead = true);
	__declspec(dllexport) int RegisterChaoAccessory(EAccessoryType type, NJS_OBJECT* model, NJS_TEXLIST* texlist, BlackMarketItemAttributes* attrib, const char* name, const char* description);
	__declspec(dllexport) void AccessoryMakeBald(int accID);
	__declspec(dllexport) int RegisterChaoSpecial(NJS_OBJECT* model, NJS_TEXLIST* texlist, BlackMarketItemAttributes* attrib, SpecialItemFuncPtr func, SpecialConditionFuncPtr cond = 0, const char* name = 0, const char* description = 0, bool isAction = false);
	__declspec(dllexport) void Chao_RegAnimation(ObjectMaster* a1, std::string name);
	void Chao_RegAnimationTbl(MotionTableData* a1, std::string name);
	__declspec(dllexport) void RegisterDataFunc(RegisterDataFuncPtr ptr);
	__declspec(dllexport) void RegisterSaveLoad(const char* suffix, void* buffer, int size);
	__declspec(dllexport) MotionTableAction* GetChaoAnimations();
	__declspec(dllexport) int RegisterChaoAnimation(std::string name, MotionTableAction* action);
	__declspec(dllexport) int GetChaoAnimationIndex(std::string name);
	__declspec(dllexport) void RegisterChaoTexlistLoad(const char* name, NJS_TEXLIST* load);
	__declspec(dllexport) int  RegisterChaoMinimal(CWE_MINIMAL* minimalentry);
	__declspec(dllexport) void RegisterChaoMinimalFruit(int fruitID, int minimalID, int chanceMin, int chanceMax);
	__declspec(dllexport) void RegisterEyeColor(const char* textureName, NJS_TEXLIST* texlist, int specialID);
}
*/
void CallRegisteredHooks(CWE_REGAPI* cwe_api);
void AL_ModAPI_UpdatePtr();
void Animation_Init();
void RegisterCWEData(CWE_REGAPI* cwe_api);
void AL_ModAPI_Init();
