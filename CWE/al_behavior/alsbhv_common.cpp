#include "stdafx.h"
#include "..//SA2ModLoader.h"
#include "..//Chao.h"
#include "../al_social.h"
#include "../al_world.h"
#include "../ALifeSDK_Functions.h"
#include <ninja_functions.h>
#include <random>
#include "albhv.h"

int __cdecl AdjustAngle_(__int16 bams_a, unsigned __int16 bams_b, int dang)
{
	int result; // eax
	__int16 v4; // cx

	result = bams_b;
	v4 = bams_b - bams_a;
	if ((bams_b - bams_a) > dang || v4 < -dang)
	{
		if (v4 >= 0)
		{
			result = (dang + bams_a);
		}
		else
		{
			result = (bams_a - dang);
		}
	}
	return result;
}
int __cdecl DiffAngle(int a1, int a2)
{
	int result; // eax

	result = a2 - a1;
	if ((a2 - a1) < 0)
	{
		result = a1 - a2;
	}
	return result;
}
signed int __cdecl ALBHV_GoNextToSocial(ObjectMaster* a1)
{
	chaowk* v1; // esi
	int v2; // eax
	AL_BEHAVIOR* v3; // esi
	float a2; // ST08_4
	double v5; // st7
	
	if (ALW_RecieveCommand(a1) == ALW_CMD_CHANGE) {
		PrintDebug("ALW_CMD_CHANGE");
		return BHV_RET_BREAK;
	}

	v1 = (chaowk*)a1->Data1.Chao;
	v2 = v1->Behavior.Mode;
	v3 = &v1->Behavior;
	switch (v2)
	{
	case 0:
		AL_FaceChangeEye(a1, 4);
		v3->LimitTimer = 1800;
		if (++v3->Timer > 30)
		{
			AL_SetMotionLink(a1, 100);
			++v3->Mode;
		}
		break;
	case 1:
		MOV_TurnToAim2(a1, 384);
		a2 = ChaoGlobal.WalkAcc * 0.8f;
		AL_ForwardAcc(a1, a2);
		v5 = MOV_DistFromAim(a1);
		if (v5 >= 0.0)
		{
			if (v5 < 1.0)
			{
				AL_SetMotionLinkStep(a1, 100, 15);
				++v3->Mode;
				v3->Timer = 0;
			}
		}
		else
		{
			AL_SetBehavior(a1, ALBHV_Think);
		}
		break;
	case 2:

		v1->entity.Rotation.y = AdjustAngle_(v1->entity.Rotation.y, ALW_GetLockOnTask(a1)->Data1.Entity->Rotation.y, 1024);
		if (v3->Timer++ > 30)
			return 1;
		break;
	}
	AL_SetMotionSpd(a1, 1.5);

	v3->LimitTimer--;
	if (!v3->LimitTimer)
	{
		AL_SetBehavior(a1, ALBHV_Think);
	}
	return 0;
}
int ALBHV_WaitForSocialArrive(ObjectMaster* tp)
{
	chaowk* work = CHAOWK(tp);
	AL_BEHAVIOR* bhv = &work->Behavior; // esi
	float dist;

	if (ALW_RecieveCommand(tp) == ALW_CMD_CHANGE) 
		return BHV_RET_BREAK;
	
	switch (bhv->Mode) {
	case 0:
		ALW_SendCommand(tp, ALW_CMD_GO);
		if (njRandom() > 0.65f)
			AL_FaceChangeEye(tp, ChaoEyes_ClosedHappy);
		AL_SetMotionLink(tp, 414);
		++bhv->Mode;
		bhv->Timer = 60;
	case 1:
		ALW_TurnToLockOn(tp, 512);

		dist = ALW_CalcDistFromLockOn(tp);
		if (dist >= 0 && dist < 4) {
			return 1;
		}

		break;
	}
	
	return 0;
}
int ALBHV_LockUp(ObjectMaster* a1) {
	if ((a1->Data1.Chao)->Behavior.Mode == 0)
	{
		AL_SetMotionLink(a1, 0);
		(a1->Data1.Chao)->Behavior.Mode++;
	}
	return 0;
}
int ALBHV_GoToSocial(ObjectMaster* a1) {
	chaowk* v1; // esi
	int v2; // eax
	AL_BEHAVIOR* v3; // esi
	float a2; // ST08_4
	double v5; // st7

	if (ALW_RecieveCommand(a1) == ALW_CMD_CHANGE) {
		PrintDebug("ALW_CMD_CHANGE");
		return BHV_RET_BREAK;
	}

	v1 = (chaowk*)a1->Data1.Chao;
	v2 = v1->Behavior.Mode;
	v3 = &v1->Behavior;
	switch (v2)
	{
	case 0:
		AL_FaceChangeEye(a1, 4);
		v3->LimitTimer = 1800;
		if (++v3->Timer > 30)
		{
			AL_SetMotionLink(a1, 100);
			++v3->Mode;
		}
		break;
	case 1:
		ALW_TurnToLockOn(a1, 384);
		a2 = ChaoGlobal.WalkAcc * 0.8f;
		AL_ForwardAcc(a1, a2);
		break;
	}
	AL_SetMotionSpd(a1, 1.5);

	v5 = ALW_CalcDistFromLockOn(a1);
	if (v5 >= 0 && v5 < 4) {
		return BHV_RET_FINISH;
	}

	if (!--v3->LimitTimer) {
		return BHV_RET_BREAK;
	}

	return BHV_RET_CONTINUE;
}

int ALBHV_HandShake(task* tp) {
	chaowk* work = GET_CHAOWK(tp);
	AL_BEHAVIOR* bhv = &work->Behavior;

	switch (bhv->Mode) {
	case 0:
		AL_SetMotionLink(tp, 402);
		bhv->Timer = 60;
		bhv->Mode++;
		break;
	case 1:
		if (!--bhv->Timer) {
			return BHV_RET_FINISH;
		}
		break;
	}
	ALW_TurnToLockOn(tp, 384);

	return BHV_RET_CONTINUE;
}

int ALBHV_WaitForHandshake(task* tp) {
	chaowk* work = GET_CHAOWK(tp);
	AL_BEHAVIOR* bhv = &work->Behavior;

	float dist;

	switch (bhv->Mode) {
	case 0:
		AL_IconSet(tp, 2, 60);
		AL_SetMotionLink(tp, 100);
		bhv->Mode++;
		[[fallthrough]];
	case 1:
		int ang = ALW_TurnToLockOn(tp, 384);
		if (abs(ang) < 384) {
			AL_SetMotionLink(tp, 0);
			bhv->Mode++;
		}

		break;
	}

	dist = ALW_CalcDistFromLockOn(tp);
	if (dist >= 0 && dist < 4) {
		return BHV_RET_FINISH;
	}

	return BHV_RET_CONTINUE;
}

int ALBHV_WaitForSocialToArrive(task* tp) {
	if (ALW_RecieveCommand(tp) == ALW_CMD_CHANGE) {
		return BHV_RET_BREAK;
	}

	chaowk* work = GET_CHAOWK(tp);
	AL_BEHAVIOR* bhv = &work->Behavior;
	task* pChao = ALW_GetLockOnTask(tp);
	float randval;

	const float maxDist = 30.f;

	switch (bhv->Mode) {
	case 0:
		// PostureChangeSit
		switch (AL_GetMotionPosture(tp)) {
		case AL_PST_LIE:
			AL_SetMotionLinkStep(tp, ALM_OKIRU, 40);
			break;
		case AL_PST_STAND:
			AL_SetMotionLinkStep(tp, ALM_GENKI_SUWARI, 40);
			break;
		case AL_PST_FUSE:
		default:
			break;
		}
		bhv->Mode++;
		break;
	case 1:
		if (!AL_IsMotionEnd(tp)) break;
		// copied straight from the albhv_rest.c decomp, all we want is a "spoof" rest action
		randval = njRandom();
		if (randval < 0.2f) {
			AL_SetMotionLinkStep(tp, ALM_HIMA_SIT, 40);
		}
		else if (randval < 0.3f) {
			AL_SetMotionLinkStep(tp, ALM_HIMA_SIT_PATAPATA, 40);
		}
		else if (randval < 0.4f) {
			AL_SetMotionLinkStep(tp, ALM_HIMA_SIT_GUDE, 40);
		}
		else if (randval < 0.5f) {
			AL_SetMotionLinkStep(tp, ALM_HIMA_SIT_PATA, 40);
		}
		else if (randval < 0.6f) {
			AL_SetMotionLinkStep(tp, ALM_HIMA_SIT_PATAPATA, 40);
		}
		else if (randval < 0.7f) {
			AL_SetMotionLinkStep(tp, ALM_HIMA_SIT_SWING, 40);
		}
		else if (randval < 0.8f) {
			if (njRandom() < 0.5f) {
				AL_SetMotionLinkStep(tp, ALM_HIMA_SIT_LOOK_R, 40);
			}
			else {
				AL_SetMotionLinkStep(tp, ALM_HIMA_SIT_LOOK_L, 40);
			}
		}
		else {
			AL_SetMotionLinkStep(tp, ALM_HIMA_SIT_PORI, 40);
		}

		bhv->Mode++;
		break;
	case 2:
		if (CheckDistance(&work->entity.Position, &GET_CHAOWK(pChao)->entity.Position) < maxDist) {
			AL_FaceChangeEye(tp, ChaoEyes_ClosedUp);
			AL_FaceChangeMouth(tp, ChaoMouth_ClosedSmile);
			AL_IconSet(tp, 1, 90);

			bhv->Mode++;
		}

		break;
	case 3:
		// PostureChangeStand copypasta ik ik pretty nasty
		switch (AL_GetMotionPosture(tp)) {
		case AL_PST_SIT:
			AL_SetMotionLinkStep(tp, ALM_GENKI_TATI, 40);
			break;
		case AL_PST_LIE:
			AL_SetMotionLinkStep(tp, ALM_JUMP_UP, 40);
			break;
		case AL_PST_FUSE:
			AL_SetMotionLinkStep(tp, ALM_KOKEOKI, 35);
			break;
		case AL_PST_STAND:
		default:
			break;
		}
		bhv->Mode++;
		break;
	case 4:
		if (!AL_IsMotionEnd(tp)) break;

		AL_SetMotionLink(tp, 100);
		bhv->Mode++;
		[[fallthrough]];
	case 5:
		int ang = ALW_TurnToLockOn(tp, 384);
		if (abs(ang) < 384) {
			AL_SetMotionLink(tp, 0);
			bhv->Mode++;
		}

		break;
	}

	const float dist = ALW_CalcDistFromLockOn(tp);
	if (dist >= 0 && dist < 4) {
		return BHV_RET_FINISH;
	}

	return BHV_RET_CONTINUE;
}

static void GoNextToSocialNewSetAim(task* tp) {
	task* pChao = ALW_GetLockOnTask(tp);

	NJS_VECTOR finalPos = { 3,0,0 };
	NJS_POINT3 aim;

	njPushUnitMatrix();
	njTranslateEx(&pChao->Data1.Entity->Position);
	RotateY(pChao->Data1.Entity->Rotation.y);
	sub_426CC0(_nj_current_matrix_ptr_, &aim, &finalPos, 0);
	njPopMatrixEx();

	MOV_SetAimPos(tp, &aim);
}

int ALBHV_GoNextToSocialNew(task* tp) {
	if (ALW_RecieveCommand(tp) == ALW_CMD_CHANGE) {
		return BHV_RET_BREAK;
	}

	chaowk* work = GET_CHAOWK(tp);
	AL_BEHAVIOR* bhv = &work->Behavior;

	task* pChao = ALW_GetLockOnTask(tp);
	
	const int TIME_TO_LERP = 90;
	const int ANGLE_SPD = 384;

	const auto targetAng = pChao->Data1.Entity->Rotation.y;

	switch (bhv->Mode) {
	case 0:
		AL_SetMotionLink(tp, 100);
		bhv->LimitTimer = 60 * 30;
		bhv->Timer = 0;
		bhv->Mode++;

		[[fallthrough]];
	case 1:
		GoNextToSocialNewSetAim(tp);

		MOV_TurnToAim2(tp, ANGLE_SPD);

		{
			const float speed = GET_GLOBAL()->WalkAcc * 0.8f;
			tp->EntityData2->speed.x = njSin(work->entity.Rotation.y) * speed;
			tp->EntityData2->speed.z = njCos(work->entity.Rotation.y) * speed;
		}

		if (MOV_DistFromAimXZ(tp) < 0.5f) {
			bhv->Mode++;
		}
		break;
	case 2:
		work->entity.Rotation.y = AdjustAngle(work->entity.Rotation.y, targetAng, ANGLE_SPD);

		if (abs(work->entity.Rotation.y - targetAng) <= ANGLE_SPD) {
			return BHV_RET_FINISH;
		}
		break;
	}

	if (bhv->LimitTimer-- <= 0) {
		return BHV_RET_FINISH;
	}

	return BHV_RET_CONTINUE;
}