#pragma once

#include "cwe_api_common.h"
#include "cwe_api_accessory.h"
#include "cwe_api_chao.h"
#include "cwe_api_odekake.h"

#include <string>

#define CWE_API_VER 1
#define CWE_API_REGISTER_VER 1
#define CWE_API_REGISTER_MOTION_VER 1
#define CWE_API_REGISTER_TEXTURE_VER 1
#define CWE_API_REGISTER_MSG_VER 1

using OtherItemPtr = void(*)(int ID);
using LastBiteFruitFuncPtr = void(*)(ChaoData* chaoData, ObjectMaster* fruit);
using SpecialItemFuncPtr = void(*)(ObjectMaster* chao, ObjectMaster* item);
using SpecialConditionFuncPtr = bool(*)(ObjectMaster* chao, ObjectMaster* item);

struct CWE_API_TREE_DATA {
	NJS_OBJECT* pSeedObj;

	NJS_OBJECT* pSaplingObj;
	NJS_OBJECT* pAdultObj;
	NJS_OBJECT* pDeadObj;

	NJS_TEXLIST* pTexlist;

	char ID[16]; //use if possible, but it goes unused in cwe code for now

	Uint32 Flags; //unused for now

	int FruitIDs[3]; //the 3 fruit IDs to init with

	Sint32 reserved[4];
};

//animal stuff
struct CWE_PARTS {
	NJS_OBJECT** objects;
	NJS_TEXLIST* tex;
};

enum {
	COLOR_SWIM,
	COLOR_FLY,
	COLOR_RUN,
	COLOR_POWER,
	COLOR_RANDOM
};

struct CWE_MINIMAL {
	CWE_PARTS child;
	CWE_PARTS adult;
	NJS_OBJECT* animalObject;
	const char* texlistName;
	NJS_TEXLIST* animalTexlist;
	NJS_MOTION* motions[4];
	ChaoItemStats stats;
	int color;

	Uint32 Flags;
	char ID[16];
	const char* Name;

	Sint32 reserved[4];
};

struct CWE_REGAPI_LEGACY {
	Uint32 Version;

	//"msg" functions
	size_t(*RegisterFoName)(char* str); //fortune teller names, doesn't support every type of characters, try to stick to just letters
	size_t(*RegisterAlItemString)(const char* str); //blackmarket strings, id returned used in BlackMarketItemAttributes Name/Description
	void(*OverwriteAlItemString)(size_t id, const char* str);

	//black market functions
	void(*RegisterBlackMarketGeneralFruit)(int ID, int chance);
	void(*RegisterBlackMarketRareFruit)(int ID, int chance);
	BlackMarketItemAttributes* (*GetItemAttr)(ChaoItemCategory cat, int index); //let's you pull and edit existing item attributes
	void(*RegisterOtherItemFunc)(int ID, OtherItemPtr func); //things like "themes", we don't really support this properly yet though, we just did it to code the themes
	void(*SetRebuyFlag)(int Category, int ID, bool rebuy);

	size_t(*AddChaoHat)(NJS_OBJECT* model, NJS_TEXLIST* texlist, BlackMarketItemAttributes* attrib, const char* name, const char* description);

	//fruit functions
	int(*RegisterChaoFruit)(NJS_OBJECT* model, NJS_TEXLIST* texlist, ChaoItemStats* stats, BlackMarketItemAttributes* attrib, LastBiteFruitFuncPtr funcPtr, const char* name, const char* description);
	void(*SetFruitTexlist)(int ID, NJS_TEXLIST* texlist); //used to override the textures of a fruit, i honestly don't remember why we did this at all

	//accessory functions
	int(*RegisterChaoAccessory)(EAccessoryType type, NJS_OBJECT* model, NJS_TEXLIST* texlist, BlackMarketItemAttributes* attrib, const char* name, const char* description);
	void(*AccessoryMakeBald)(int accessory_id);
	void(*AccessoryDisableJiggle)(int accessory_id);

	//tree
	size_t(*AddChaoTree)(const CWE_API_TREE_DATA& tree_data, BlackMarketItemAttributes* attrib, const char* name, const char* description);

	//specials
	int(*RegisterChaoSpecial)(NJS_OBJECT* model, NJS_TEXLIST* texlist, BlackMarketItemAttributes* attrib, SpecialItemFuncPtr func, SpecialConditionFuncPtr cond, const char* name, const char* description, bool isAction);
	size_t(*RegisterEyeColor)(const char* textureName, NJS_TEXLIST* texlist, int specialID); //lens, to be used in conjunction with specials

	//custom chao
	size_t(*AddChaoType)(CWE_API_CHAO_DATA const* pData);

	//animals
	size_t(*AddChaoMinimal)(CWE_MINIMAL* pMinimal);
	void(*RegisterChaoMinimalFruit)(int fruitID, int minimalID, int chanceMin, int chanceMax);

	//animation
	int(*RegisterChaoAnimation)(std::string name, MOTION_TABLE* action);
	bool(*RegisterChaoAnimTransition) (const std::string& from, const std::string& to);
	size_t (*GetChaoAnimationIndex)(const std::string& name);
	MOTION_TABLE* (*GetChaoAnimation) (size_t index);

	//misc
	void(*RegisterChaoTexlistLoad)(const char* name, NJS_TEXLIST* load); //register texlists to load/unload in chao world
	void(*RegisterSaveLoad)(const char* suffix, void* buffer, int size); //register custom chao world savefiles like _CWE, karate mod uses this too
	void(*AddOdekakeMenu)(const CWE_API_ODEKAKE_ENTRY& entry); //add new button to transporter main menu, and handles logic to addnew menu

	size_t(*GetAccessoryID)(const char* pID);
};

struct CWE_API_REGISTER_TEXTURE {
	int Version;

	void (*AddChaoTexlistLoad) (const char* pTextureName, NJS_TEXLIST* pTexList);
	NJS_TEXLIST* (*AddAutoTextureLoad) (const char* pTextureName);
};

struct CWE_API_REGISTER_MOTION {
	int Version;

	size_t (*AddChaoMotion)(const char* pID, MOTION_TABLE* pMotionTable);
	size_t (*GetChaoMotionIndex) (const char* pID);
	MOTION_TABLE* (*GetChaoMotionTable) (size_t index);
	bool (*SetChaoMotionTransition) (const char* pFromID, const char* pToID);
};

struct CWE_API_REGISTER_MSG {
	int Version;

	size_t (*AddFortuneTellerName)(char* pStr);
	size_t (*AddAlItemString)(const char* pStr);
	void (*SetAlItemString)(size_t index, const char* pStr);
};

struct CWE_API_REGISTER_MARKET {
	int Version;

	BlackMarketItemAttributes* (*GetItemAttr)(ChaoItemCategory category, size_t index);
};

struct CWE_API_REGISTER {
	int Version;

	CWE_API_REGISTER_CHAO* pChao;
	CWE_API_REGISTER_ACCESSORY* pAccessory;
	CWE_API_REGISTER_TEXTURE* pTexture;
	CWE_API_REGISTER_MSG* pMsg;
};

struct CWE_API {
	int Version;

	// this is only included to let you port more complex API mods more easily
	// if there's any non-legacy equivalent for something you're trying to use, please use that
	CWE_REGAPI_LEGACY* pLegacy;

	CWE_API_REGISTER* pRegister;
};