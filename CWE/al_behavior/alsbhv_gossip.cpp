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
			AL_FaceChangeMouth(data->chaoPointer, ((chaowk*)data->chaoPointer->Data1.Chao)->Face.MouthDefaultNum);
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
		if (((chaowk*)data->chaoPointer->Data1.Chao)->MotionTable.AnimID != data->parameter1 + 405)
			AL_SetMotionLink(data->chaoPointer, data->parameter1 + 405);
	}
	return 0;
}
void ALS_GossipSetup(ObjectMaster* a1, ObjectMaster* a2)
{
	ObjectMaster* social = Social_Create(ALS_GossipIdle);

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


int ALBHV_GossipTest(ObjectMaster* a1)
{
	ALS_GossipSetup(a1, ALW_GetLockOnTask(a1));
	return 0;
}

int ALBHV_SnapToGossip(ObjectMaster* a1)
{
	ObjectMaster* otherChao = ALW_IsCommunicating(a1)->tp;
	NJS_VECTOR v8;
	NJS_VECTOR testVectr = { 3.2f,0,0 };
	if(a1->Data1.Chao->Behavior.Mode == 0)
	{
		sub_54A690(a1);

		njPushUnitMatrix();
		njTranslateEx(&otherChao->Data1.Entity->Position);
		RotateY(otherChao->Data1.Entity->Rotation.y);
		sub_426CC0(_nj_current_matrix_ptr_, &v8, &testVectr, 0);
		njPopMatrixEx();

		a1->Data1.Entity->Position.x = (v8.x - a1->Data1.Entity->Position.x) * 0.1f + a1->Data1.Entity->Position.x;
		a1->Data1.Entity->Position.y = (v8.y - a1->Data1.Entity->Position.y) * 0.1f + a1->Data1.Entity->Position.y;
		a1->Data1.Entity->Position.z = (v8.z - a1->Data1.Entity->Position.z) * 0.1f + a1->Data1.Entity->Position.z;

		a1->Data1.Entity->Rotation.y = AdjustAngle_(a1->Data1.Entity->Rotation.y, otherChao->Data1.Entity->Rotation.y, 1024);

		v8.y = a1->Data1.Entity->Position.y;
		if (CheckDistance(&a1->Data1.Entity->Position, &v8) <= 0.2)
			return 1;
	}
	return 0;
}

void ALBHV_Gossip(ObjectMaster* a1, ObjectMaster* a2)
{
	ObjectMaster* otherChao = a2; //hardcoded select second chao
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
	njTranslateEx(&otherChao->Data1.Entity->Position);
	RotateY(otherChao->Data1.Entity->Rotation.y);
	sub_426CC0(_nj_current_matrix_ptr_, &a1->EntityData2->Waypoint, &finalPos, 0);
	//njCalcVector(&test, &otherChao->Data1.Entity->Position, _nj_current_matrix_ptr_);
	//TESTcalcpoint(0, &otherChao->Data1.Entity->Position, &otherChao->Data1.Entity->Position);
	njPopMatrixEx();

	//AL_SetBehavior(otherChao, ALBHV_WaitForSocialArrive); //wait for chao to arrive
	AL_SetBehavior(otherChao, ALBHV_LockUp);          //wait until other chao inits gossip

	AL_SetBehavior(a1, ALBHV_GoNextToSocial);                 //go to selected chao and turn to it	
	AL_SetNextBehavior(a1, ALBHV_SnapToGossip);
	AL_SetNextBehavior(a1, ALBHV_GossipTest);               //init talking

	sub_54A690(a1);
	ALW_SendCommand(a1, ALW_CMD_GO);
	ALW_SendCommand(otherChao, ALW_CMD_GO);
}