#include "stdafx.h"
#include "..//SA2ModLoader.h"
#include "..//Chao.h"
#include "../al_social.h"
#include "../al_world.h"
#include "../ALifeSDK_Functions.h"
#include "../ninja_functions.h"
#include "alsbhv.h"
#include <random>

//DataPointer(NJS_MATRIX, flt_25F02A0, 0x25F02A0);

int ALBHV_HoldHands_Left(ObjectMaster* a1)
{
	if (ALW_RecieveCommand(a1) == ALW_CMD_CHANGE) {
		//PrintDebug("ALW_CMD_CHANGE");
		return BHV_RET_BREAK;
	}
	ObjectMaster* otherChao;
	NJS_VECTOR v8;
	chaowk* wk = (chaowk*)a1->Data1.Chao;

	NJS_VECTOR testVectr;
	testVectr.x = 3;
	testVectr.y = 0;
	testVectr.z = 0;
	otherChao = ALW_IsCommunicating(a1)->tp;
	switch (wk->Behavior.Mode)
	{
	case 0:
		AL_SetMotionLink(a1, 394);
		AL_SetMotionLink(otherChao, 392);

		if (njRandom() > 0.5f)
			AL_FaceChangeEye(a1, ChaoEyes_ClosedHappy);
		else
			AL_FaceChangeEye(a1, ChaoEyes_Normal);
		AL_FaceChangeMouth(a1, ChaoMouth_ClosedSmile);
		wk->Behavior.Mode++;
		//sub_54A690(a1);
		break;
	case 1:
		

		njPushUnitMatrix();
		njTranslateEx(&a1->Data1.Entity->Position);
		RotateY(a1->Data1.Entity->Rotation.y);
		sub_426CC0(_nj_current_matrix_ptr_, &v8, &testVectr, 0);
		njPopMatrixEx();

		otherChao->Data1.Entity->Position.x = (v8.x - otherChao->Data1.Entity->Position.x) * 0.1f + otherChao->Data1.Entity->Position.x;
		otherChao->Data1.Entity->Position.y = (v8.y - otherChao->Data1.Entity->Position.y) * 0.1f + otherChao->Data1.Entity->Position.y;
		otherChao->Data1.Entity->Position.z = (v8.z - otherChao->Data1.Entity->Position.z) * 0.1f + otherChao->Data1.Entity->Position.z;

		otherChao->Data1.Entity->Rotation.y = AdjustAngle_(otherChao->Data1.Entity->Rotation.y, a1->Data1.Entity->Rotation.y, 1024);

		v8.y = otherChao->Data1.Entity->Position.y;
		if(CheckDistance(&v8, &otherChao->Data1.Entity->Position) <= 0.2)
			wk->Behavior.Mode++;
		break;
	case 2:
		otherChao = ALW_IsCommunicating(a1)->tp;

		//FIX THIS
		float backupY = otherChao->Data1.Entity->Position.y;
		njPushUnitMatrix();
		njTranslateEx(&a1->Data1.Entity->Position);
		njRotateY(NULL, a1->Data1.Entity->Rotation.y);
		sub_426CC0(_nj_current_matrix_ptr_, &otherChao->Data1.Entity->Position, &testVectr, 0);
		njPopMatrixEx();

		otherChao->Data1.Entity->Position.y = backupY;
		otherChao->Data1.Entity->Rotation.y = a1->Data1.Entity->Rotation.y;

		((UnknownData2*)a1->EntityData2)->Waypoint.y = a1->Data1.Entity->Position.y; //DISGUSTING hack

		MOV_TurnToAim2(a1, 384);
		float a2 = ChaoGlobal.WalkAcc * 0.4f;
		AL_ForwardAcc(a1, a2);
		float v5 = MOV_DistFromAim(a1);
		if (v5 >= 0)
		{
			if (v5 < 4)
			{
				return 1;
			}
		}

		break;
	}
	return 0;
}
//FunctionPointer(signed int, AL_GetRandomAttrPos_0, (ObjectMaster* a1), 0x0073AEE0);

int ALBHV_HoldHands_Right(ObjectMaster* a1)
{
	if (ALW_RecieveCommand(a1) == ALW_CMD_CHANGE) {
		//PrintDebug("ALW_CMD_CHANGE");
		return BHV_RET_BREAK;
	}
	chaowk* wk = (chaowk*)a1->Data1.Chao;
	switch (wk->Behavior.Mode)
	{
	case 0:
		//AL_SetMotionLink(a1, 392);
		if (njRandom() > 0.5)
			AL_FaceChangeEye(a1, ChaoEyes_ClosedHappy);
		else
			AL_FaceChangeEye(a1, ChaoEyes_Normal);
		AL_FaceChangeMouth(a1, ChaoMouth_ClosedSmile);
		wk->Behavior.Mode++;
		//sub_54A690(a1);
		break;
	case 1:
		break;
	}
	return 0;
}
int ALBHV_InitHoldHands(ObjectMaster* a1)
{
	ObjectMaster* otherChao = ALW_GetLockOnTask(a1); //hardcoded select second chao
	if (otherChao == a1)
	{
		return 1;
	}

	ALW_LockOn(a1, otherChao);
	ALW_LockOn(otherChao, a1);
	ALW_CommunicationOn(a1, otherChao);

	AL_GetRandomAttrPos_0(a1);

	//MOV_SetAimPos(a1, );

	AL_SetBehavior(otherChao, ALBHV_WaitForSocialArrive); //wait for chao to arrive
	AL_SetNextBehavior(otherChao, ALBHV_HoldHands_Right);
	//AL_SetNextBehavior(otherChao, ALBHV_HandShake);       //shake hands
	//AL_SetNextBehavior(otherChao, ALBHV_LockUp);          //wait until other chao steps back and inits talking

	AL_SetBehavior(a1, ALBHV_GoToLockOn);                 //go to selected chao and turn to it
	AL_SetNextBehavior(a1, ALBHV_HoldHands_Left);
	//AL_SetNextBehavior(a1, ALBHV_HandShake);              //shake hands
	//AL_SetNextBehavior(a1, ALBHV_StepBack);               //step back a bit so that theyre not super close to eachother when talking
	//AL_SetNextBehavior(a1, ALBHV_TalkTest);               //init talking

	ALW_SendCommand(a1, ALW_CMD_GO);
	return 0;
}
