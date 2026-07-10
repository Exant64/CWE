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
#include "al_intention.h"
#include "../ChaoMain.h"
#include "../data/cwe/object_common_cnk/CakeTest.h"
#include "../data/accessory/ala_birthdayhat.nja"

task* pBirthdayChao;

int ALBHV_Birthday(task* a1)
{
	NJS_VECTOR vel{ 0,0,0 };
	chaowk* wk = GET_CHAOWK(a1);
	switch (wk->Behavior.Mode)
	{
	case 0:
		AL_SetMotionLink(a1, 170);
		AL_FaceChangeEye(a1, ChaoEyes_ClosedHappy);
		AL_FaceChangeMouth(a1, ChaoMouth_ClosedSmile);

		sub_5669B0(a1, (int)&object_cake, 28);
		GET_CHAOWK(a1)->Shape.CurrObjectList[28]->pItemTexlist = &AL_SANDHOLE_TEXLIST;
		sub_5669B0(a1, (int)&hat_main, 16);
		GET_CHAOWK(a1)->Shape.CurrObjectList[16]->pItemTexlist = &BIRTHDAYHAT_TEXLIST;
		GET_CHAOWK(pBirthdayChao)->Behavior.FreeWork = 0;
		wk->Behavior.Mode = 1;
		break;
	case 1:
	//	if (pBirthdayChao->Data1.Chao->Behavior.FreeWork > 7)
		wk->Behavior.Timer++;
		if (wk->Behavior.Timer > 25 * 60) //25 seconds
			wk->Behavior.Mode++;
		break;
	case 2:
		//spawn the fruit eat it and end the whole thing
		task * slice = ALO_FruitCreate(CakeSliceID, &GET_CHAOWK(a1)->pos, 0, &vel, NULL);
		ALW_LockOn(a1, slice);
		AL_SetBehavior(a1, (BHV_FUNC)0x5613C0);
		AL_SetNextBehavior(a1, (BHV_FUNC)0x05607C0);
		break;
	}
	return BHV_RET_CONTINUE;
}

int ALBHV_Celebrate(task* a1)
{
	*(int*)0x01DBE574 = 0;
	NJS_VECTOR vel{};
	float v5;
	chaowk* wk = GET_CHAOWK(a1);
	switch (wk->Behavior.Mode)
	{
	case 0:
		ALW_LockOn(a1, pBirthdayChao);
		AL_SetMotionLink(a1, 100);

		wk->Behavior.Timer = 0;
		wk->Behavior.Mode++;		
	case 1:
		ALW_TurnToLockOn(a1, 384);
		AL_ForwardAcc(a1, ChaoGlobal.WalkAcc * 0.8f);
		v5 = ALW_CalcDistFromLockOn(a1);
		if (v5 >= 0)
		{
			if (v5 < 8.0f)
			{
				wk->Behavior.Mode++;
				wk->Behavior.Timer = 0;
			}
		}

		//new cooldown
		wk->Behavior.Timer++;
		if (wk->Behavior.Timer > 25 * 60) {
			return BHV_RET_FINISH;
		}

		break;
		
	case 2:
		AL_SetMotionLink(a1, 468);
		wk->Behavior.Mode++;
		break;
	case 3:
		ALW_TurnToLockOn(a1, 512);
		wk->Behavior.Timer++;
		if (wk->Behavior.Timer > 215)
		{
			wk->Behavior.Timer = 0;
			wk->Behavior.Mode++;
		}
		break;
	case 4:
		NJS_VECTOR pos = GET_CHAOWK(a1)->pos;
		pos.x += njSin(a1->twp->ang.y) * -2;
		pos.z += njCos(a1->twp->ang.y) * -2;
		task* slice = ALO_FruitCreate(CakeSliceID, &pos, 0, &vel, NULL);
		ALW_LockOn(a1, slice);
		//ALW_TurnToLockOn(a1, 3000);
		AL_SetBehavior(a1, (BHV_FUNC)0x0056B6C0);
		AL_SetNextBehavior(a1, (BHV_FUNC)0x5613C0);
		AL_SetNextBehavior(a1, (BHV_FUNC)0x05607C0);
		break;
	}
	return BHV_RET_CONTINUE;
}