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

int ALBHV_Hug1(ObjectMaster* a1)
{
	if (ALW_RecieveCommand(a1) == ALW_CMD_CHANGE) {
		//PrintDebug("ALW_CMD_CHANGE");
		return BHV_RET_BREAK;
	}
	ObjectMaster* otherChao;
	chaowk* wk = (chaowk*)a1->Data1.Chao;

	otherChao = ALW_IsCommunicating(a1)->tp;
	switch (wk->Behavior.Mode)
	{
	case 0:
		AL_SetMotionLink(a1, 412);
		AL_SetMotionLink(otherChao, 412);

		AL_FaceChangeEye(a1, ChaoEyes_ClosedHappy);
		AL_FaceChangeMouth(a1, ChaoMouth_ClosedSmile);

		AL_IconSet(a1, 4, 90);
		AL_IconSet(otherChao, 4, 90);

		wk->Behavior.Mode++;
		//sub_54A690(a1);
		break;

	case 1:
		ALW_TurnToLockOn(a1, 384);
		wk->Behavior.Timer++;
		if (wk->Behavior.Timer > 90)
			return 1;
		break;
	}
	return 0;
}
//FunctionPointer(signed int, AL_GetRandomAttrPos_0, (ObjectMaster* a1), 0x0073AEE0);

int ALBHV_Hug2(ObjectMaster* a1)
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
		AL_FaceChangeEye(a1, ChaoEyes_ClosedHappy);
		
		AL_FaceChangeMouth(a1, ChaoMouth_ClosedSmile);
		wk->Behavior.Mode++;
		//sub_54A690(a1);
		break;
	case 1:
		ALW_TurnToLockOn(a1, 384);
		break;
	}
	return 0;
}
signed int __cdecl ALBHV_GoToHug(ObjectMaster* a1)
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
		if (v5 >= 0)
		{
			if (v5 < 2)
			{
				return 1;
			}
		}
		else
		{
			AL_SetBehavior(a1, ALBHV_Think);
		}
		break;
	}
	AL_SetMotionSpd(a1, 1.5f);

	v3->LimitTimer--;
	if (!v3->LimitTimer)
	{
		AL_SetBehavior(a1, ALBHV_Think);
	}
	return 0;
}
int ALBHV_InitHug(ObjectMaster* a1)
{
	ObjectMaster* otherChao = ALW_GetLockOnTask(a1); //hardcoded select second chao
	if (otherChao == a1)
	{
		return 1;
	}

	ALW_LockOn(a1, otherChao);
	ALW_LockOn(otherChao, a1);
	ALW_CommunicationOn(a1, otherChao);

	//AL_GetRandomAttrPos_0(a1);

	MOV_SetAimPos(a1, &otherChao->Data1.Entity->Position);

	AL_SetBehavior(otherChao, ALBHV_WaitForSocialArrive); //wait for chao to arrive
	AL_SetNextBehavior(otherChao, ALBHV_Hug2);
	//AL_SetNextBehavior(otherChao, ALBHV_HandShake);       //shake hands
	//AL_SetNextBehavior(otherChao, ALBHV_LockUp);          //wait until other chao steps back and inits talking

	AL_SetBehavior(a1, ALBHV_GoToHug);                 //go to selected chao and turn to it
	AL_SetNextBehavior(a1, ALBHV_Hug1);
	//AL_SetNextBehavior(a1, ALBHV_HandShake);              //shake hands
	//AL_SetNextBehavior(a1, ALBHV_StepBack);               //step back a bit so that theyre not super close to eachother when talking
	//AL_SetNextBehavior(a1, ALBHV_TalkTest);               //init talking

	ALW_SendCommand(a1, ALW_CMD_GO);
	return 0;
}
