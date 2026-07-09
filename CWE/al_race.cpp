#include "stdafx.h"

#include "al_race.h"
#include "Chao.h"
#include "data/toy/al_toy_wrench_prize.nja"

#include <map>
#include "al_chao_info.h"
#include "al_save.h"
#include "ChaoMain.h"
#include "ALifeSDK_Functions.h"
#include "al_sandhole.h"

int off_1361888[] =
{
	20286292,
	0,
	20304612,
	0,
	20271988,
	0,
	20316628,
	0,
	20278692,
	0,
	20296012,
	0,
	19885396,
	0,
	20291036,
	0,
	20273372,
	0,
	20318316,
	0,
	20316628,
	0,
	20302156,
	0,
	(int)&object_8D9005D83DB4F504F30,
	0,
};

DataPointer(task*, pWinnerChao, 0x019F6418);
FunctionPointer(RaceConfigThing*, sub_53A9B0, (), 0x53A9B0);
FunctionPointer(void, CreateToyPresenter, (int a1, int isLToy), 0x00539F30);
DataPointer(task*, pChaoObject, 0x01DCFAF0);

DataPointer(task*, dword_1A5AF18, 0x1A5AF18);

void AL_EggPresenterExecutor(task* tp) {
	taskwk* v1 = tp->twp;

	switch (v1->mode)
	{
	case 0:
		if (++v1->wtimer <= 0x23Au)
		{
			goto LABEL_12;
		}
		v1->ang.y += 512;
		v1->mode++;
		v1->smode = 0;
		break;
	case 1:
		v1->scl.x += 0.08f;
		if (v1->scl.x <= 1)
		{
			goto LABEL_12;
		}
		v1->scl.x = 1;
		v1->ang.y += 512;
		v1->mode++;
		v1->smode = 0;
		v1->wtimer = 0;
		break;
	case 2:
		if (v1->wtimer == 30)
		{
			PlayMusic("chao_r_item_get.adx");
			ResetMusic();
		}
		if (v1->wtimer++ <= 0x14Au)
		{
			goto LABEL_12;
		}
		v1->mode++;
		v1->ang.y += 512;
		v1->smode = 0;
		v1->wtimer = 0;
		break;
	case 3:
		v1->scl.x -= 0.08f;
		if (v1->scl.x >= 0.0)
		{
			goto LABEL_12;
		}
		v1->scl.x = 0;
		v1->ang.y += 512;
		v1->mode++;
		v1->smode = 0;
		break;
	default:
	LABEL_12:
		v1->ang.y += 512;
		break;
	}
}
void AL_EggPresenterDisplayer(task* tp) {
	if (tp->twp->mode && *(int*)dword_1A5AF18->awp < 10 && tp->twp->scl.x > 0.001f) {
		SetShaderType(1);
		DoLighting(0);
		Angle3 rot = { 0,tp->twp->ang.y,0 };
		DrawItem(
			320,
			400,
			tp->twp->scl.x * 1.2f, 
			rot,
		{
				ALW_CATEGORY_EGG, 
				(Uint16)tp->twp->ang.x
			}

		);
	}
}

//converts current jewel race to jewel texture to set
const std::map<Uint8, int> RaceToTexture = {
	{4, ChaoTexture_SkyBlueJewel}, //aquamarine
	{5, ChaoTexture_OrangeJewel}, //topaz
	{6, ChaoTexture_LimeGreenJewel}, //peridot
	{7, ChaoTexture_RedJewel}, //garnet
	{8, ChaoTexture_BlackJewel}, //onyx
	{9, ChaoTexture_Pearl}, //diamond
};

void WinJewelChaoInRace() {
	if (pWinnerChao != pChaoObject) return; //if your chao didn't win
	if (RaceMainType != 1) return; //not jewel
	if (RaceLevel != 4) return; //not level 5

	//not really supposed to happen though lol
	if (!RaceToTexture.contains(RaceSubType)) return; 
	int texture = RaceToTexture.at(RaceSubType);

	size_t chaoCount = 0;
	bool noTexture = true; //check if no chao in the garden have this texture already
	for (auto param : ChaoInfo::Instance()) {
		if (param->type != 0) {
			chaoCount++;

			if (param->body.JewelNum == texture) noTexture = false;
		}
	}

	if (chaoCount < ChaoInfo::Instance().Count() && noTexture) {
		//todo: make function in al_save or alg_kinder_bl that adds to purchased items including check
		if (cweSaveFile.purchasedItemCount < (int)save::CWE_PurchasedItems.size()) {
			save::CWE_PurchasedItems[cweSaveFile.purchasedItemCount] = 
			{ 
				ALW_CATEGORY_EGG, 
				(Uint16)(54 + texture - 1) 
			};

			cweSaveFile.purchasedItemCount++;

			//task* eggPresent = LoadObject(2, "AL_EggPresenter", AL_EggPresenterExecutor, IM_TWK);
			//eggPresent->DisplaySub = AL_EggPresenterDisplayer;
			//eggPresent->twp->Rotation.x = 54 + texture - 1;
		}
	}
}

void RaceNewPrize()
{
	RaceConfigThing* v5 = sub_53A9B0();
	ResetMusic();

	if (gConfigVal.WinJewelChaoInJewelRace) {
		WinJewelChaoInRace();
	}

	switch (RaceMainType)
	{
	case 0:
	case 1:
		if (pWinnerChao == pChaoObject)
		{
			if (v5->toyUnlock != -1 && !(GET_CHAOPARAM(pChaoObject)->knowledge.SToyFlag & (1 << v5->toyUnlock)))
			{
				CreateToyPresenter(v5->toyUnlock, 0);
				GET_CHAOPARAM(pChaoObject)->knowledge.SToyFlag |= 1 << v5->toyUnlock;
			}
		}
		break;
	case 2:
	case 3:
	case 4:
		if (RaceMainType == 2 && RaceLevel == 2) //force stoy unlock instead of ltoy challenge race, "i'm trying" course (for the wrench)
		{
			if (pWinnerChao == pChaoObject)
			{
				if (v5->toyUnlock != -1 && !(GET_CHAOPARAM(pChaoObject)->knowledge.SToyFlag & (1 << v5->toyUnlock)))
				{
					CreateToyPresenter(v5->toyUnlock, 0);
					GET_CHAOPARAM(pChaoObject)->knowledge.SToyFlag |= 1 << v5->toyUnlock;
				}
			}
		}
		else if (pWinnerChao == pChaoObject)
		{
			if (v5->toyUnlock != -1 && !(AL_GetCurrGardenInfo()->ToyGetFlag & (1 << v5->toyUnlock)))
			{
				CreateToyPresenter(v5->toyUnlock, 1);
				AL_GetCurrGardenInfo()->ToyGetFlag |= 1 << v5->toyUnlock;
			}
		}
		break;
	}
}

void al_race_Init() {
	//race toy unlocks
	HeroRaceConfigs[2].toyUnlock = AL_LTOY_UKIWA;
	//NO COFFIN FOR NOW
	//DarkRaceConfigs[2].toyUnlock = AL_LTOY_KANOKE;

	HeroRaceConfigs[0].toyUnlock = AL_LTOY_PIANO;
	DarkRaceConfigs[0].toyUnlock = AL_LTOY_ORGAN;

	//ChallengeRaceConfigs[1].toyUnlock = AL_STOY_CWE_UMBRELLA; 
	ChallengeRaceConfigs[2].toyUnlock = AL_STOY_CWE_WRENCH;

	//prize
	WriteData((int*)0x539EA1, (int)off_1361888);
	WriteCall((void*)0x0052C34D, nullsub_1);
	WriteCall((void*)0x0052C663, nullsub_1);
	WriteData<6>((unsigned char*)0x0052C683, (unsigned char)0x90);
	WriteData<4>((unsigned char*)0x0052C370, (unsigned char)0x90);
	WriteCall((void*)0x052C8C3, RaceNewPrize);
}