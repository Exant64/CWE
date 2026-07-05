#include "stdafx.h"
#include "..//SA2ModLoader.h"
#include "../Chao.h"
#include "../al_world.h"
#include "..//ninja_functions.h"
#include "../ALifeSDK_Functions.h"
#include <random>
#include "albhv.h"
#include "../AL_ModAPI.h"
#include "../ChaoMain.h"

signed int ALBHV_RideFloat(task* a1)
{
	chaowk* v1; // esi
	AL_BEHAVIOR* v3; // esi
	float v2;

	v1 = (chaowk*)GET_CHAOWK(a1);
	v3 = &v1->Behavior;

	//SWIM CONTROL
	ALW_CommunicationOn(a1, ALW_GetLockOnTask(a1));
	if (!(v1->Behavior.Flag & 4))
	{
		if (a1->EntityData2->Flag & 0x400)
		{
			//AL_ForwardAcc(a1, 0.1f);
			AL_ForwardSpd(a1, 1);
			AL_SetBehavior(a1, (BHV_FUNC)0x00562D20);
		}
	}

	if (AL_EmotionGetValue(a1, EM_ST_THIRSTY) < 1000 && a1->EntityData2->Flag & 0x4000)
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
		AL_SetItem(a1, 0, (NJS_OBJECT*)0x013005A0, (NJS_TEXLIST*)0x01366AFC);
		//AL_SetItem(a1, 0, (NJS_CNK_OBJECT*)0x03898530, &AL_TOY_TEXLIST); //coffin test

		//AL_SetMotionLink(a1, 260);
		v3->Mode++;
		break;
	case 1:
		GET_CHAOWK(a1)->Behavior.SubTimer++;
		if (GET_CHAOWK(a1)->Behavior.SubTimer % 600 == 0)
			if (njRandom() > 0.6f)
				AL_EmotionSet(a1, EM_ST_THIRSTY, 0);

		if (MOV_DistFromAim(a1) < 36.0)
			sub_561740((int)a1);

		a1->twp->pos.y = a1->EntityData2->WaterY;
		MOV_TurnToAim2(a1, 100);
		float v8 = 0.005f;
		a1->EntityData2->Acc.y = -a1->EntityData2->Gravity - a1->EntityData2->Velo.y * 0.1f;
		a1->EntityData2->Acc.x = njSin(a1->twp->ang.y) * v8 - a1->EntityData2->Velo.x * 0.05f;
		a1->EntityData2->Acc.z = njCos(a1->twp->ang.y) * v8 - a1->EntityData2->Velo.z * 0.05f;


		//AL_ForwardAcc(a1, ChaoGlobal.WalkAcc * 0.8f);

		task* pCommu = ALW_GetLockOnTask(a1);
		pCommu->twp->wtimer = 0;//dont render
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

signed int __cdecl ALBHV_GoToWaterWithBoat(task* a1);
signed int __cdecl ALBHV_GoToFloat(task* a1)
{
	task* v1; // edi

	v1 = AL_GetFoundToyTask(a1);
	if (!v1)
	{
		return BHV_RET_FINISH;
	}
	ALW_LockOn(a1, v1);
	AL_EmotionAdd(a1, EM_ST_THIRSTY, 100);

	//sub_534F80((int)& stru_1A15938[9], &a1->EntityData2->Waypoint, stru_1A15938[9].index);

	//tbh, im not sure what this is, i think we use this as the first waypoint to go to 
	//(but why don't we just select a random water waypoint like it does afterwards?)
	NJS_POINT3 waterWaypointMaybe = { 75, -6.5f, -60 };
	MOV_SetAimPos(a1, &waterWaypointMaybe);

	AL_SetBehavior(a1, ALBHV_PostureChangeStand);
	AL_SetNextBehavior(a1, (BHV_FUNC)0x056B480);
	AL_SetNextBehavior(a1, ALBHV_GoToLockOn);
	AL_SetNextBehavior(a1, (BHV_FUNC)0x5613C0);
	AL_SetNextBehavior(a1, ALBHV_GoToWaterWithBoat);
	AL_SetNextBehavior(a1, ALBHV_RideFloat);
	return BHV_RET_CONTINUE;
}