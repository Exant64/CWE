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
int ALBHV_LockUp(ObjectMaster* a1)
{
	if (ALW_RecieveCommand(a1) == ALW_CMD_CHANGE) {
		PrintDebug("ALW_CMD_CHANGE");
		return BHV_RET_BREAK;
	}
	if ((a1->Data1.Chao)->Behavior.Mode == 0)
	{
		AL_SetMotionLink(a1, 0);
		(a1->Data1.Chao)->Behavior.Mode++;
	}
	return 0;
}
signed int __cdecl ALBHV_GoToSocial(ObjectMaster* a1)
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
		ALW_TurnToLockOn(a1, 384);
		a2 = ChaoGlobal.WalkAcc * 0.8f;
		AL_ForwardAcc(a1, a2);
		v5 = ALW_CalcDistFromLockOn(a1);
		if (v5 >= 0)
		{
			if (v5 < 4)
			{
				AL_SetMotionLinkStep(a1, 100, 15);
				++v3->Mode;
			}
		}
		else
		{
			AL_SetBehavior(a1, ALBHV_Think);
		}
		break;
	case 2:
		if (ALW_TurnToLockOn(a1, 512) < 182)
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