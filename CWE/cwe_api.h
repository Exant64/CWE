#pragma once
#include "SA2ModLoader.h"
#include <string>

//ignore this, this is for our side of things
#ifdef CWEV9
	#include "alg_kinder_bl.h"
	#include "alg_kinder_ortho.h"
#else
	struct ChaoHudThingB
	{
		int adjust;
		float wd;
		float ht;
		float s0;
		float t0;
		float s1;
		float t1;
		NJS_TEXLIST* pTexlist;
		int TexNum;
	};

	struct BlackMarketItemAttributes
	{
		int PurchasePrice;
		int SalePrice;
		__int16 RequiredEmblems;
		__int16 Name;
		__int16 Descriptions;
		__int16 Unknown;
	};

	struct __declspec(align(4)) MotionTableAction
	{
		NJS_MOTION* NJS_MOTION;
		__int16 FlagThing1;
		__int16 field_6;
		int TransitionToID;
		int field_C;
		float StartFrame;
		float EndFrame;
		float PlaySpeed;
	};
#endif

#define ICON_TYPE_BALL 0
#define ICON_TYPE_HALO 1
#define ICON_TYPE_SPIKY 2
#define ICON_TYPE_CUSTOM 3 //don't use this for now, since pIconData isnt finished

#define CUSTOM_CHAO_FLAG_SECOND_EVO BIT_0 //only for advanced users for now since we don't have safety checks in place for it yet

struct CWE_API_CHAO_DATA {
	using CanEvolve = bool(*)(ObjectMaster* tp);

	NJS_OBJECT* pObject; //chunk model format (sa2mdl)
	NJS_OBJECT* pSecondEvoList[5];

	const char* TextureName; //we don't support texlists because we need to "inject" the loaded textures into al_body
	int TextureCount;
	Uint32 IconColor; //emotion ball color
	Uint32 IconType; //0 - ball, 1 - halo, 2 - spiky, 3 - custom emote ball
	void* pIconData; //placeholder for custom emotion balls in the future

	CanEvolve pEvolveFunc; //if function returns true during evolution, chao will evolve into this type

	Uint32 Flags;
	const char* Name; //we use it for the health center for now, also useful for KCE, ex. "Spartoi"
	char ID[16]; //type id, HAS TO BE NULL TERMINATED, ex. "cwe_spartoi"

	Sint32 reserved[4];
};

using OtherItemPtr = void(*)(int ID);
using LastBiteFruitFuncPtr = void(*)(ChaoData* chaoData, ObjectMaster* fruit);
using SpecialItemFuncPtr = void(*)(ObjectMaster* chao, ObjectMaster* item);
using SpecialConditionFuncPtr = bool(*)(ObjectMaster* chao, ObjectMaster* item);

enum EAccessoryType : unsigned char {
	Head = 0,
	Face,
	Generic1,
	Generic2,
	AccessoryTypeCount
};

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

#define ODE_FLAGS_NONE (0)
#define ODE_FLAGS_REQUIRE_CHAO BIT_0 // holding chao required to enter the menu (button greyed out if otherwise)
#define ODE_FLAGS_REQUIRE_NO_CHAO BIT_1 // not allowed to enter the menu if you're holding a chao (button greyed out if otherwise)

struct ODE_MENU_MASTER_WORK;

struct CWE_API_ODEKAKE_ENTRY {
	using OdekakeFuncPtr = void(*)(ODE_MENU_MASTER_WORK*);
	using OdekakeConditionPtr = bool(*)();

	// the function ran every frame for the menu logic
	OdekakeFuncPtr pMenuFunc;

	// placeholder for future (intended to be used for "unlocking" menus, ignore for now)
	OdekakeConditionPtr pConditionFunc;

	// flags are above the struct ("ODE_FLAGS")
	Uint32 Flags;

	// the icon sprite on the left of the button
	ChaoHudThingB* ButtonIcon;

	// the text sprite in the middle of the button if the menu is accessible
	ChaoHudThingB* ButtonText;

	// the text sprite in the middle of the button if the menu is inaccessible
	ChaoHudThingB* GreyButtonText;

	// the icon sprite for the top "LargeTitleBar" in the menu
	ChaoHudThingB* BarIcon;

	// the text sprite for the top "LargeTitleBar" in the menu
	ChaoHudThingB* BarText;

	// in 0-1 float format
	float BarColorA;
	float BarColorR;
	float BarColorG;
	float BarColorB;
};

struct CWE_REGAPI {
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
	int(*RegisterChaoAnimation)(std::string name, MotionTableAction* action);
	int (*RegisterChaoAnimTransition) (const std::string& from, const std::string& to);
	int(*GetChaoAnimationIndex)(const std::string& name);
	MotionTableAction* (*GetChaoAnimation) (size_t index);

	//misc
	void(*RegisterChaoTexlistLoad)(const char* name, NJS_TEXLIST* load); //register texlists to load/unload in chao world
	void(*RegisterSaveLoad)(const char* suffix, void* buffer, int size); //register custom chao world savefiles like _CWE, karate mod uses this too
	void(*AddOdekakeMenu)(const CWE_API_ODEKAKE_ENTRY& entry); //add new button to transporter main menu, and handles logic to addnew menu
};