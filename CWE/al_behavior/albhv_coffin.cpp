#include "stdafx.h"
#include "..//SA2ModLoader.h"
#include "../Chao.h"
#include "../al_world.h"
#include "..//ninja_functions.h"
#include "../ALifeSDK_Functions.h"
#include <random>
#include "../AL_ModAPI.h"
#include "../ChaoMain.h"

signed int ALBHV_RideCoffin(ObjectMaster* a1)
{
	ChaoData1* v1; // esi
	AL_BEHAVIOR* v3; // esi
	float v2;

	v1 = (ChaoData1*)a1->Data1.Chao;
	v3 = &v1->Behavior;

	//SWIM CONTROL

	if (!(v1->Behavior.Flag & 4))
	{
		if (a1->EntityData2->field_40 & 0x400)
		{
			//AL_ForwardAcc(a1, 0.1f);
			AL_ForwardSpd(a1, 1);
			AL_SetBehavior(a1, (BHV_FUNC)0x00562D20);
		}
	}

	if (a1->EntityData2->field_40 & 0x4000)
	{
		c_colli_hit_info* v6 = CCL_IsHitKindEx(a1, 0xCB);
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
		v2 = njRandom();
		if (v2 >= 0.2f)
		{
			if (v2 < 0.4f)
			{
				AL_FaceChangeEye(a1, 4);
			}
		}
		else
		{
			AL_FaceChangeEye(a1, 1);
		}
		//AL_SetMotionLink(a1, 148 - 3);
		Chao_RegAnimation(a1, "alm_float");
		//AL_SetItem(a1, 0x22, &object_00F005A0, &AL_TOY_TEXLIST); //0x22
		AL_SetItem(a1, 0, (NJS_OBJECT*)0x01301BB4, (NJS_TEXLIST*)0x01366AFC);
		//AL_SetItem(a1, 0, (NJS_CNK_OBJECT*)0x03898530, &AL_TOY_TEXLIST); //coffin test

		//AL_SetMotionLink(a1, 260);
		v3->Mode++;
		break;
	case 1:
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
		pCommu->Data1.Entity->field_6 = 0;//dont render
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

signed int __cdecl ALBHV_GoToWaterWithBoat(ObjectMaster* a1);
signed int __cdecl ALBHV_GoToCoffin(ObjectMaster* a1)
{
	ObjectMaster* v1; // edi

	v1 = AL_GetFoundToyTask(a1);
	if (!v1)
	{
		return BHV_RET_FINISH;
	}
	ALW_LockOn(a1, v1);
	AL_EmotionAdd(a1, EM_ST_THIRSTY, 100);

	sub_534F80((int)& stru_1A15938[9], &a1->EntityData2->Waypoint, stru_1A15938[9].nbIndex);

	AL_SetBehavior(a1, (BHV_FUNC)0x0055C3C0);
	AL_SetNextBehavior(a1, (BHV_FUNC)0x056B480);
	AL_SetNextBehavior(a1, ALBHV_GoToLockOn);
	AL_SetNextBehavior(a1, (BHV_FUNC)0x5613C0);
	AL_SetNextBehavior(a1, ALBHV_GoToWaterWithBoat);
	AL_SetNextBehavior(a1, ALBHV_RideCoffin);
	return BHV_RET_CONTINUE;
}