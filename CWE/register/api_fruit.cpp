#include "stdafx.h"
#include <AL_ModAPI.h>
#include <ChaoMain.h>
#include <data/cwe/object_common_cnk/ali_all.h>
#include <alo_fruit.h>

ChaoItemStats HyperSwimFruitStats = { 0,0,160,0,0,0,0,0 };
ChaoItemStats HyperFlyFruitStats = { 0,0,0,160,0,0,0,0 };
ChaoItemStats HyperRunFruitStats = { 0,0,0,0,160,0,0,0 };
ChaoItemStats HyperPowerFruitStats = { 0,0,0,0,0,160,0,0 };
ChaoItemStats GenericStaminaStats = { 0,0,0,0,0,0,40,0 }; //generic 4 bars of stamina type fruit
ChaoItemStats SpoiledFruitStats = { 0,0,0,0,0,0,-40,0,0,0 };
ChaoItemStats StrongSpoiledFruitStats = { 0,0,0,0,0,0,-30,0,0,0 };
ChaoItemStats TastySpoiledFruitStats = { 0,0,0,0,0,0,-50,0,0,0 };

BlackMarketItemAttributes CakeSliceAttrib = { 0, 0, 999, -1, -1, 0 };
BlackMarketItemAttributes OrangeAttrib = { 500, 100, 0, -1, -1, 0 };
BlackMarketItemAttributes ShinyFruitAttrib = { 3000, 1000, 0, -1, -1, 0 };
BlackMarketItemAttributes HyperSwimFruitAttrib = { 10000, 2500, 0, -1, -1, 0 };
BlackMarketItemAttributes HyperFlyFruitAttrib = { 10000, 2500, 0, -1, -1, 0 };
BlackMarketItemAttributes HyperRunFruitAttrib = { 10000, 2500, 0, -1, -1, 0 };
BlackMarketItemAttributes HyperPowerFruitAttrib = { 10000, 2500, 0, -1, -1, 0 };

static const char* ShinyFruitDescription = "A Bright Fruit for Shiny Chao";
static const char* HyperFruitDescription = "The rarest, but most powerful fruit";

static const char* GBAFruitDescription = "Animals love this!";

// "fruit" is intentionally not capitalized, like the other vanilla fruit
static const char* GBAFruitNames[] = {
	"Orange fruit",
	"Blue fruit",
	"Pink fruit",
	"Green fruit",
	"Purple fruit",
	"Yellow fruit",
	"Red fruit",
};


void CWEFruit_HyperFruit(task* tp, CHAO_SAVE_INFO* chaoData, task* fruit) {
	const int hyperFruitID = fruit->twp->ang.x - HyperSwimFruitID;

	if (chaoData->data.Abl[hyperFruitID] == 5 && GET_CWEPARAM(chaoData)->XGradeValue == 0) {
		GET_CWEPARAM(chaoData)->XGradeValue = hyperFruitID + 1;
	}
}

void CWEFruit_Shiny(task* tp, CHAO_SAVE_INFO* chaoData, task* fruit) {
	if (chaoData->data.body.MultiNum)
	{
		chaoData->data.body.MultiNum = (chaoData->data.body.MultiNum % 2) + 1;
	}
}

void ALAPI_RegisterFruit(CWE_REGAPI* cwe_api) {
	BlackMarketItemAttributes Spoiled = { 0,0,999,-1,-1,0 };
	cwe_api->RegisterChaoFruit(FruitModels[24], &AL_OBJECT_TEXLIST, &SpoiledFruitStats, &Spoiled, 0, "Spoiled Fruit", "");
	cwe_api->RegisterChaoFruit(FruitModels[25], &AL_OBJECT_TEXLIST, &SpoiledFruitStats, &Spoiled, 0, "Spoiled Fruit", "");
	cwe_api->RegisterChaoFruit(FruitModels[26], &AL_OBJECT_TEXLIST, &SpoiledFruitStats, &Spoiled, 0, "Spoiled Fruit", "");
	cwe_api->RegisterChaoFruit(FruitModels[27], &AL_OBJECT_TEXLIST, &StrongSpoiledFruitStats, &Spoiled, 0, "Spoiled Strong Fruit", "");
	cwe_api->RegisterChaoFruit(FruitModels[28], &AL_OBJECT_TEXLIST, &TastySpoiledFruitStats, &Spoiled, 0, "Spoiled Tasty Fruit", "");

	//New v8 fruit
	HyperSwimFruitID = cwe_api->RegisterChaoFruit(&object_HyperSwimFruit, texlist_cwe_object, &HyperSwimFruitStats, &HyperSwimFruitAttrib, CWEFruit_HyperFruit, "Hyper Swim Fruit", HyperFruitDescription);
	HyperFlyFruitID = cwe_api->RegisterChaoFruit(&object_HyperFlyFruit, texlist_cwe_object, &HyperFlyFruitStats, &HyperFlyFruitAttrib, CWEFruit_HyperFruit, "Hyper Fly Fruit", HyperFruitDescription);
	HyperRunFruitID = cwe_api->RegisterChaoFruit(&object_HyperRunFruit, texlist_cwe_object, &HyperRunFruitStats, &HyperRunFruitAttrib, CWEFruit_HyperFruit, "Hyper Run Fruit", HyperFruitDescription);
	HyperPowerFruitID = cwe_api->RegisterChaoFruit(&object_HyperPowerFruit, texlist_cwe_object, &HyperPowerFruitStats, &HyperPowerFruitAttrib, CWEFruit_HyperFruit, "Hyper Power Fruit", HyperFruitDescription);
	ShinyFruitID = cwe_api->RegisterChaoFruit(&object_ShinyFruit, texlist_cwe_object, &GenericStaminaStats, &ShinyFruitAttrib, CWEFruit_Shiny, "Shiny Fruit", ShinyFruitDescription);
	CakeSliceID = cwe_api->RegisterChaoFruit(&object_alo_cakeslice, texlist_cwe_object, &GenericStaminaStats, &CakeSliceAttrib, 0, "Cake Slice", "");
	OrangeID = cwe_api->RegisterChaoFruit(&obj_ali_orange, texlist_cwe_object, &GenericStaminaStats, &OrangeAttrib, 0, "Orange", "Tastes fresh, might attract bugs if you leave it out for too long...");


	//vanilla fruit names
	cwe_api->GetItemAttr(ALW_CATEGORY_FRUIT, 0)->Name = cwe_api->RegisterAlItemString("Chao Garden fruit");
	cwe_api->GetItemAttr(ALW_CATEGORY_FRUIT, 1)->Name = cwe_api->RegisterAlItemString("Hero Garden fruit");
	cwe_api->GetItemAttr(ALW_CATEGORY_FRUIT, 2)->Name = cwe_api->RegisterAlItemString("Dark Garden fruit");
	cwe_api->GetItemAttr(ALW_CATEGORY_FRUIT, 3)->Name = cwe_api->RegisterAlItemString("Strong fruit");
	cwe_api->GetItemAttr(ALW_CATEGORY_FRUIT, 4)->Name = cwe_api->RegisterAlItemString("Tasty fruit");

	cwe_api->GetItemAttr(ALW_CATEGORY_MASK, 1)->Name = cwe_api->RegisterAlItemString("Pumpkin");
	cwe_api->GetItemAttr(ALW_CATEGORY_MASK, 2)->Name = cwe_api->RegisterAlItemString("Skull");

	//gba fruit
	int gbaDescription = cwe_api->RegisterAlItemString(GBAFruitDescription);
	for (int i = 13; i < 20; i++)
	{
		auto fruitAttr = cwe_api->GetItemAttr(ALW_CATEGORY_FRUIT, i);
		fruitAttr->Name = cwe_api->RegisterAlItemString(GBAFruitNames[i - 13]);
		fruitAttr->Descriptions = gbaDescription;
		fruitAttr->PurchasePrice = 350;
		fruitAttr->SalePrice = 75;

		cwe_api->RegisterBlackMarketGeneralFruit(i, 40);
	}
	cwe_api->GetItemAttr(ALW_CATEGORY_FRUIT, ChaoFruit_SmartFruit)->RequiredEmblems = 0;
	cwe_api->GetItemAttr(ALW_CATEGORY_FRUIT, ChaoFruit_MintCandy)->RequiredEmblems = 0;

	cwe_api->RegisterBlackMarketRareFruit(ChaoFruit_SmartFruit, 50);
	cwe_api->RegisterBlackMarketRareFruit(ChaoFruit_MintCandy, 60);
	cwe_api->RegisterBlackMarketRareFruit(ShinyFruitID, 60);
	cwe_api->RegisterBlackMarketRareFruit(HyperSwimFruitID, 10);
	cwe_api->RegisterBlackMarketRareFruit(HyperFlyFruitID, 10);
	cwe_api->RegisterBlackMarketRareFruit(HyperRunFruitID, 10);
	cwe_api->RegisterBlackMarketRareFruit(HyperPowerFruitID, 10);
	cwe_api->RegisterBlackMarketGeneralFruit(OrangeID, 40);

	//set all vanilla fruits except mushrooms and what comes after mushrooms rebuyable
	for (int i = 0; i < ChaoFruit_Mushroom; i++)
		cwe_api->SetRebuyFlag(ALW_CATEGORY_FRUIT, i, true);

	//set gba fruits to be rebuyable
	for (int i = ChaoFruit_OrangeFruit; i <= ChaoFruit_RedFruit; i++)
		cwe_api->SetRebuyFlag(ALW_CATEGORY_FRUIT, i, true);
}