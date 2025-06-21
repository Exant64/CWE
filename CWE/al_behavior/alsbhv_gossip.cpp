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
#include "albhv.h"

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
		if (!tp || tp == pChao || tp == pOtherChao) continue;

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
	Uint32 registeredEntryPairs[maxEntries][3];
	size_t nbEntries = 0;

	const auto addEntry = [&](Uint32 type, Uint32 id, Uint32 flags = 0) {
		if (nbEntries < maxEntries) {
			registeredEntryPairs[nbEntries][0] = type;
			registeredEntryPairs[nbEntries][1] = id;
			registeredEntryPairs[nbEntries][2] = flags;

			nbEntries++;
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

	const float favoriteFruitTopicChance = 0.4f;

	// this one is random so that chao that don't have too many topics don't always show this
	if (njRandom() < favoriteFruitTopicChance) {
		switch (param.Emotion.FavoriteFruit) {
		case TASTE_LS_DT:
		case TASTE_LS_DC:
			addEntry(SPEECH_BUBBLE_FRUIT, SA2BFruit_SquareFruit);
			break;
		case TASTE_LC_DT:
		case TASTE_LC_DS:
			addEntry(SPEECH_BUBBLE_FRUIT, SA2BFruit_RoundFruit);
			break;
		case TASTE_LT_DS:
		case TASTE_LT_DC:
			addEntry(SPEECH_BUBBLE_FRUIT, SA2BFruit_TriangleFruit);
			break;
		}
	}

	if (njRandom() < favoriteFruitTopicChance) {
		switch (param.Emotion.FavoriteFruit) {
			case TASTE_LS_DT:
			case TASTE_LC_DT:
				addEntry(SPEECH_BUBBLE_FRUIT, SA2BFruit_TriangleFruit, SPEECH_BUBBLE_FLAG_DISLIKE);
				break;
			case TASTE_LT_DS:
			case TASTE_LC_DS:
				addEntry(SPEECH_BUBBLE_FRUIT, SA2BFruit_SquareFruit, SPEECH_BUBBLE_FLAG_DISLIKE);
				break;
			case TASTE_LS_DC:
			case TASTE_LT_DC:
				addEntry(SPEECH_BUBBLE_FRUIT, SA2BFruit_RoundFruit, SPEECH_BUBBLE_FLAG_DISLIKE);
				break;
		}
	}

	if (param.Type == ChaoType_Child) {
		if (param.EvolutionProgress >= 0.9) {
			addEntry(SPEECH_BUBBLE_COCOON, 0);
		}
	}
	else if (param.Lifespan < 200) {
		if (param.Happiness < GET_GLOBAL()->SucceedBoundaryUserLike) {
			addEntry(SPEECH_BUBBLE_COCOON, 2);
		}
		else {
			addEntry(SPEECH_BUBBLE_COCOON, 1);
		}
	}

	if (nbEntries > 0) {
		const auto& pickedEntry = registeredEntryPairs[int(njRandom() * (nbEntries - 0.0001f))];
		AL_SpeechBubbleCreate(pChao, NULL, pickedEntry[0], pickedEntry[1], pos, GossipSpeechSpawnTimer, GossipSpeechStayTimer);
	}
}

static void ALS_GossipFace(SOCIAL_ACTOR* data) {
	task* pChao = data->chaoPointer;
	auto& bhv = data->bhvStatus;

	bhv.SubTimer++;

	if (bhv.SubTimer % 100 == 0) {
		float randval = njRandom();
		if (randval < 0.33f) {
			AL_FaceChangeEye(pChao, ChaoEyes_Normal);
		}
		else if (randval < 0.66f) {
			AL_FaceChangeEye(pChao, ChaoEyes_Mean);
		}
		else {
			AL_FaceChangeEye(pChao, ChaoEyes_Tiny);
		}
	}

	if (bhv.SubTimer % 120 == 0) {
		// random gossip sound in range of all gossip sounds
		PlaySound_XYZ(0x604B + njRandom() * (1 + 0x4F - 0x4B - 0.001f), &pChao->Data1.Entity->Position, 0, 0, 110);
	}

	if (bhv.SubTimer % 30 == 0) {
		AL_FaceSetMouth(pChao, ChaoMouth_Open, 15);
	}
}

static int ALS_Gossip(SOCIAL_ACTOR* data) {
	task* pChao = data->chaoPointer;
	auto& bhv = data->bhvStatus;
	ChaoData* pGossipChaoData = (ChaoData*)bhv.UserData;

	switch (bhv.Mode) {
		case 0:
			AL_SetMotionLinkStep(pChao, data->parameter1 + 404, 0x30);

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
				return 1;
			}

			ALS_GossipFace(data);
			break;
	}

	return 0;
}

static int ALS_GossipIdle(SOCIAL_ACTOR* data) {
	task* pChao = data->chaoPointer;
	auto& bhv = data->bhvStatus;

	switch (bhv.Mode) {
	case 0:
		if (GET_CHAOWK(pChao)->MotionTable.AnimID != data->parameter1 + 405)
			AL_SetMotionLinkStep(pChao, data->parameter1 + 405, 0x30);
		bhv.Mode++;
		break;
	case 1:
		bhv.SubTimer++;
		if ((bhv.SubTimer % 90) == 0) {
			float randval = njRandom();
			if (randval < 0.33f) {
				AL_FaceChangeEye(pChao, ChaoEyes_Normal);
			}
			else if (randval < 0.66f) {
				AL_FaceChangeEye(pChao, ChaoEyes_Mean);
			}
			else {
				AL_FaceChangeEye(pChao, ChaoEyes_Tiny);
			}

			if (njRandom() < 0.5) {
				AL_FaceChangeMouth(pChao, ChaoMouth_ClosedSmile);
			}
			else {
				AL_FaceChangeMouth(pChao, ChaoMouth_Open);
			}
		}

		break;
	}

	return 0;
}

static void ALS_GossipSetup(task* pChao1, task* pChao2) {
	ObjectMaster* social = Social_Create(ALS_GossipIdle);

	ALW_CommunicationOff(pChao1);

	//set up "actors"
	Social_SetActor(social, SOCIAL_CHAO1, pChao1);
	Social_SetActor(social, SOCIAL_CHAO2, pChao2);

	Social_SetParameter(social, SOCIAL_CHAO1, 4 + ((njRandom() < 0.5) ? 2 : 0));
	Social_SetParameter(social, SOCIAL_CHAO2, 0 + ((njRandom() < 0.5) ? 2 : 0));

	const int behaviorCount = 3 + int(njRandom() * 3.0f);

	bool flipped = false;
	for (int i = 0; i < behaviorCount; i++) {
		if (njRandom() < 0.5f)
			flipped = !flipped;

		Social_QueueBehavior(social, !flipped ? SOCIAL_CHAO1 : SOCIAL_CHAO2, ALS_Gossip);
	}
}

static int ALBHV_StartGossip(task* tp) {
	ALS_GossipSetup(tp, ALW_GetLockOnTask(tp));

	return BHV_RET_CONTINUE;
}

static void GreetTest(task* pChao1, task* pChao2) {
	AL_SetBehavior(pChao2, ALBHV_WaitForSocialToArrive);
	AL_SetNextBehavior(pChao2, ALBHV_SocialCheck<ALBHV_HandShake>);
	AL_SetNextBehavior(pChao2, ALBHV_SocialCheck<ALBHV_LockUp>);

	AL_SetBehavior(pChao1, ALBHV_GoToSocial);                 //go to selected chao and turn to it
	AL_SetNextBehavior(pChao1, ALBHV_SocialCheck<ALBHV_HandShake>);              //shake hands
}

void ALBHV_Gossip(task* pChao1, task* pChao2) {
	// stop if trying to gossip with self
	if (pChao1 == pChao2) {
		return;
	}

	ALW_LockOn(pChao1, pChao2);
	ALW_LockOn(pChao2, pChao1);
	ALW_CommunicationOn(pChao1, pChao2);

	GreetTest(pChao1, pChao2);

	AL_SetNextBehavior(pChao1, ALBHV_GoNextToSocialNew);                 //go to selected chao and turn to it	
	AL_SetNextBehavior(pChao1, ALBHV_StartGossip);               //init talking

	sub_54A690(pChao1);
	ALW_SendCommand(pChao1, ALW_CMD_GO);
	ALW_SendCommand(pChao2, ALW_CMD_GO);
}