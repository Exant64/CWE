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

#define GET_SOCIALWK(tp) ((socialwk*)tp->Data2.Undefined)

void Social_ErrorCheck(ObjectMaster* a1)
{
#ifdef SOCIALDEBUG
	if (!a1->Data2) PrintDebug("Social error: socialwk is 0");
#endif
}

// the "real" behavior that gets run on chao while the task manages the social behavior system
// the social task sets the mode here to 1 to finish the social action for the chao
static int ALBHV_Social(task* tp) {
	if (GET_CHAOWK(tp)->Behavior.Mode == 1) {
		return BHV_RET_FINISH;
	}

	return BHV_RET_CONTINUE;
}

static void Social_ClearBehavior(task* tp) {
	Social_ErrorCheck(tp);

	memset(GET_SOCIALWK(tp)->bhvFuncs, 0, sizeof(SBHV_FUNCDATA) * SBHVCOUNT);
}

static void Social_Clear(task* tp) {
	Social_ErrorCheck(tp);
	Social_ClearBehavior(tp);
}

void Social_SetParameter(task* tp, size_t actorIndex, int param) {
	GET_SOCIALWK(tp)->actorData[actorIndex].parameter1 = param;
}

void Social_SetActor(task* tp, size_t actorIndex, task* pChao) {
	Social_ErrorCheck(tp);

	AL_SetBehavior(pChao, ALBHV_Social);

	auto& actorData = GET_SOCIALWK(tp)->actorData[actorIndex];
	actorData.chaoPointer = pChao;
	actorData.parameter1 = 0;
}

void Social_QueueBehavior(task* tp, size_t actorIndex, SBHV_FUNC func) {
	socialwk* wk = GET_SOCIALWK(tp);
	Social_ErrorCheck(tp);

	if (wk->nbBhvFuncEntry < _countof(wk->bhvFuncs)) {
		wk->bhvFuncs[wk->nbBhvFuncEntry++] = { actorIndex, func, MAXSIZE_T };
	}
}

// syncs the last set behavior to the specified actor index
// (both run it at the same time)
void Social_SetSync(task* tp, size_t actorIndex) {
	socialwk* wk = GET_SOCIALWK(tp);
	auto& func = wk->bhvFuncs[wk->nbBhvFuncEntry - 1];

	Social_ErrorCheck(tp);

	// check so that the last behavior's actor isn't the same one we're syncing to it
	if (func.actorindex != actorIndex) {
		func.syncindex = actorIndex;
	}
}

void Social_SetIdle(task* tp, SBHV_FUNC pFunc) {
	GET_SOCIALWK(tp)->idleFunc = pFunc;
}

task* Social_GetActor(task* tp, size_t actorIndex) {
	return GET_SOCIALWK(tp)->actorData[actorIndex].chaoPointer;
}

static void Social_Main(task* tp) {
	socialwk* wk = GET_SOCIALWK(tp);
	SBHV_FUNCDATA& currFunc = wk->bhvFuncs[wk->CurrBhvFuncNum];
	bool abort = false;

	for (size_t i = 0; i < SOCIAL_CHAOCOUNT; i++) {
		task* pChao = wk->actorData[i].chaoPointer;
		// if the chao changed behavior we need to stop the social
		if (pChao && AL_GetBehavior(pChao) != ALBHV_Social) {
			abort = true;
			break;
		}
	}

	// if social doesnt have functions left in queue or aborting
	if (!currFunc.func || abort) {
		for (size_t i = 0; i < SOCIAL_CHAOCOUNT; i++){
			task* pChao = wk->actorData[i].chaoPointer;

			// mode 1 causes the ALBHV_Social BHV to end
			if (pChao && AL_GetBehavior(pChao) == ALBHV_Social) {
				GET_CHAOWK(pChao)->Behavior.Mode = 1;
			}
		}

		// kill
		tp->MainSub = DeleteObject_;
		return;
	}
	
	// idle function for when an actor is not the actor in the current behavior, useful for "listening"
	for (size_t i = 0; i < SOCIAL_CHAOCOUNT; i++) {
		SOCIAL_ACTOR& data = wk->actorData[i];

		//if the chao is an actor in the scene, or syncing with somebody in the scene, dont run idle
		if (i == currFunc.actorindex || i == currFunc.syncindex) continue;
		if (!data.chaoPointer) continue; //if there isnt even an actor in the slot

		wk->idleFunc(&data);
	}

	//current running actions actor
	SOCIAL_ACTOR& data = wk->actorData[currFunc.actorindex];

	data.actorIndex = currFunc.actorindex;

	//if function finishes
	if (currFunc.func(&data) == 1 ||
		(currFunc.syncindex != MAXSIZE_T && currFunc.func(&wk->actorData[currFunc.syncindex]) == 1))
	{
		// clear status data for everyone
		for (size_t i = 0; i < SOCIAL_CHAOCOUNT; i++) {
			memset(&wk->actorData[i].bhvStatus, 0, sizeof(SOCIAL_BHVDATA));
		}

		//go to next function
		wk->CurrBhvFuncNum++;
	}
}

task* Social_Create(SBHV_FUNC idleFunc) {
	task* social = LoadObject(5, "AL_Social", Social_Main, (LoadObj)0);

	socialwk* wk = ALLOC(socialwk);
	memset(wk, 0, sizeof(socialwk));

	social->Data2.Undefined = wk;

	for (size_t i = 0; i < SOCIAL_CHAOCOUNT; i++) {
		wk->actorData[i].socialContext = &wk->socialContext;
	}

	wk->idleFunc = idleFunc;

	return social;
}

