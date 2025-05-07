#include "stdafx.h"
#include "AL_ModAPI.h"
#include "ALifeSDK_Functions.h"
#include "alo_fruit.h"
#include "ChaoMain.h"
#include "alo_obakehead.h"
#include "ninja_functions.h"
#include "Chao.h"
#include "al_sandhole.h"
#include "alo_accessory.h"
#include "alo_special.h"
#include "al_minimal.h"
#include "al_parts.h"
#include "al_save.h"
#include "al_growtree.h"
#include "al_modelcontainer.h"
#include "al_marketattr.h"
#include "al_odekake.h"
#include "api/api_msg.h"
#include "api/api_customchao.h"
#include "api/api_util.h"

#include <unordered_set>
#include <api/api_tree.h>
#include <kce_helper.h>

#pragma warning(push)
#pragma warning( disable: 4838 )
#include "data/alo_missing_tree.nja"
#include <al_draw.h>
#pragma warning(pop)

extern NJS_OBJECT object_alo_missing;

std::vector<MotionTableAction> chaoAnimations;
std::map<std::string, int> registeredAnimations;
std::vector<std::pair<const char*, NJS_TEXLIST*>> TexlistLoads;
std::vector<LastBiteFruitFuncPtr> lastBiteFruit;
std::vector<std::pair<SpecialItemFuncPtr, SpecialConditionFuncPtr>> specialItemFuncs;
std::vector<RegisterDataFuncPtr> RegisterDataHooks;

std::unordered_map<int, int> ModAPI_LensColorMap;
std::unordered_set<int> ModAPI_SpecialAction;
std::unordered_set<int> ModAPI_DisableJiggle;
std::unordered_set<int> ModAPI_BaldAccessory;

std::vector<NJS_TEXLIST*> ModAPI_EyeColors;

size_t AddChaoHat(NJS_OBJECT* model, NJS_TEXLIST* texlist, BlackMarketItemAttributes* attrib, const char* name, const char* description) {
	size_t ret = MaskObjObjectList.size();
	if (model == nullptr)
		MaskObjObjectList.push_back(std::make_pair(&object_alo_missing, &CWE_OBJECT_TEXLIST));
	else
		MaskObjObjectList.push_back(std::make_pair(model, texlist));

	BlackMarketAttributes::Get()->Add(ChaoItemCategory_Hat, attrib, name, description);
	return ret;
}

size_t AddChaoMinimal(CWE_MINIMAL* pMinimal) {
	ModAPI_MiniParts.push_back({ pMinimal->child, pMinimal->adult });

	ModAPI_MinimalModels.push_back(pMinimal->animalObject);

	cweAPI.RegisterChaoTexlistLoad(pMinimal->texlistName, pMinimal->animalTexlist);
	ModAPI_MinimalTexlists.push_back(pMinimal->animalTexlist);
	ModAPI_MinimalNames.push_back(pMinimal->Name ? pMinimal->Name : pMinimal->texlistName);

	ModAPI_MinimalMotion0.push_back(pMinimal->motions[0]);
	ModAPI_MinimalMotion1.push_back(pMinimal->motions[1]);
	ModAPI_MinimalMotion2.push_back(pMinimal->motions[2]);
	ModAPI_MinimalMotion3.push_back(pMinimal->motions[3]);
	ModAPI_MinimalStats.push_back(pMinimal->stats);
	ModAPI_MinimalInfluence.push_back(pMinimal->color);
	return ModAPI_MinimalInfluence.size() - 1;
}

void AccessoryDisableJiggle(int acccessory_id) {
	ModAPI_DisableJiggle.insert(acccessory_id);
}

extern "C"
{
	//COMPATBILITY LAYER
	__declspec(dllexport) int RegisterChaoHat(NJS_OBJECT* model, NJS_TEXLIST* texlist, BlackMarketItemAttributes* attrib, const char* name, const char* description, bool hideHead) {
		if (!hideHead) {
			return cweAPI.RegisterChaoAccessory(EAccessoryType::Head, model, texlist, attrib, name, description);
		}
		
		return AddChaoHat(model, texlist, attrib, name, description);
	}

	__declspec(dllexport) int RegisterChaoMinimal(CWE_MINIMAL* minimalentry) {
		//we only copy the legacy funcs
		CWE_MINIMAL legacyEntry = {
			.child = minimalentry->child,
			.adult = minimalentry->adult,
			.animalObject = minimalentry->animalObject,
			.texlistName = minimalentry->texlistName,
			.animalTexlist = minimalentry->animalTexlist,
			.motions = {
				minimalentry->motions[0],
				minimalentry->motions[1],
				minimalentry->motions[2],
				minimalentry->motions[3]
			},
			.stats = minimalentry->stats,
			.color = minimalentry->color,
			.Flags = 0,
			.ID = "",
			.Name = nullptr
		};

		return AddChaoMinimal(&legacyEntry);
	}

	__declspec(dllexport) void RegisterChaoTexlistLoad(const char* name, NJS_TEXLIST* load)
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

	__declspec(dllexport) void SetRebuyFlag(int Category, int ID, bool rebuy)
	{
		if (!rebuy) {
			AlItemRebuyable[Category].erase(ID);
		}
		else {
			AlItemRebuyable[Category].insert(ID);
		}
	}
	__declspec(dllexport) void SetFruitTexlist(int ID, NJS_TEXLIST* texlist)
	{
		ObjectRegistry::Get(ChaoItemCategory_Fruit)->Set(ID, texlist);
	}

	__declspec(dllexport) int RegisterChaoFruit(NJS_OBJECT* model, NJS_TEXLIST* texlist, ChaoItemStats* stats, BlackMarketItemAttributes* attrib, LastBiteFruitFuncPtr funcPtr, const char* name, const char* description)
	{
		int ret = ObjectRegistry::Get(ChaoItemCategory_Fruit)->Size();

		ObjectRegistry::Get(ChaoItemCategory_Fruit)->Add(model, texlist);
		ModAPI_FruitStats.push_back(*stats);
		BlackMarketAttributes::Get()->Add(ChaoItemCategory_Fruit, attrib, name, description);
		lastBiteFruit.push_back(funcPtr);

		return ret;
	}
	__declspec(dllexport) void RegisterDataFunc(RegisterDataFuncPtr ptr)
	{
		RegisterDataHooks.push_back(ptr);
	}
	__declspec(dllexport) void RegisterSaveLoad(const char* suffix, void* buffer, int size)
	{
		SaveFileAPIEntry entry = { suffix,buffer,size };
		ModAPI_SaveAPI.push_back(entry);
	}
	bool ALS_Default(ObjectMaster* chao, ObjectMaster* item)
	{
		return true;
	}
	void RegisterOtherItemFunc(int ID, OtherItemPtr func)
	{
		OtherItemFuncs.insert(std::make_pair(ID, (int)func));
	}
	__declspec(dllexport) int RegisterChaoSpecial(NJS_OBJECT* model, NJS_TEXLIST* texlist, BlackMarketItemAttributes* attrib, SpecialItemFuncPtr func, SpecialConditionFuncPtr cond, const char* name, const char* description, bool isAction)
	{
		int ret = ObjectRegistry::Get(ChaoItemCategory_Special)->Size();

		if (isAction) {
			ModAPI_SpecialAction.insert(ret);
		}

		specialItemFuncs.push_back(std::make_pair(func, (cond == nullptr) ? ALS_Default : cond));
		ObjectRegistry::Get(ChaoItemCategory_Special)->Add(model, texlist);
		BlackMarketAttributes::Get()->Add(ChaoItemCategory_Special, attrib, name, description);

		return ret;
	}
	__declspec(dllexport) size_t RegisterEyeColor(const char* textureName, NJS_TEXLIST* texlist, int specialID)
	{
		RegisterChaoTexlistLoad(textureName, texlist);

		size_t eyeCID = ModAPI_EyeColors.size();
		ModAPI_EyeColors.push_back(texlist);
		ModAPI_LensColorMap.insert(std::make_pair(specialID, eyeCID));
		return eyeCID + 1;
	}
	__declspec(dllexport) int RegisterChaoAccessory(EAccessoryType type, NJS_OBJECT* model, NJS_TEXLIST* texlist, BlackMarketItemAttributes* attrib, const char* name, const char* description)
	{
		int ret = ObjectRegistry::Get(ChaoItemCategory_Accessory)->Size();
		AccessoryTypeMap.insert(std::make_pair(ret, type));
		ObjectRegistry::Get(ChaoItemCategory_Accessory)->Add(model, texlist);
		BlackMarketAttributes::Get()->Add(ChaoItemCategory_Accessory, attrib, name, description);
		return ret;
	}

	__declspec(dllexport) void AccessoryMakeBald(int acccessory_id) {
		ModAPI_BaldAccessory.insert(acccessory_id);
		ModAPI_DisableJiggle.insert(acccessory_id);
	}

	__declspec(dllexport) void RegisterChaoMinimalFruit(int fruitID, int minimalID, int chanceMin, int chanceMax)
	{
		CWE_MINIMAL_FRUIT minimalFruit;
		minimalFruit.minimalID = minimalID;
		minimalFruit.chanceMin = chanceMin;
		minimalFruit.chanceMax = chanceMax;
		ModAPI_MinimalFruit[fruitID].push_back(minimalFruit);
	}

	void FruitErrorMsg(ItemChance_old* oldChance) {
		auto* attrib = BlackMarketAttributes::Get()->Attrib(ChaoItemCategory_Fruit, oldChance->item);
		bool validNameID = attrib && attrib->Name >= 0 && (size_t)attrib->Name < MsgAlItem.size();

		APIErrorUtil error("RegisterBlackMarketFruit error registering %s:", validNameID ? MsgAlItem[attrib->Name] : "Unknown Fruit");
		error.print("incompatible with CWE 9.4 or above, please update the mod, or alert the mod creator if you already did.");
	}

	__declspec(dllexport) void RegisterBlackMarketGeneralFruit(int ID, int chance) {
		// this is a legacy check for pretty old mods that still passed in an ItemChance (and the old one, without the valid ID system)
		if (chance > 100) {
			FruitErrorMsg((ItemChance_old*)&chance);
			return;
		}
		ItemChance itemChance = { (Uint16)ID, (Sint8)chance };
		GeneralFruitMarket.push_back(itemChance);
	}

	__declspec(dllexport) void RegisterBlackMarketRareFruit(int ID, int chance) {
		if (chance > 100) {
			FruitErrorMsg((ItemChance_old*)&chance);
			return;
		}

		ItemChance itemChance = { (Uint16)ID, (Sint8)chance };
		RareFruitMarket.push_back(itemChance);
	}

	__declspec(dllexport) int RegisterChaoAnimation(std::string name, MotionTableAction* action)
	{
		APIErrorUtil error("RegisterChaoAnimation error for animation \"%s\": ", name.c_str());

		if (!action) {
			error.print("action is null!");
			return -1;
		}

		if (!action->NJS_MOTION) {
			error.print("the action's animation is null! (missing file?)");
			return -1;
		}

		if (action->NJS_MOTION->type != (NJD_MTYPE_POS_0 | NJD_MTYPE_ANG_1)) {
			error.print("the animation's type is invalid! (scale keyframes in animation?)");
			return -1;
		}

		const NJS_MDATA2* mdata = reinterpret_cast<NJS_MDATA2*>(action->NJS_MOTION->mdata);
		for (size_t i = 0; i < AL_PART_END; i++) {
			for (size_t j = 0; j < 2; j++) {
				if (mdata[i].nb[j] && mdata[i].nb[j] > action->NJS_MOTION->nbFrame) {
					error.print("the animation's model count is invalid!");
					return -1;
				}
			}
		}

		int index = chaoAnimations.size();
		registeredAnimations.insert(std::make_pair(name, index));
		chaoAnimations.push_back(*action);
		return index;
	}

	int RegisterChaoAnimTransition(const std::string& from, const std::string& to) {
		APIErrorUtil error("RegisterChaoAnimTransition error (\"%s\"->\"%s\"): ", from.c_str(), to.c_str());
		
		if (!registeredAnimations.contains(from)) {
			error.print("the \"from\" animation is not registered!");
			return 0;
		}

		if (!registeredAnimations.contains(to)) {
			error.print("the \"to\" animation is not registered!");
			return 0;
		}

		chaoAnimations[registeredAnimations[from]].TransitionToID = registeredAnimations[to];
		return 1;
	}

	__declspec(dllexport) void Chao_RegAnimation(ObjectMaster* a1, const std::string& name) {
		AL_SetMotionLink(a1, registeredAnimations[name]);
	}

	void Chao_RegAnimationTbl(MotionTableData* a1, const std::string& name) {
		Chao_Animation(a1, registeredAnimations[name]);
	}

	__declspec(dllexport) int GetChaoAnimationIndex(const std::string& name) {
		return registeredAnimations[name];
	}
}

BlackMarketItemAttributes* GetItemAttr(ChaoItemCategory cat, int index) {
	return BlackMarketAttributes::Get()->Attrib(cat, index);
}

MotionTableAction* GetChaoAnimation(size_t index)
{
	if (index >= chaoAnimations.size()) return nullptr;

	return &chaoAnimations[index];
}

CWE_REGAPI cweAPI {
	.Version = 1,

	.RegisterFoName = RegisterFoName,
	.RegisterAlItemString = RegisterAlItemString,
	.OverwriteAlItemString = OverwriteAlItemString,

	.RegisterBlackMarketGeneralFruit = RegisterBlackMarketGeneralFruit,
	.RegisterBlackMarketRareFruit = RegisterBlackMarketRareFruit,
	.GetItemAttr = GetItemAttr,
	.RegisterOtherItemFunc = RegisterOtherItemFunc,
	.SetRebuyFlag = SetRebuyFlag,

	.AddChaoHat = AddChaoHat,

	.RegisterChaoFruit = RegisterChaoFruit,
	.SetFruitTexlist = SetFruitTexlist,
	
	.RegisterChaoAccessory = RegisterChaoAccessory,
	.AccessoryMakeBald = AccessoryMakeBald,
	.AccessoryDisableJiggle = AccessoryDisableJiggle,

	.AddChaoTree = AddChaoTree,
	
	.RegisterChaoSpecial = RegisterChaoSpecial,
	.RegisterEyeColor = RegisterEyeColor,

	.AddChaoType = AddChaoType,

	.AddChaoMinimal = AddChaoMinimal,
	.RegisterChaoMinimalFruit = RegisterChaoMinimalFruit,

	.RegisterChaoAnimation = RegisterChaoAnimation,
	.RegisterChaoAnimTransition = RegisterChaoAnimTransition,
	.GetChaoAnimationIndex = GetChaoAnimationIndex,
	.GetChaoAnimation = GetChaoAnimation,

	.RegisterChaoTexlistLoad = RegisterChaoTexlistLoad,
	.RegisterSaveLoad = RegisterSaveLoad,
	.AddOdekakeMenu = AddOdekakeMenu
};

void AL_ModAPI_UpdatePtr()
{
	//BlackMarketAttributes::Get()->Finalize(); //also shrinks to fit
	//ObjectRegistry::FinalizeAll();

	for (size_t i = 0; i < ModAPI_SaveAPI.size(); i++)
		memset(ModAPI_SaveAPI[i].pointer, 0, ModAPI_SaveAPI[i].fileSize);

	//custom anims
	g_HelperFunctions->HookExport("al_motion_table", chaoAnimations.data());
	
	AL_ModAPI_CharacterChao_Update();

	AL_Odekake_Update();

	KCE_Update();

	WriteData((ChaoItemStats**)0x535F28, (ChaoItemStats*)ModAPI_FruitStats.data());

	AL_ModAPI_Tree_Update();

	//animals
	WriteData((int*)0x535F31, (int)ModAPI_MinimalStats.data());
	WriteData((int*)0x548d9d, (int)ModAPI_MinimalMotion0.data());
	WriteData((int*)0x548da6, (int)ModAPI_MinimalMotion1.data());
	WriteData((int*)0x548db0, (int)ModAPI_MinimalMotion2.data());
	WriteData((int*)0x548dba, (int)ModAPI_MinimalMotion3.data());
}

void CallRegisteredHooks(CWE_REGAPI* cwe_api)
{
	for (size_t i = 0; i < RegisterDataHooks.size(); i++)
		RegisterDataHooks[i](cwe_api);
}

void Animation_Init()
{
	//Animation
	MotionTableAction* actions = (MotionTableAction*)GetDllData("al_motion_table");
	chaoAnimations.insert(chaoAnimations.end(), actions, &actions[622]);
	if (chaoAnimations.size() < 622)
		PrintDebug("Animation_Init: Couldnt copy animations!!!\n");
}

void RegisterCWEData(CWE_REGAPI* cwe_api)
{
	AL_ModAPI_Init();
	al_minimal_Init();

	CallRegisteredHooks(cwe_api);

	AL_Odekake_Finalize();

	int MissingItem = cwe_api->RegisterAlItemString("Missing Item");
	int MissingAcc = cwe_api->RegisterAlItemString("Accessory");
	int MissingHat = cwe_api->RegisterAlItemString("Hat");
	int MissingFruit = cwe_api->RegisterAlItemString("Fruit");
	int MissingSeed = cwe_api->RegisterAlItemString("Seed");
	for (int i = 0; i < 50; i++)
	{
		BlackMarketItemAttributes missing_attrib = { 2500,500,999, (short)MissingItem,-1,0 };
		ChaoItemStats missing_stats = { 0 };
		missing_attrib.Descriptions = MissingAcc;
		cwe_api->RegisterChaoAccessory(EAccessoryType::Head, &object_alo_missing, &CWE_OBJECT_TEXLIST, &missing_attrib, 0, 0);
		missing_attrib.Descriptions = MissingFruit;
		cwe_api->RegisterChaoFruit(&object_alo_missing, &CWE_OBJECT_TEXLIST, &missing_stats, &missing_attrib, 0, 0, 0);
		missing_attrib.Descriptions = MissingAcc;
		cwe_api->AddChaoHat(&object_alo_missing, &CWE_OBJECT_TEXLIST, &missing_attrib, 0, 0);

		const CWE_API_TREE_DATA missing_tree = {
			.pSeedObj = &object_alo_missing,

			.pSaplingObj = &object_alo_missing_tree,
			.pAdultObj = &object_alo_missing_tree,
			.pDeadObj = &object_alo_missing_tree,

			.pTexlist = &CWE_OBJECT_TEXLIST,

			.ID = "cwe_missing",
			.Flags = 0,

			.FruitIDs = {0,0,0},
		};
		missing_attrib.Descriptions = MissingSeed;
		cwe_api->AddChaoTree(missing_tree, &missing_attrib, nullptr, nullptr);
	}
	
	//copy pearl price which is 50 for some reason
	//HatBMAttributesMod[SADXHat_PearlEggShell].PurchasePrice = 1350;

	AL_ModAPI_UpdatePtr();
}

void AL_ModAPI_Init()
{
	AL_ModAPI_Msg_Init();

	//Fruit
	for (int i = SA2BFruit_ChaoGardenFruit; i < 24; i++)
	{
		ObjectRegistry::Get(ChaoItemCategory_Fruit)->Add(FruitModels[i], &AL_OBJECT_TEXLIST);
		ModAPI_FruitStats.push_back(ChaoFruitStatArray[i]);
		lastBiteFruit.push_back(nullptr);
	}
	
	AL_ModAPI_Tree_Init();

	//Black Market
	for (int i = 0; i < 4; i++)
	{
		ItemChance chance = { (Uint16)GeneralFruitChances[i].item , GeneralFruitChances[i].chance };
		GeneralFruitMarket.push_back(chance);
	}

	//custom chao
	AL_ModAPI_CharacterChao_Init();

	AL_Odekake_Init();

	//AL_ModAPI_UpdatePtr();
}