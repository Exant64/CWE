#include "stdafx.h"
#include <AL_ModAPI.h>
#include <ChaoMain.h>
#include <data/special/als_all.h>
#include <data/lens/LensModels.h>
#include <al_behavior/al_knowledge.h>
#include <al_omochao_build.h>
#include <al_behavior/albhv.h>

size_t SpartoiLensID = 0;

BlackMarketItemAttributes LensBoxDefaultAttr = { 500,100,0,  -1,-1,   0 };
BlackMarketItemAttributes LensBoxAligAttr = { 1000,250,0,  -1,-1,   0 };
BlackMarketItemAttributes LensBoxColorAttr = { 2000,500,0,  -1,-1,   0 };
BlackMarketItemAttributes LensBoxSpecialAttr = { 6000,1500,0,  -1, -1,   0 };
const char* LensDefaultDescription = "These lenses restore your Chao's eye color to Normal.";
const char* LensGenericDescription = "These lenses allow you to customize your Chao's eye color!";

extern "C" __declspec(dllexport) void ALS_LensSpecial(ObjectMaster * chao, ObjectMaster * item)
{
	if (item->Data1.Entity->Rotation.x == 0)
	{
		chao->Data1.Chao->pParamGC->EyeColor = 0;
		chao->Data1.Chao->pParamGC->EyeAlignment = 0;
	}
	else if (item->Data1.Entity->Rotation.x >= 1 && item->Data1.Entity->Rotation.x <= 3)
	{
		//	chao->Data1.Chao->pParamGC->EyeColor = 0;
		chao->Data1.Chao->pParamGC->EyeAlignment = item->Data1.Entity->Rotation.x;
	}
	else if (item->Data1.Entity->Rotation.x >= 4)
	{
		chao->Data1.Chao->pParamGC->EyeColor = ModAPI_LensColorMap[item->Data1.Entity->Rotation.x] + 1;
		//chao->Data1.Chao->pParamGC->EyeAlignment = 0;
	}
}
void ALS_DCWings(ObjectMaster* chao, ObjectMaster* item)
{
	chao->Data1.Chao->pParamGC->DCWings = !chao->Data1.Chao->pParamGC->DCWings;
}

void ALS_Brush(ObjectMaster* chao, ObjectMaster* item)
{
	chao->Data1.Chao->pParamGC->LobbyTextureValue = (item->Data1.Entity->Rotation.x - (MirrorID + 1)) + 1;
}
bool ALS_BrushCondition(ObjectMaster* chao, ObjectMaster* item)
{
	if (!chao->Data1.Chao->pParamGC) return false;
	return AL_IsHero(chao->Data1.Chao->pParamGC->Type);
}

const int AL_GrabObjectBothHandsPtr = 0x0056CFB0;
void AL_GrabObjectBothHands(ObjectMaster* a2, int a1)
{
	__asm
	{
		mov esi, a2
		mov ecx, a1
		call AL_GrabObjectBothHandsPtr
	}
}
void ALS_Omobuild(ObjectMaster* chao, ObjectMaster* item)
{
	AL_GrabObjectBothHands(chao, (int)item);
	AL_SetBehavior(chao, ALBHV_BuildStart);
}
bool ALS_OmobuildCondition(ObjectMaster* chao, ObjectMaster* item)
{
	ChaoDataBase* pParam = chao->Data1.Chao->pParamGC;
	OMOCHAO_INFO* omo = GetOmoData();
	return (omo->phase == 0 ||
		!AL_KW_IDExists(omo->chaoID) ||
		(pParam->ChaoID.id[0] == omo->chaoID.id[0] &&
			pParam->ChaoID.id[1] == omo->chaoID.id[1])) &&
		pParam->Knowledge.SToyFlag & (1 << 12) && omo->phase != 255;
}

bool ALS_ReincarnCondition(ObjectMaster* chao, ObjectMaster* item)
{
	if (!chao->Data1.Chao->pParamGC) return false;

	return (chao->Data1.Chao->pParamGC->Type > 2 && !chao->Data1.Chao->pParamGC->ForceReincarnate);
}

void ALS_Reincarn(ObjectMaster* chao, ObjectMaster* item)
{
	chao->Data1.Chao->pParamGC->Lifespan = 0;
	chao->Data1.Chao->pParamGC->Lifespan2 = 0;
	chao->Data1.Chao->pParamGC->StatGrades[6]++;
	chao->Data1.Chao->pParamGC->StatGrades[5]++;
	chao->Data1.Chao->pParamGC->ForceReincarnate = true;
}

void ALAPI_RegisterSpecial(CWE_REGAPI_LEGACY* cwe_api) {
	int LensDefault = cwe_api->RegisterChaoSpecial(&LensBoxDefault, &CWE_OBJECT_TEXLIST, &LensBoxDefaultAttr, ALS_LensSpecial, nullptr, "Default Lens", LensDefaultDescription, false);
	int lensGenericDescription = cwe_api->RegisterAlItemString(LensGenericDescription);
	LensBoxAligAttr.Descriptions = lensGenericDescription;
	LensBoxColorAttr.Descriptions = lensGenericDescription;
	LensBoxSpecialAttr.Descriptions = lensGenericDescription;
	cwe_api->RegisterChaoSpecial(&LensBoxNeut, &CWE_OBJECT_TEXLIST, &LensBoxAligAttr, ALS_LensSpecial, nullptr, "Neutral Lens", 0, false);
	cwe_api->RegisterChaoSpecial(&LensBoxHero, &CWE_OBJECT_TEXLIST, &LensBoxAligAttr, ALS_LensSpecial, nullptr, "Hero Lens", 0, false);
	cwe_api->RegisterChaoSpecial(&LensBoxDark, &CWE_OBJECT_TEXLIST, &LensBoxAligAttr, ALS_LensSpecial, nullptr, "Dark Lens", 0, false);

	int LensGreen = cwe_api->RegisterChaoSpecial(&LensBoxGreen, &CWE_OBJECT_TEXLIST, &LensBoxColorAttr, ALS_LensSpecial, nullptr, "Green Lens", 0, false);
	int LensMagenta = cwe_api->RegisterChaoSpecial(&LensBoxMagenta, &CWE_OBJECT_TEXLIST, &LensBoxColorAttr, ALS_LensSpecial, nullptr, "Magenta Lens", 0, false);
	int LensPurple = cwe_api->RegisterChaoSpecial(&LensBoxPurple, &CWE_OBJECT_TEXLIST, &LensBoxColorAttr, ALS_LensSpecial, nullptr, "Purple Lens", 0, false);
	int LensRed = cwe_api->RegisterChaoSpecial(&LensBoxRed, &CWE_OBJECT_TEXLIST, &LensBoxColorAttr, ALS_LensSpecial, nullptr, "Red Lens", 0, false);
	int LensYellow = cwe_api->RegisterChaoSpecial(&LensBoxYellow, &CWE_OBJECT_TEXLIST, &LensBoxColorAttr, ALS_LensSpecial, nullptr, "Yellow Lens", 0, false);
	int LensSpartoi = cwe_api->RegisterChaoSpecial(&LensBoxSpartoi, &CWE_OBJECT_TEXLIST, &LensBoxSpecialAttr, ALS_LensSpecial, nullptr, "Spartoi Lens", 0, false);
	int LensSnake = cwe_api->RegisterChaoSpecial(&LensBoxSnake, &CWE_OBJECT_TEXLIST, &LensBoxSpecialAttr, ALS_LensSpecial, nullptr, "Snake Lens", 0, false);
	int LensRobot = cwe_api->RegisterChaoSpecial(&LensBoxRobot, &CWE_OBJECT_TEXLIST, &LensBoxSpecialAttr, ALS_LensSpecial, nullptr, "Robot Lens", 0, false);
	cwe_api->RegisterEyeColor("cwe_eye_green", &CWE_LENS_GREEN_TEXLIST, LensGreen);
	cwe_api->RegisterEyeColor("cwe_eye_magenta", &CWE_LENS_MAGENTA_TEXLIST, LensMagenta);
	cwe_api->RegisterEyeColor("cwe_eye_purple", &CWE_LENS_PURPLE_TEXLIST, LensPurple);
	cwe_api->RegisterEyeColor("cwe_eye_red", &CWE_LENS_RED_TEXLIST, LensRed);
	cwe_api->RegisterEyeColor("cwe_eye_yellow", &CWE_LENS_YELLOW_TEXLIST, LensYellow);
	SpartoiLensID = cwe_api->RegisterEyeColor("cwe_eye_spartoi", &CWE_LENS_SPARTOI_TEXLIST, LensSpartoi);
	cwe_api->RegisterEyeColor("cwe_eye_snake", &CWE_LENS_SNAKE_TEXLIST, LensSnake);
	cwe_api->RegisterEyeColor("cwe_eye_robot", &CWE_LENS_ROBOT_TEXLIST, LensRobot);

	BlackMarketItemAttributes DCWings = { 500,150,0,  -1,-1,   0 };
	cwe_api->RegisterChaoSpecial(&object_als_dcwings, &CWE_OBJECT_TEXLIST, &DCWings, ALS_DCWings, nullptr, "Cosmetic Wings", "Matches a Chao's wings to it's appearance.", false);

	BlackMarketItemAttributes Negative = { 2000,400,0, -1, -1,   0 };
	MirrorID = cwe_api->RegisterChaoSpecial(&object_als_mirror,
		&CWE_OBJECT_TEXLIST,
		&Negative,
		[](ObjectMaster* chao, ObjectMaster* item) {
			chao->Data1.Chao->pParamGC->Negative = !chao->Data1.Chao->pParamGC->Negative;
		},
		nullptr,
			"Reverse Mirror",
			"A mirror staring into the other side of the dimension",
			false
			);

	BlackMarketItemAttributes Reincarnation = { 50000,5000,0,  -1,-1,   0 };
	BlackMarketItemAttributes Brush = { 1000,200,0,  -1,-1,   0 };
	cwe_api->RegisterChaoSpecial(&object_brush_hn, &CWE_OBJECT_TEXLIST, &Brush, ALS_Brush, ALS_BrushCondition, "Normal Brush", "A special brush for hero chao", false);
	cwe_api->RegisterChaoSpecial(&object_brush_hs, &CWE_OBJECT_TEXLIST, &Brush, ALS_Brush, ALS_BrushCondition, "Swim Brush", "A special brush for hero chao", false);
	cwe_api->RegisterChaoSpecial(&object_brush_hf, &CWE_OBJECT_TEXLIST, &Brush, ALS_Brush, ALS_BrushCondition, "Fly Brush", "A special brush for hero chao", false);
	cwe_api->RegisterChaoSpecial(&object_brush_hr, &CWE_OBJECT_TEXLIST, &Brush, ALS_Brush, ALS_BrushCondition, "Run Brush", "A special brush for hero chao", false);
	cwe_api->RegisterChaoSpecial(&object_brush_hp, &CWE_OBJECT_TEXLIST, &Brush, ALS_Brush, ALS_BrushCondition, "Power Brush", "A special brush for hero chao", false);
	cwe_api->RegisterChaoSpecial(&object_susbottle, &CWE_OBJECT_TEXLIST, &Reincarnation, ALS_Reincarn, ALS_ReincarnCondition, "Suspicious Bottle", "Warning: Affects your chao's lifespan", false);

	//eru lenses
	int LensJewelIDStart = cwe_api->RegisterChaoSpecial(&LensBoxJewel, &CWE_LENS_AQUA_TEXLIST, &LensBoxColorAttr, ALS_LensSpecial, nullptr, "Jewel Aqua Lens", 0, false);
	cwe_api->RegisterChaoSpecial(&LensBoxJewel, &CWE_LENS_JEWEL_BLUE_TEXLIST, &LensBoxColorAttr, ALS_LensSpecial, nullptr, "Jewel Blue Lens", 0, false);
	cwe_api->RegisterChaoSpecial(&LensBoxJewel, &CWE_LENS_JEWEL_YELLOW_TEXLIST, &LensBoxColorAttr, ALS_LensSpecial, nullptr, "Jewel Gold Lens", 0, false);
	cwe_api->RegisterChaoSpecial(&LensBoxJewel, &CWE_LENS_JEWEL_GREEN_TEXLIST, &LensBoxColorAttr, ALS_LensSpecial, nullptr, "Jewel Green Lens", 0, false);
	cwe_api->RegisterChaoSpecial(&LensBoxJewel, &CWE_LENS_JEWEL_PURPLE_TEXLIST, &LensBoxColorAttr, ALS_LensSpecial, nullptr, "Jewel Purple Lens", 0, false);
	cwe_api->RegisterChaoSpecial(&LensBoxJewel, &CWE_LENS_JEWEL_RED_TEXLIST, &LensBoxColorAttr, ALS_LensSpecial, nullptr, "Jewel Red Lens", 0, false);
	cwe_api->RegisterChaoSpecial(&LensBoxJewel, &CWE_LENS_JEWEL_SILVER_TEXLIST, &LensBoxColorAttr, ALS_LensSpecial, nullptr, "Jewel Silver Lens", 0, false);
	cwe_api->RegisterEyeColor("cwe_eye_aqua", &CWE_LENS_AQUA_TEXLIST, LensJewelIDStart);
	cwe_api->RegisterEyeColor("cwe_eye_jewel_blue", &CWE_LENS_JEWEL_BLUE_TEXLIST, LensJewelIDStart + 1);
	cwe_api->RegisterEyeColor("cwe_eye_jewel_yellow", &CWE_LENS_JEWEL_YELLOW_TEXLIST, LensJewelIDStart + 2);
	cwe_api->RegisterEyeColor("cwe_eye_jewel_green", &CWE_LENS_JEWEL_GREEN_TEXLIST, LensJewelIDStart + 3);
	cwe_api->RegisterEyeColor("cwe_eye_jewel_purple", &CWE_LENS_JEWEL_PURPLE_TEXLIST, LensJewelIDStart + 4);
	cwe_api->RegisterEyeColor("cwe_eye_jewel_red", &CWE_LENS_JEWEL_RED_TEXLIST, LensJewelIDStart + 5);
	cwe_api->RegisterEyeColor("cwe_eye_jewel_silver", &CWE_LENS_JEWEL_SILVER_TEXLIST, LensJewelIDStart + 6);

	BlackMarketItemAttributes Box = { 2500,500,0,  -1,-1,   0 };
	cwe_api->RegisterChaoSpecial(&object_als_boxparts, &CWE_OBJECT_TEXLIST, &Box, ALS_Omobuild, ALS_OmobuildCondition, "Box Of Parts", "Might be useful to chao with the right equipment...", true);


	//default lens rebuyable
	cwe_api->SetRebuyFlag(ChaoItemCategory_Special, LensDefault, true);
}