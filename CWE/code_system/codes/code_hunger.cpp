#include "stdafx.h"
#include "include/code_hunger.h"
#include <Chao.h>
#include <random>

int __cdecl ALBHV_ChaoStarving_Punishment(task* a1)
{
	auto pParam = GET_CHAOPARAM(a1);

	AL_EmotionAdd(a1, EM_MD_SORROW, (rand() % 100 + 100));

	pParam->Happiness -= 5;
	if (pParam->Happiness < -100) pParam->Happiness = -100;

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