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

void Social_ErrorCheck(ObjectMaster* a1)
{
#ifdef SOCIALDEBUG
	if (!a1->Data2) PrintDebug("Social error: socialwk is 0");
#endif
}

//generic "lowlevel" social behavior
int ALBHV_Social(ObjectMaster* a1)
{
	//if finished
	if (a1->Data1.Chao->Behavior.Mode == 1)//ALOField_Find(a1,1,0xB1))//)
	{
		//((chaowk*)a1->Data1)->pAnyTask = nullptr;
		return 1;
	}
	return 0;
}

void Social_ClearBehavior(ObjectMaster* a1)
{
	Social_ErrorCheck(a1);
	memset(((socialwk*)a1->Data2.Undefined)->bhvFuncs, 0, sizeof(SBHV_FUNCDATA) * SBHVCOUNT);
}

void Social_Clear(ObjectMaster* a1)
{
	Social_ErrorCheck(a1);
	Social_ClearBehavior(a1);
}

void Social_SetParameter(ObjectMaster* a1, int a2, int param)
{
	socialwk* wk = ((socialwk*)a1->Data2.Undefined);
	wk->data[a2].parameter1 = param;
}

void Social_SetActor(ObjectMaster* a1, int a2, ObjectMaster* chao)
{
	Social_ErrorCheck(a1);
	AL_SetBehaviorWithTimer(chao, (int)ALBHV_Social, -1);
	//((chaowk*)chao->Data1)->pAnyTask = a1;
	((socialwk*)a1->Data2.Undefined)->data[a2].chaoPointer = chao;
	((socialwk*)a1->Data2.Undefined)->data[a2].parameter1 = 0;
}

void Social_QueueBehavior(ObjectMaster* a1, int a2, SBHV_FUNC func)
{
	socialwk* wk = ((socialwk*)a1->Data2.Undefined);
	Social_ErrorCheck(a1);
	if (wk->nbBhvFuncEntry < 18)
	{
		wk->bhvFuncs[wk->nbBhvFuncEntry++] = { (unsigned char)a2, func, (char)-1 };
	}
}
void Social_SetSync(ObjectMaster* a1, int a2) //syncs behavior with another actor
{
	socialwk* wk = ((socialwk*)a1->Data2.Undefined);
	Social_ErrorCheck(a1);
	if (wk->bhvFuncs[wk->nbBhvFuncEntry - 1].actorindex != a2) //if index is not identical with other actor
		wk->bhvFuncs[wk->nbBhvFuncEntry - 1].syncindex = a2;  //set sync
}
void Social_Main(ObjectMaster* a1)
{
	socialwk* wk = ((socialwk*)a1->Data2.Undefined);
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
			ObjectMaster* pChao = wk->data[i].chaoPointer;

			if (pChao && AL_GetBehavior(pChao) == ALBHV_Social) //chao exists in actor slot and still in social action
				pChao->Data1.Chao->Behavior.Mode = 1;//exit
		}

		//kill
		a1->MainSub = DeleteObject_;
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
void Social_SetIdle(ObjectMaster* a1, SBHV_FUNC a2)
{
	socialwk* wk = ((socialwk*)a1->Data2.Undefined);
	wk->idleFunc = a2;
}
ObjectMaster* Social_GetActor(ObjectMaster* a1, int a2)
{
	socialwk* wk = ((socialwk*)a1->Data2.Undefined);
	return wk->data[a2].chaoPointer;
}
ObjectMaster* Social_Create(SBHV_FUNC idleFunc)
{
	ObjectMaster* social = LoadObject(5, "AL_Social", Social_Main, (LoadObj)0);

	social->Data2.Undefined = syMalloc(sizeof(socialwk), "al_social.cpp", __LINE__);
	memset(social->Data2.Undefined, 0, sizeof(socialwk));

	socialwk* wk = (socialwk*)(social->Data2.Undefined);
	wk->idleFunc = idleFunc;
	return social;
}

