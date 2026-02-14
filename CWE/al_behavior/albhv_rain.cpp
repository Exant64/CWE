#include "stdafx.h"
#include "Chao.h"
#include "al_behavior.h"
#include "al_texlist.h"
#include "albhv.h"
#include <njdef.h>
#include <data/toy/al_toy_umbrella.nja>
#include <al_daynight.h>
#include <util.h>
#include "al_knowledge.h"

int ALBHV_Umbrella(task* tp) {
	chaowk* work = GET_CHAOWK(tp);
	AL_BEHAVIOR* bhv = &work->Behavior;

	switch (bhv->Mode) {
	case 0:
		AL_SetMotionLink(tp, 0);
		AL_FaceChangeEye(tp, ChaoEyes_ClosedHappy);
		AL_FaceChangeMouth(tp, ChaoMouth_ClosedSmile);
		AL_SetItem(tp, 4, al_item_parasol, &CWE_OBJECT_TEXLIST);
		bhv->Mode++;
		bhv->Timer = 150 + njRandom() * 300;
		break;	
	case 1:
		if (!AL_DayNightCycle_IsRain()) {
			bhv->Mode++;
		}
		break;
	case 2:
		if (!--bhv->Timer) {
			return BHV_RET_FINISH;
		}
		break;
	}
	
	return BHV_RET_CONTINUE;
}

static int ALBHV_WaitForRainToEnd(task* tp) {
	chaowk* work = GET_CHAOWK(tp);
	AL_BEHAVIOR* bhv = &work->Behavior;

	switch (bhv->Mode) {
	case 0:
		AL_SetMotionLink(tp, 0);
		AL_FaceChangeEye(tp, ChaoEyes_ClosedHappy);
		AL_FaceChangeMouth(tp, ChaoMouth_ClosedSmile);
		bhv->Mode++;
		bhv->Timer = 150 + njRandom() * 300;
		break;
	case 1:
		if (!AL_DayNightCycle_IsRain()) {
			bhv->Mode++;
		}
		break;
	case 2:
		if (!--bhv->Timer) {
			return BHV_RET_FINISH;
		}
		break;
	}

	return BHV_RET_CONTINUE;
}

void AL_CalcIntentionScore_Rain(task* tp, float* pMaxScore) {
	if (!AL_DayNightCycle_IsRain()) return;

	const auto emotion = 3000 + njRandom() * 7000.f;
	const float calmValue = (AL_EmotionGetValue(tp, EM_PER_CALM) + 100) / 200.f;
	float score = 0.99f * AL_CalcScoreTypeA(emotion, lerp(2500, 6000, calmValue));
	//AL_ScoreRandomize(&score);

	if (true || score > *pMaxScore) {
		if (AL_KW_IsSToyFlagOn(tp, AL_STOY_CWE_UMBRELLA) && njRandom() < 0.75f) {
			AL_SetBehavior(tp, ALBHV_Umbrella);
			*pMaxScore = score;
			return;
		}
		
		// if they don't have an umbrella, chill under a tree a bit
		//check if tree found and other chao isn't shaking it + check if tree is adult
		al_perception_link* pLink = AL_GetFoundTree(tp);
		if (pLink && !ALW_IsSheAttentionOtherOne(tp, pLink->pEntry->tp) && pLink->pEntry->tp->Data1.Entity->Action == 7)
		{
			//ALW_LockOn(tp, pLink->pEntry->tp);
			MOV_SetAimPos(tp, &pLink->pEntry->tp->Data1.Entity->Position);

			AL_SetBehavior(tp, ALBHV_GoToAimTree);
			AL_SetNextBehavior(tp, ALBHV_WaitForRainToEnd);

			*pMaxScore = score;
		}
		
	}
}

Bool AL_CheckRain(task* tp) {
	chaowk* work = GET_CHAOWK(tp);
	auto v1 = work->Behavior.BhvFuncList[work->Behavior.CurrBhvFuncNum];
	if ((int)v1 == 0x5607C0) {
		return FALSE;
	}
	if ((int)v1 == 0x54EF10) {
		return FALSE;
	}

	if (v1 == ALBHV_PostureChangeStand) return FALSE;
	if (v1 == ALBHV_Umbrella) return FALSE;

	if (!AL_DayNightCycle_IsRain()) return FALSE;

	AL_SetBehavior(tp, ALBHV_PostureChangeStand);
	AL_SetNextBehavior(tp, ALBHV_Umbrella);

	return TRUE;
}