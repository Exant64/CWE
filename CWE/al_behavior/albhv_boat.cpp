#include "stdafx.h"
#include "..//SA2ModLoader.h"
#include "../Chao.h"
#include "../al_world.h"
#include "..//ninja_functions.h"
#include "../ALifeSDK_Functions.h"
#include <random>
#include "albhv.h"
#include "../AL_ModAPI.h"

signed int __cdecl ALBHV_GoToWaterWithBoat(ObjectMaster* a1)
{
	ChaoData1* v1; // esi
	int v2; // eax
	AL_BEHAVIOR* v3; // esi
	float a2; // ST08_4
	double v5; // st7
	int v7; // eax

	v1 = (ChaoData1*)a1->Data1.Chao;
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
		sub_54A690(a1);

		if (v7 > 30)
		{
			AL_SetMotionLink(a1, 194 - 3);
			++v3->Mode;
		}
	}
	AL_SetMotionSpd(a1, 1.5);

	((ChaoData1*)a1->Data1.Chao)->Behavior.Flag |= 1;		 //block water detection?
	UnknownData2* mov = (UnknownData2*)a1->EntityData2;
	if (a1->Data1.Entity->Position.y + 2.0 < mov->field_DC)
		return BHV_RET_FINISH;
	return AL_MoveHoldingObject(a1) == 0;
}
signed int ALBHV_RideBoat(ObjectMaster* a1)
{
	ChaoData1* v1; // esi
	AL_BEHAVIOR* v3; // esi

	v1 = (ChaoData1*)a1->Data1.Chao;
	v3 = &v1->Behavior;

	
	//SWIM CONTROL
	ALW_CommunicationOn(a1, ALW_GetLockOnTask(a1));
	if (!(v1->Behavior.Flag & 4))
	{
		if (a1->EntityData2->field_40 & 0x400)
		{
			//AL_ForwardAcc(a1, 0.1f);
			AL_ForwardSpd(a1, 1);
			AL_SetBehavior(a1, (BHV_FUNC)0x00562D20);
		}
	}
	if (AL_EmotionGetValue(a1, EM_ST_THIRSTY) < 1000 && a1->EntityData2->field_40 & 0x4000)
	{
		c_colli_hit_info* v6 = CCL_IsHitKindEx(a1, 0xCB); //climb
		if (v6 && v6->hit_twp)
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
		a1->Data1.Chao->Behavior.SubTimer++;
		if (a1->Data1.Chao->Behavior.SubTimer % 600 == 0)
			if (njRandom() > 0.5)
				AL_EmotionSet(a1, EM_ST_THIRSTY, 0);

		if (MOV_DistFromAim(a1) < 36.0)
			sub_561740((int)a1);

		a1->Data1.Entity->Position.y = a1->EntityData2->field_DC;
		MOV_TurnToAim2(a1, 100);
		float v8 = 0.005f;
		a1->EntityData2->speed.y = -a1->EntityData2->gravity - a1->EntityData2->velocity.y * 0.1f;
		a1->EntityData2->speed.x = njSin(a1->Data1.Entity->Rotation.y) * v8 - a1->EntityData2->velocity.x * 0.05f;
		a1->EntityData2->speed.z = njCos(a1->Data1.Entity->Rotation.y) * v8 - a1->EntityData2->velocity.z * 0.05f;


		//AL_ForwardAcc(a1, ChaoGlobal.WalkAcc * 0.8f);

		ObjectMaster* pCommu = ALW_GetLockOnTask(a1);
		pCommu->Data1.Entity->Position = a1->Data1.Entity->Position;
		pCommu->Data1.Entity->Rotation.y = a1->Data1.Entity->Rotation.y;
		pCommu->Data1.Entity->Position.x += njSin(a1->Data1.Entity->Rotation.y);
		pCommu->Data1.Entity->Position.y -= 2;
		pCommu->Data1.Entity->Position.z += njCos(a1->Data1.Entity->Rotation.y);
		break;
	}
	((ChaoData1*)a1->Data1.Chao)->Behavior.Flag |= 1;
	return BHV_RET_CONTINUE;
}

signed int __cdecl ALBHV_GoToBoat(ObjectMaster* a1)
{
	ObjectMaster* v1; // edi

	v1 = AL_GetFoundToyTask(a1);
	if (!v1)
	{
		return BHV_RET_FINISH;
	}

	ALW_LockOn(a1, v1);
	AL_EmotionAdd(a1, EM_ST_THIRSTY, 100);
	
	//AL_GetRandomAttrPos(9, &a1->EntityData2->Waypoint);
	sub_534F80((int)& stru_1A15938[9], &a1->EntityData2->Waypoint, stru_1A15938[9].nbIndex);
	
	AL_SetBehavior(a1, ALBHV_PostureChangeStand);
	AL_SetNextBehavior(a1, (BHV_FUNC)0x056B480);
	AL_SetNextBehavior(a1, ALBHV_GoToLockOn);
	AL_SetNextBehavior(a1, (BHV_FUNC)0x5613C0);
	AL_SetNextBehavior(a1, ALBHV_GoToWaterWithBoat);
	AL_SetNextBehavior(a1, ALBHV_RideBoat);
	//AL_SetNextBehavior(a1, ALBHV_TurnToLockOn);
	//AL_SetNextBehavior(a1, ALBHV_WatchTV);
	return BHV_RET_CONTINUE;
}