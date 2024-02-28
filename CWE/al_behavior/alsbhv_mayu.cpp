#include "stdafx.h"
#include "..//SA2ModLoader.h"
#include "..//Chao.h"
#include "../al_social.h"
#include "../al_world.h"
#include "../ALifeSDK_Functions.h"
#include "../ninja_functions.h"
#include "alsbhv.h"
#include <random>
#include "al_knowledge.h"
#include "..//AL_ModAPI.h"
#include <al_behavior/al_intention.h>

int ALBHV_MayuPleasure(task* tp) {
	AL_BEHAVIOR* bhv = &GET_CHAOWK(tp)->Behavior;
	float randval;

	switch (bhv->Mode) {
	case 0u:		
		if (njRandom() < 0.5f) {
			AL_SetMotionLink(tp, 468);
		}
		else {
			AL_SetMotionLink(tp, 463);
		}
		
		if (njRandom() < 0.5f) {
			AL_FaceChangeEye(tp, 1);
		}
		else {
			AL_FaceChangeEye(tp, 4);
		}

		randval = njRandom();
		if (randval < 0.4f) {
			AL_FaceChangeMouth(tp, ChaoMouth_ClosedSmile);
		}
		else if (randval < 0.8f) {
			AL_FaceChangeMouth(tp, ChaoMouth_OpenSmile);
		}
		else {
			AL_FaceChangeMouth(tp, ChaoMouth_Open);
		}
		
		bhv->Timer = (200 + (int)(njRandom() * 100.f));
		bhv->Mode++;

		break;
	case 1u:
		if (--bhv->Timer <= 0) {
			bhv->Mode = 2;
		}
		break;
	case 2u:
		if (njRandom() < 0.5f) {
			return BHV_RET_FINISH;
		}

		bhv->Mode = 0;
		break;
	}

	return BHV_RET_CONTINUE;
}

int ALBHV_GoToMayu(task* tp) {
	AL_BEHAVIOR* bhv = &GET_CHAOWK(tp)->Behavior;
	float dist;

	switch (bhv->Mode) {
	case 0:
		AL_FaceChangeEye(tp, 4);

		bhv->LimitTimer = 1800;
		if (++bhv->Timer > 30)
		{
			AL_SetMotionLink(tp, 100);
			++bhv->Mode;
		}
		break;
	case 1:
		MOV_TurnToAim2(tp, 384);
		AL_ForwardAcc(tp, GET_GLOBAL()->WalkAcc * 0.8f);

		dist = MOV_DistFromAim(tp);
		if (dist >= 0) {
			if (dist < 6) {
				AL_SetMotionLinkStep(tp, 100, 15);
				++bhv->Mode;
			}
		}
		else {
			AL_SetBehavior(tp, ALBHV_Think);
		}

		break;
	case 2:
		if (MOV_TurnToAim2(tp, 512) < 182)
			return BHV_RET_FINISH;

		break;
	}

	AL_SetMotionSpd(tp, 1.5f);

	if (!--bhv->LimitTimer) {
		AL_SetBehavior(tp, ALBHV_Think);
	}

	return BHV_RET_CONTINUE;
}

int ALBHV_SetMayuIntentionTimer(task* tp) {
	AL_SetIntervalTimer(tp, INT_TIMER_COCOONREACT, 1800);
	return BHV_RET_FINISH;
}

int ALBHV_MayuDance(task *tp) {
	AL_BEHAVIOR* bhv = &GET_CHAOWK(tp)->Behavior;

	switch (bhv->Mode) {
	case 0:
		Chao_RegAnimation(tp, "alm_dance_n");
		AL_FaceChangeEye(tp, ChaoEyes_Painful);
		AL_FaceChangeMouth(tp, ChaoMouth_ClosedSmile);
		bhv->Timer = (300 + (int)(njRandom() * 300));
		bhv->Mode++;
		break;
	case 1:
		if (--bhv->Timer <= 0)
			return BHV_RET_FINISH;

		break;
	}

	return BHV_RET_CONTINUE;
}

void AL_CalcIntentionScore_Mayu(ObjectMaster* tp, float* pMaxScore) {
	task* pField;

	if (GET_CHAOWK(tp)->Behavior.IntervalTimer[0x10]) return;

	if (pField = ALO_Field_Find_(tp, 1, CI_KIND_AL_MAYU_GROW_FIELD)) {
		MOV_SetAimPos(tp, &pField->Parent->Data1.Entity->Position);
		AL_KW_MeetEachother(tp, pField->Parent, KW_MEET_EVOLVE);

		AL_SetBehavior(tp, ALBHV_GoToMayu);
		AL_SetNextBehavior(tp, ALBHV_MayuPleasure);
		AL_SetNextBehavior(tp, ALBHV_SetMayuIntentionTimer);

		*pMaxScore = 1.0f;
	}
	else if (pField = ALO_Field_Find_(tp, 1, CI_KIND_AL_MAYU_DEATH_FIELD)) {
		MOV_SetAimPos(tp, &pField->Parent->Data1.Entity->Position);
		AL_EmotionAdd(tp, EM_MD_SORROW, 50);

		AL_SetBehavior(tp, ALBHV_GoToMayu);
		AL_SetNextBehavior(tp, ALBHV_Cry);
		AL_SetNextBehavior(tp, ALBHV_SetMayuIntentionTimer);

		*pMaxScore = 1.0f;
	}
	else if (pField = ALO_Field_Find_(tp, 1, CI_KIND_AL_MAYU_SUCCEED_FIELD)) {
		MOV_SetAimPos(tp, &pField->Parent->Data1.Entity->Position); //set waypoint to chao position
		AL_KW_MeetEachother(tp, pField->Parent, KW_MEET_REBIRTH); //meet chao

		AL_SetBehavior(tp, ALBHV_GoToMayu);
		AL_SetNextBehavior(tp, ALBHV_MayuDance);
		AL_SetNextBehavior(tp, ALBHV_SetMayuIntentionTimer);

		*pMaxScore = 1.0f;
	}
}