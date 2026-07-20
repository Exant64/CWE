#include "stdafx.h"
#include "..//SA2ModLoader.h"
#include "../Chao.h"
#include "../al_world.h"
#include "..//ninja_functions.h"
#include "../ALifeSDK_Functions.h"
#include <random>
#include "albhv.h"
#include "../AL_ModAPI.h"
#include <ChaoMain.h>
#include "albhv_navigation.h"

signed int __cdecl ALBHV_GoToWaterWithBoat(task* a1)
{
	chaowk* v1; // esi
	int v2; // eax
	AL_BEHAVIOR* v3; // esi
	float a2; // ST08_4
	double v5; // st7
	int v7; // eax

	v1 = GET_CHAOWK(a1);
	v2 = v1->Behavior.Mode;
	v3 = &v1->Behavior;
	if (v2)
	{
		if (v2 == 1)
		{
			MOV_TurnToAim2(a1, 384);
			a2 = ChaoGlobal.WalkAcc * 0.8f;
			AL_ForwardAcc(a1, a2);
			v5 = MOV_DistFromAim(a1);
			if (v5 >= 0.0)
			{
				if (v5 < 4.0)
				{
					//return 1;
				}
			}
			else
			{
				AL_SetBehavior(a1, ALBHV_Think);
			}
		}
	}
	else
	{
		//AL_FaceChangeEye(a1, 4);
		v7 = v3->Timer;
		v3->Timer = v7 + 1;
		AL_FixPosition(a1);

		if (v7 > 30)
		{
			AL_SetMotionLink(a1, 194 - 3);
			++v3->Mode;
		}
	}
	AL_SetMotionSpd(a1, 1.5);

	GET_CHAOWK(a1)->Behavior.Flag |= 1;		 //block water detection?
	MOVE_WORK* mov = (MOVE_WORK*)a1->mwp;
	if (a1->twp->pos.y + 2.0 < mov->WaterY)
		return BHV_RET_FINISH;
	return AL_MoveHoldingObject(a1) == 0;
}
signed int ALBHV_RideBoat(task* a1)
{
	chaowk* v1; // esi
	AL_BEHAVIOR* v3; // esi

	v1 = GET_CHAOWK(a1);
	v3 = &v1->Behavior;
	MOVE_WORK* move = (MOVE_WORK*)a1->mwp;
	
	//SWIM CONTROL
	ALW_CommunicationOn(a1, ALW_GetLockOnTask(a1));
	if (!(v1->Behavior.Flag & 4))
	{
		if (move->Flag & 0x400)
		{
			//AL_ForwardAcc(a1, 0.1f);
			AL_ForwardSpd(a1, 1);
			AL_SetBehavior(a1, (BHV_FUNC)0x00562D20);
		}
	}
	if (AL_EmotionGetValue(a1, EM_ST_THIRSTY) < 1000 && move->Flag & 0x4000)
	{
		CCL_HIT_INFO* v6 = CCL_IsHitKindEx(a1, 0xCB); //climb
		if (v6 && v6->hit_tp)
		{
			AL_SetBehavior(a1, (BHV_FUNC)0x562EB0);
		}
	}

	switch (v3->Mode)
	{
	case 0:
		//AL_FixPosition(a1);
		//v1->ChaoFlag &= ~2u;
		Chao_RegAnimation(a1, "alm_duck");
		//AL_SetMotionLink(a1, 260 - 3);
		v3->Mode++;
		break;
	case 1:
		GET_CHAOWK(a1)->Behavior.SubTimer++;
		if (GET_CHAOWK(a1)->Behavior.SubTimer % 600 == 0)
			if (njRandom() > 0.5)
				AL_EmotionSetValue(a1, EM_ST_THIRSTY, 0);

		if (MOV_DistFromAim(a1) < 36.0)
			sub_561740((int)a1);

		a1->twp->pos.y = move->WaterY;
		MOV_TurnToAim2(a1, 100);
		float v8 = 0.005f;
		move->Acc.y = -move->Gravity - move->Velo.y * 0.1f;
		move->Acc.x = njSin(a1->twp->ang.y) * v8 - move->Velo.x * 0.05f;
		move->Acc.z = njCos(a1->twp->ang.y) * v8 - move->Velo.z * 0.05f;


		//AL_ForwardAcc(a1, ChaoGlobal.WalkAcc * 0.8f);

		task* pCommu = ALW_GetLockOnTask(a1);
		pCommu->twp->pos = a1->twp->pos;
		pCommu->twp->ang.y = a1->twp->ang.y;
		pCommu->twp->pos.x += njSin(a1->twp->ang.y);
		pCommu->twp->pos.y -= 2;
		pCommu->twp->pos.z += njCos(a1->twp->ang.y);
		break;
	}
	GET_CHAOWK(a1)->Behavior.Flag |= 1;
	return BHV_RET_CONTINUE;
}

int ALBHV_GoToBoat(task* tp) {
	task* v1; // edi

	v1 = AL_GetFoundToyTask(tp);
	if (!v1)
	{
		return BHV_RET_FINISH;
	}

	ALW_LockOn(tp, v1);
	AL_EmotionAdd(tp, EM_ST_THIRSTY, 100);
	
	//AL_GetRandomAttrPos(9, &a1->EntityData2->Waypoint);
	sub_534F80((int)& stru_1A15938[9], &GET_MOVE_WORK(tp)->AimPos, stru_1A15938[9].nbIndex);
	
	AL_SetBehavior(tp, ALBHV_PostureChangeStand);
	AL_SetNextBehavior(tp, ALBHV_Notice);

	if(!gConfigVal.PathfindingVanilla) {
		AL_SetNextBehavior(tp, ALBHV_ToyMoveCheck<ALBHV_GoToLockOn>);
	}
	else {
		AL_SetNextBehavior(tp, ALBHV_ToyMoveCheck<ALBHV_SetNaviTarget<NAVIGATION_TYPE::LOCKON>>);
		AL_SetNextBehavior(tp, ALBHV_ToyMoveCheck<ALBHV_CheckNavigate>);
		AL_SetNextBehavior(tp, ALBHV_ToyMoveCheck<ALBHV_Navigation>);
	}

	AL_SetNextBehavior(tp, ALBHV_PickUpLockOn);
	AL_SetNextBehavior(tp, ALBHV_GoToWaterWithBoat);
	AL_SetNextBehavior(tp, ALBHV_RideBoat);
	
	return BHV_RET_CONTINUE;
}