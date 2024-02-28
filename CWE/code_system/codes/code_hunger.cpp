#include "stdafx.h"
#include "include/code_hunger.h"
#include <Chao.h>
#include <random>

int __cdecl ALBHV_ChaoStarving_Punishment(ObjectMaster* a1)
{
	ChaoDataBase* base = a1->Data1.Chao->pParamGC;

	AL_EmotionAdd(a1, EM_MD_ANGER, (rand() % 100 + 100));
	AL_EmotionAdd(a1, EM_MD_SORROW, (rand() % 100 + 100));

	base->Happiness -= 5;
	if (base->Happiness < -100) base->Happiness = -100;

	AL_SetBehavior(a1, (BHV_FUNC)ChaoBehaviour_STARVING);
	return 0;
}

void ChaoHunger::Init() {
	WriteData((int*)(0x0059EBD6 - 4), (int)ALBHV_ChaoStarving_Punishment);

	//TODO: change this to edit the modapi array
	WriteData((unsigned short*)0x008A6454, (unsigned short)50); //+25% bonus for the fruit's garden
	WriteData((unsigned short*)0x008A6468, (unsigned short)50);
	WriteData((unsigned short*)0x008A647C, (unsigned short)50);
}