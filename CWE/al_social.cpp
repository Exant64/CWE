#pragma once
#include "stdafx.h"

#include <random>
#include "Chao.h"
#include "ALifeSDK_Functions.h"
#include "al_world.h"
#include "al_social.h"
#include "al_behavior/al_behavior.h"
#include "memory.h"

//#include "../al_soundrestoration.h"

//#define SOCIALDEBUG

void Social_ErrorCheck(task* a1)
{
#ifdef SOCIALDEBUG
	if (!a1->Data2) ___OutputDebugString("Social error: socialwk is 0");
#endif
}

//generic "lowlevel" social behavior
int ALBHV_Social(task* a1)
{
	//if finished
	if (GET_CHAOWK(a1)->Behavior.Mode == 1)//ALOField_Find(a1,1,0xB1))//)
	{
		//((chaowk*)a1->Data1)->pAnyTask = nullptr;
		return 1;
	}
	return 0;
}

void Social_ClearBehavior(task* a1)
{
	Social_ErrorCheck(a1);
	memset(((socialwk*)a1->awp)->bhvFuncs, 0, sizeof(SBHV_FUNCDATA) * SBHVCOUNT);
}

void Social_Clear(task* a1)
{
	Social_ErrorCheck(a1);
	Social_ClearBehavior(a1);
}

void Social_SetParameter(task* a1, int a2, int param)
{
	socialwk* wk = ((socialwk*)a1->awp);
	wk->data[a2].parameter1 = param;
}

void Social_SetActor(task* a1, int a2, task* chao)
{
	Social_ErrorCheck(a1);
	AL_SetBehaviorWithTimer(chao, (int)ALBHV_Social, -1);
	//((chaowk*)chao->Data1)->pAnyTask = a1;
	((socialwk*)a1->awp)->data[a2].chaoPointer = chao;
	((socialwk*)a1->awp)->data[a2].parameter1 = 0;
}

void Social_QueueBehavior(task* a1, int a2, SBHV_FUNC func)
{
	socialwk* wk = ((socialwk*)a1->awp);
	Social_ErrorCheck(a1);
	if (wk->nbBhvFuncEntry < 18)
	{
		wk->bhvFuncs[wk->nbBhvFuncEntry++] = { (unsigned char)a2, func, (char)-1 };
	}
}
void Social_SetSync(task* a1, int a2) //syncs behavior with another actor
{
	socialwk* wk = ((socialwk*)a1->awp);
	Social_ErrorCheck(a1);
	if (wk->bhvFuncs[wk->nbBhvFuncEntry - 1].actorindex != a2) //if index is not identical with other actor
		wk->bhvFuncs[wk->nbBhvFuncEntry - 1].syncindex = a2;  //set sync
}
void Social_Main(task* a1)
{
	socialwk* wk = ((socialwk*)a1->awp);
	SBHV_FUNCDATA& currFunc = wk->bhvFuncs[wk->CurrBhvFuncNum];
	bool abort = false;

	for (int i = 0; i < SOCIAL_CHAOCOUNT; i++)
	{
		if (wk->data[i].chaoPointer) //chao exists in actor slot
		{
			if (AL_GetBehavior(wk->data[i].chaoPointer) != ALBHV_Social) //if any chao changed behavior			
			{
				abort = true;
				break;
			}
		}
	}

	//if social doesnt have functions left in queue or aborting
	if (!currFunc.func || abort) {
		for (int i = 0; i < SOCIAL_CHAOCOUNT; i++){
			task* pChao = wk->data[i].chaoPointer;

			if (pChao && AL_GetBehavior(pChao) == ALBHV_Social) //chao exists in actor slot and still in social action
				GET_CHAOWK(pChao)->Behavior.Mode = 1;//exit
		}

		//kill
		a1->exec = DestroyTask;
		return;
	}
	
	//idle function for when an actor is not the actor in the current behavior, useful for "listening"
	for (int i = 0; i < SOCIAL_CHAOCOUNT; i++) {
		SOCIALDATA& data = wk->data[i];

		//if the chao is an actor in the scene, or syncing with somebody in the scene, dont run idle
		if (i == currFunc.actorindex || i == currFunc.syncindex) continue;
		if (!data.chaoPointer) continue; //if there isnt even an actor in the slot

		wk->idleFunc(&data);
	}

	//current running actions actor
	SOCIALDATA& data = wk->data[currFunc.actorindex];

	data.actorIndex = currFunc.actorindex;

	//if function finishes
	if (currFunc.func(&data) == 1 ||
		(currFunc.syncindex >= 0 && currFunc.func(&wk->data[currFunc.syncindex]) == 1))
	{
		//clear status data for everyone
		for (int i = 0; i < SOCIAL_CHAOCOUNT; i++)
			memset(&wk->data[i].bhvStatus, 0, sizeof(SOCIAL_BHVDATA));

		//go to next function
		wk->CurrBhvFuncNum++;
	}
}
void Social_SetIdle(task* a1, SBHV_FUNC a2)
{
	socialwk* wk = ((socialwk*)a1->awp);
	wk->idleFunc = a2;
}
task* Social_GetActor(task* a1, int a2)
{
	socialwk* wk = ((socialwk*)a1->awp);
	return wk->data[a2].chaoPointer;
}
task* Social_Create(SBHV_FUNC idleFunc)
{
	task* social = CreateElementalTask(0, LEV_5, Social_Main, "AL_Social");

	social->awp = syMalloc(sizeof(socialwk), "al_social.cpp", __LINE__);
	memset(social->awp, 0, sizeof(socialwk));

	socialwk* wk = (socialwk*)(social->awp);
	wk->idleFunc = idleFunc;
	return social;
}

