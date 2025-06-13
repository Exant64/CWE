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
#include <al_speechbubble.h>
#include <al_behavior/al_knowledge.h>

// gets chao he doesn't know (
static ChaoData* GetGossipDoesntKnowSubject(task* pChao, task* pOtherChao) {
	chaowk* work = GET_CHAOWK(pChao);

	const size_t maxGossip = 25;
	ChaoData* pGossipList[maxGossip];
	size_t nbGossip = 0;

	const size_t chaoCount = ALW_CountEntry(0);
	for (size_t i = 0; i < chaoCount; i++) {
		if (nbGossip >= maxGossip) break;

		task* tp = GetChaoObject(0, i);
		if (tp == pChao || tp == pOtherChao) continue;

		pGossipList[nbGossip++] = (ChaoData*)GET_CHAOPARAM(tp);
	}

	if (!nbGossip) return NULL;

	return pGossipList[int(njRandom() * (nbGossip - 0.0001f))];
}

// gets chao that he personally knows (met with more than once) or parents
static ChaoData* GetGossipKnowsSubject(task* pChao) {
	chaowk* work = GET_CHAOWK(pChao);

	const size_t maxGossip = 25;
	ChaoData* pGossipList[maxGossip];
	size_t nbGossip = 0;

	const size_t nbFriends = AL_KW_GetFriendCount(pChao);

	for (size_t i = 0; i < nbFriends; i++) {
		if (nbGossip >= maxGossip) break;
		
		const auto& entry = GET_CHAOPARAM(pChao)->Knowledge.chao[i];
		if (entry.meet > 1) {
			ChaoData* pParam = (ChaoData*)AL_KW_FindChaoBasedOnId(entry.id);
			if (!pParam) continue;

			pGossipList[nbGossip++] = pParam;
		}
	}

	if (!nbGossip) return NULL;

	return pGossipList[int(njRandom() * (nbGossip - 0.0001f))];
}

static ChaoData* GetGossipSubject(task* pChao) {
	ChaoData* pDoesntKnow = GetGossipDoesntKnowSubject(pChao, ALW_GetLockOnTask(pChao));
	ChaoData* pKnows = GetGossipKnowsSubject(pChao);

	ChaoData* pPickedChao = NULL;
	if (!pDoesntKnow || !pKnows) {
		return !pDoesntKnow ? pKnows : pDoesntKnow;
	}
	
	return (njRandom() < 0.5) ? pDoesntKnow : pKnows;
}

static const int GossipSpeechSpawnTimer = 45;
static const int GossipSpeechStayTimer = 90;
static const int GossipTotalTimer = GossipSpeechSpawnTimer * 2 + GossipSpeechStayTimer;

static void PickAndSpawnTopicGossipChao(task* pChao, ChaoData* gossipData, eSpeechPos pos) {
	auto& param = gossipData->data;
	const size_t maxEntries = 32;
	Uint32 registeredEntryPairs[maxEntries][2];
	size_t nbEntries = 0;

	const auto addEntry = [&](Uint32 type, Uint32 id) {
		if (nbEntries < maxEntries) {
			registeredEntryPairs[nbEntries][0] = type;
			registeredEntryPairs[nbEntries++][1] = id;
		}
	};

	for (size_t i = 0; i < _countof(param.Accessories); i++) {
		if (!param.Accessories[i]) continue;

		addEntry(SPEECH_BUBBLE_ACCESSORY, param.Accessories[i] - 1);
	}

	if (param.Headgear) {
		addEntry(SPEECH_BUBBLE_MASK, param.Headgear);
	}

	if (param.Medal) {
		addEntry(SPEECH_BUBBLE_MEDAL, param.Medal - 1);
	}

	for (size_t i = 0; i < NB_AL_STOY; i++) {
		if (param.Knowledge.SToyFlag & (1 << i)) {
			addEntry(SPEECH_BUBBLE_STOY, i);
		}
	}

	for (size_t i = 0; i < NB_AL_MUSIC; i++) {
		if (param.Knowledge.MusicFlag & (1 << i)) {
			addEntry(SPEECH_BUBBLE_MUSIC, i);
		}
	}

	if (nbEntries > 0) {
		const auto& pickedEntry = registeredEntryPairs[int(njRandom() * (nbEntries - 0.0001f))];
		AL_SpeechBubbleCreate(pChao, NULL, pickedEntry[0], pickedEntry[1], pos, GossipSpeechSpawnTimer, GossipSpeechStayTimer);
	}
}

static void ALS_GossipFace(SOCIALDATA* data) {
	data->bhvStatus.SubTimer--;

	if (!data->bhvStatus.SubTimer)
		data->bhvStatus.SubTimer = 60;

	//if (data->bhvStatus.SubTimer % 60 == 0);
	//PlaySoundXYZAlt(/*0x4A7*/VOICEBANK5(75) + (njRandom() * 20.0f), data->chaoPointer->Data1, 1, 140, data->chaoPointer->Data1->Position.x, data->chaoPointer->Data1->Position.y, data->chaoPointer->Data1->Position.z);

	if (data->bhvStatus.SubTimer % 30 == 0)
		AL_FaceSetMouth(data->chaoPointer, ChaoMouth_Open, 15);
}

static int ALS_Gossip(SOCIALDATA* data) {
	task* pChao = data->chaoPointer;
	auto& bhv = data->bhvStatus;
	ChaoData* pGossipChaoData = (ChaoData*)bhv.UserData;

	switch (bhv.Mode) {
		case 0:
			AL_SetMotionLink(pChao, data->parameter1 + 404 + ((njRandom() < 0.5) ? 2 : 0));

			bhv.UserData = GetGossipSubject(pChao);

			bhv.Timer = 2 * GossipTotalTimer + (3 + njRandom() * 4) * 60;
			bhv.SubTimer = 0;

			bhv.Mode++;
			break;
		// mode 1 and 2 cover talking about a chao and then talking about something related to said chao
		default:
			bhv.Timer--;

			if ((bhv.Timer % GossipTotalTimer) == 0) {
				if (pGossipChaoData) {
					// first talk about the chao in mode 1
					// then talk about some trait of chao in mode 2
					switch (bhv.Mode) {
					case 1:
						AL_SpeechBubbleCreate(pChao, pGossipChaoData, -1, 0, SPEECH_POS_TOP, GossipSpeechSpawnTimer, GossipSpeechStayTimer);
						break;
					case 2:
						PickAndSpawnTopicGossipChao(pChao, pGossipChaoData, SPEECH_POS_TOP);
						break;
					}
					
					bhv.Mode++;
				}
			}

			if (!bhv.Timer)
			{
				AL_FaceChangeEye(pChao, ChaoEyes_Normal);

				// AL_FaceReturnDefaultMouth
				AL_FaceChangeMouth(pChao, GET_CHAOWK(pChao)->Face.MouthDefaultNum);
				//AL_SetMotionLink(data->chaoPointer, data->parameter1 + 405 + 3);
				return 1;
			}

			ALS_GossipFace(data);
			break;
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