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

ObjectMaster* pBirthdayChao;

int ALBHV_Birthday(ObjectMaster* a1)
{
	NJS_VECTOR vel{ 0,0,0 };
	ChaoData1* wk = CHAOWK(a1);
	switch (wk->Behavior.Mode)
	{
	case 0:
		AL_SetMotionLink(a1, 170);
		AL_FaceChangeEye(a1, ChaoEyes_ClosedHappy);
		AL_FaceChangeMouth(a1, ChaoMouth_ClosedSmile);

		sub_5669B0(a1, (int)&object_cake, 28);
		a1->Data1.Chao->field_524[28]->toy.texlist = &AL_SANDHOLE_TEXLIST;
		sub_5669B0(a1, (int)&hat_main, 16);
		a1->Data1.Chao->field_524[16]->toy.texlist = &BIRTHDAYHAT_TEXLIST;
		pBirthdayChao->Data1.Chao->Behavior.FreeWork = 0;
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
		ObjectMaster * slice = ALO_FruitExecutor_Load(CakeSliceID, &a1->Data1.Chao->entity.Position, 0, &vel, 0);
		ALW_LockOn(a1, slice);
		AL_SetBehavior(a1, (BHV_FUNC)0x5613C0);
		AL_SetNextBehavior(a1, (BHV_FUNC)0x05607C0);
		break;
	}
	return BHV_RET_CONTINUE;
}

int ALBHV_Celebrate(ObjectMaster* a1)
{
	*(int*)0x01DBE574 = 0;
	NJS_VECTOR vel{};
	float v5;
	ChaoData1* wk = CHAOWK(a1);
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
		NJS_VECTOR pos = a1->Data1.Chao->entity.Position;
		pos.x += njSin(a1->Data1.Entity->Rotation.y) * -2;
		pos.z += njCos(a1->Data1.Entity->Rotation.y) * -2;
		ObjectMaster* slice = ALO_FruitExecutor_Load(CakeSliceID, &pos, 0, &vel, 0);
		ALW_LockOn(a1, slice);
		//ALW_TurnToLockOn(a1, 3000);
		AL_SetBehavior(a1, (BHV_FUNC)0x0056B6C0);
		AL_SetNextBehavior(a1, (BHV_FUNC)0x5613C0);
		AL_SetNextBehavior(a1, (BHV_FUNC)0x05607C0);
		break;
	}
	return BHV_RET_CONTINUE;
}