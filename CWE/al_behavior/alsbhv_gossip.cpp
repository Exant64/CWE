#include "stdafx.h"
#include "..//SA2ModLoader.h"
#include "..//Chao.h"
#include "../al_social.h"
#include "../al_world.h"
#include "../ALifeSDK_Functions.h"
#include "alsbhv.h"
#include "../al_social.h"
#include "../ninja_functions.h"
#include <random>

void ALS_GossipFace(SOCIALDATA* data)
{
	data->bhvStatus.SubTimer--;

	if (!data->bhvStatus.SubTimer)
		data->bhvStatus.SubTimer = 60;

	//if (data->bhvStatus.SubTimer % 60 == 0);
	//PlaySoundXYZAlt(/*0x4A7*/VOICEBANK5(75) + (njRandom() * 20.0f), data->chaoPointer->Data1, 1, 140, data->chaoPointer->Data1->Position.x, data->chaoPointer->Data1->Position.y, data->chaoPointer->Data1->Position.z);

	if (data->bhvStatus.SubTimer % 30 == 0)
		AL_FaceSetMouth(data->chaoPointer, ChaoMouth_Open, 15);
}
int ALS_Gossip(SOCIALDATA* data)
{
	if (data->bhvStatus.Mode == 0)
	{
		data->bhvStatus.Mode++;
		AL_SetMotionLink(data->chaoPointer, data->parameter1 + 404);
		data->bhvStatus.Timer = 5 * 60;
		data->bhvStatus.SubTimer = 0;
	}
	else
	{
		data->bhvStatus.Timer--;
		if (!data->bhvStatus.Timer)
		{
			AL_FaceChangeEye(data->chaoPointer, ChaoEyes_Normal);
			AL_FaceChangeMouth(data->chaoPointer, GET_CHAOWK(data->chaoPointer)->Face.MouthDefaultNum);
			//AL_SetMotionLink(data->chaoPointer, data->parameter1 + 405 + 3);
			return 1;
		}
		ALS_GossipFace(data);
	}

	return 0;
}
int ALS_GossipIdle(SOCIALDATA* data)
{
	if (data->bhvStatus.Mode == 0)
	{
		data->bhvStatus.Mode++;
		if (GET_CHAOWK(data->chaoPointer)->MotionCtrl.curr_num != data->parameter1 + 405)
			AL_SetMotionLink(data->chaoPointer, data->parameter1 + 405);
	}
	return 0;
}
void ALS_GossipSetup(task* a1, task* a2)
{
	task* social = Social_Create(ALS_GossipIdle);

	ALW_CommunicationOff(a1);

	//set up "actors"
	Social_SetActor(social, SOCIAL_CHAO1, a1);
	Social_SetActor(social, SOCIAL_CHAO2, a2);

	Social_SetParameter(social, SOCIAL_CHAO1, 4);
	Social_SetParameter(social, SOCIAL_CHAO2, 0);

	bool flipped = false; //this value is so that the conversation goes back and forth
	for (int i = 0; i < 4 + (njRandom() * 6.0f); i++)
	{
		if (njRandom() > 0.5f)
			flipped = !flipped;

		Social_QueueBehavior(social, !flipped ? SOCIAL_CHAO1 : SOCIAL_CHAO2, ALS_Gossip);
	}
}


int ALBHV_GossipTest(task* a1)
{
	ALS_GossipSetup(a1, ALW_GetLockOnTask(a1));
	return 0;
}

int ALBHV_SnapToGossip(task* a1)
{
	task* otherChao = ALW_IsCommunicating(a1)->tp;
	NJS_VECTOR v8;
	NJS_VECTOR testVectr = { 3.2f,0,0 };
	if(GET_CHAOWK(a1)->Behavior.Mode == 0)
	{
		AL_FixPosition(a1);

		njPushUnitMatrix();
		njTranslateEx(&otherChao->twp->pos);
		RotateY(otherChao->twp->ang.y);
		sub_426CC0(_nj_current_matrix_ptr_, &v8, &testVectr, 0);
		njPopMatrixEx();

		a1->twp->pos.x = (v8.x - a1->twp->pos.x) * 0.1f + a1->twp->pos.x;
		a1->twp->pos.y = (v8.y - a1->twp->pos.y) * 0.1f + a1->twp->pos.y;
		a1->twp->pos.z = (v8.z - a1->twp->pos.z) * 0.1f + a1->twp->pos.z;

		a1->twp->ang.y = AdjustAngle_(a1->twp->ang.y, otherChao->twp->ang.y, 1024);

		v8.y = a1->twp->pos.y;
		if (CheckDistance(&a1->twp->pos, &v8) <= 0.2)
			return 1;
	}
	return 0;
}

void ALBHV_Gossip(task* a1, task* a2)
{
	task* otherChao = a2; //hardcoded select second chao
	if (otherChao == a1)
	{
		// stop if trying to gossip with self
		return;
	}
	ALW_LockOn(a1, otherChao);
	ALW_LockOn(otherChao, a1);
	ALW_CommunicationOn(a1, otherChao);

	NJS_VECTOR finalPos = { 3,0,0 };

	njPushUnitMatrix();
	njTranslateEx(&otherChao->twp->pos);
	RotateY(otherChao->twp->ang.y);
	sub_426CC0(_nj_current_matrix_ptr_, &GET_MOVE_WORK(a1)->AimPos, &finalPos, 0);
	//njCalcVector(&test, &otherChao->twp->Position, _nj_current_matrix_ptr_);
	//TESTcalcpoint(0, &otherChao->twp->Position, &otherChao->twp->Position);
	njPopMatrixEx();

	//AL_SetBehavior(otherChao, ALBHV_WaitForSocialArrive); //wait for chao to arrive
	AL_SetBehavior(otherChao, ALBHV_LockUp);          //wait until other chao inits gossip

	AL_SetBehavior(a1, ALBHV_GoNextToSocial);                 //go to selected chao and turn to it	
	AL_SetNextBehavior(a1, ALBHV_SnapToGossip);
	AL_SetNextBehavior(a1, ALBHV_GossipTest);               //init talking

	AL_FixPosition(a1);
	ALW_SendCommand(a1, ALW_CMD_GO);
	ALW_SendCommand(otherChao, ALW_CMD_GO);
}