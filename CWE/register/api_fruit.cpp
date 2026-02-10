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

const char* ShinyFruitDescription = "A Bright Fruit for Shiny Chao";
const char* HyperFruitDescription = "The rarest, but most powerful fruit";

const char* GBAFruitDescription = "Animals love this!";
const char* GBAFruitNames[] = {
	"Orange Fruit",
	"Blue Fruit",
	"Pink Fruit",
	"Green Fruit",
	"Purple Fruit",
	"Yellow Fruit",
	"Red Fruit",
};


void CWEFruit_HyperFruit(ChaoData* chaoData, ObjectMaster* fruit) {
	int hyperFruitID = fruit->Data1.Entity->Rotation.x - HyperSwimFruitID;
	if (chaoData->data.StatGrades[hyperFruitID] == 5 && chaoData->data.XGradeValue == 0)
	{
		chaoData->data.XGradeValue = hyperFruitID + 1;
	}
}

void CWEFruit_Shiny(ChaoData* chaoData, ObjectMaster* fruit) {
	if (chaoData->data.Shiny)
	{
		chaoData->data.Shiny = (chaoData->data.Shiny % 2) + 1;
	}
}

void ALAPI_RegisterFruit(CWE_REGAPI_LEGACY* cwe_api) {
	BlackMarketItemAttributes Spoiled = { 0,0,999,-1,-1,0 };
	cwe_api->RegisterChaoFruit(FruitModels[24], &AL_OBJECT_TEXLIST, &SpoiledFruitStats, &Spoiled, 0, "Spoiled Fruit", "");
	cwe_api->RegisterChaoFruit(FruitModels[25], &AL_OBJECT_TEXLIST, &SpoiledFruitStats, &Spoiled, 0, "Spoiled Fruit", "");
	cwe_api->RegisterChaoFruit(FruitModels[26], &AL_OBJECT_TEXLIST, &SpoiledFruitStats, &Spoiled, 0, "Spoiled Fruit", "");
	cwe_api->RegisterChaoFruit(FruitModels[27], &AL_OBJECT_TEXLIST, &StrongSpoiledFruitStats, &Spoiled, 0, "Spoiled Strong Fruit", "");
	cwe_api->RegisterChaoFruit(FruitModels[28], &AL_OBJECT_TEXLIST, &TastySpoiledFruitStats, &Spoiled, 0, "Spoiled Tasty Fruit", "");

	//New v8 fruit
	HyperSwimFruitID = cwe_api->RegisterChaoFruit(&object_HyperSwimFruit, &CWE_OBJECT_TEXLIST, &HyperSwimFruitStats, &HyperSwimFruitAttrib, CWEFruit_HyperFruit, "Hyper Swim Fruit", HyperFruitDescription);
	HyperFlyFruitID = cwe_api->RegisterChaoFruit(&object_HyperFlyFruit, &CWE_OBJECT_TEXLIST, &HyperFlyFruitStats, &HyperFlyFruitAttrib, CWEFruit_HyperFruit, "Hyper Fly Fruit", HyperFruitDescription);
	HyperRunFruitID = cwe_api->RegisterChaoFruit(&object_HyperRunFruit, &CWE_OBJECT_TEXLIST, &HyperRunFruitStats, &HyperRunFruitAttrib, CWEFruit_HyperFruit, "Hyper Run Fruit", HyperFruitDescription);
	HyperPowerFruitID = cwe_api->RegisterChaoFruit(&object_HyperPowerFruit, &CWE_OBJECT_TEXLIST, &HyperPowerFruitStats, &HyperPowerFruitAttrib, CWEFruit_HyperFruit, "Hyper Power Fruit", HyperFruitDescription);
	ShinyFruitID = cwe_api->RegisterChaoFruit(&object_ShinyFruit, &CWE_OBJECT_TEXLIST, &GenericStaminaStats, &ShinyFruitAttrib, CWEFruit_Shiny, "Shiny Fruit", ShinyFruitDescription);
	CakeSliceID = cwe_api->RegisterChaoFruit(&object_alo_cakeslice, &CWE_OBJECT_TEXLIST, &GenericStaminaStats, &CakeSliceAttrib, 0, "Cake Slice", "");
	OrangeID = cwe_api->RegisterChaoFruit(&obj_ali_orange, &CWE_OBJECT_TEXLIST, &GenericStaminaStats, &OrangeAttrib, 0, "Orange", "Tastes fresh, might attract bugs if you leave it out for too long...");


	//vanilla fruit names
	cwe_api->GetItemAttr(ChaoItemCategory_Fruit, 0)->Name = cwe_api->RegisterAlItemString("Chao Garden Fruit");
	cwe_api->GetItemAttr(ChaoItemCategory_Fruit, 1)->Name = cwe_api->RegisterAlItemString("Hero Garden Fruit");
	cwe_api->GetItemAttr(ChaoItemCategory_Fruit, 2)->Name = cwe_api->RegisterAlItemString("Dark Garden Fruit");
	cwe_api->GetItemAttr(ChaoItemCategory_Fruit, 3)->Name = cwe_api->RegisterAlItemString("Strong Fruit");
	cwe_api->GetItemAttr(ChaoItemCategory_Fruit, 4)->Name = cwe_api->RegisterAlItemString("Tasty Fruit");

	cwe_api->GetItemAttr(ChaoItemCategory_Hat, 1)->Name = cwe_api->RegisterAlItemString("Pumpkin");
	cwe_api->GetItemAttr(ChaoItemCategory_Hat, 2)->Name = cwe_api->RegisterAlItemString("Skull");

	//gba fruit
	int gbaDescription = cwe_api->RegisterAlItemString(GBAFruitDescription);
	for (int i = 13; i < 20; i++)
	{
		auto fruitAttr = cwe_api->GetItemAttr(ChaoItemCategory_Fruit, i);
		fruitAttr->Name = cwe_api->RegisterAlItemString(GBAFruitNames[i - 13]);
		fruitAttr->Descriptions = gbaDescription;
		fruitAttr->PurchasePrice = 350;
		fruitAttr->SalePrice = 75;

		cwe_api->RegisterBlackMarketGeneralFruit(i, 40);
	}
	cwe_api->GetItemAttr(ChaoItemCategory_Fruit, SA2BFruit_SmartFruit)->RequiredEmblems = 0;
	cwe_api->GetItemAttr(ChaoItemCategory_Fruit, SA2BFruit_MintCandy)->RequiredEmblems = 0;

	cwe_api->RegisterBlackMarketRareFruit(SA2BFruit_SmartFruit, 50);
	cwe_api->RegisterBlackMarketRareFruit(SA2BFruit_MintCandy, 60);
	cwe_api->RegisterBlackMarketRareFruit(ShinyFruitID, 60);
	cwe_api->RegisterBlackMarketRareFruit(HyperSwimFruitID, 10);
	cwe_api->RegisterBlackMarketRareFruit(HyperFlyFruitID, 10);
	cwe_api->RegisterBlackMarketRareFruit(HyperRunFruitID, 10);
	cwe_api->RegisterBlackMarketRareFruit(HyperPowerFruitID, 10);
	cwe_api->RegisterBlackMarketGeneralFruit(OrangeID, 40);

	//set all vanilla fruits except mushrooms and what comes after mushrooms rebuyable
	for (int i = 0; i < SA2BFruit_Mushroom; i++)
		cwe_api->SetRebuyFlag(ChaoItemCategory_Fruit, i, true);

	//set gba fruits to be rebuyable
	for (int i = SA2BFruit_OrangeFruit; i <= SA2BFruit_RedFruit; i++)
		cwe_api->SetRebuyFlag(ChaoItemCategory_Fruit, i, true);
}