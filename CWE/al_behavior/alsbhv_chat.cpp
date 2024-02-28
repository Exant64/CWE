#include "stdafx.h"
#include "..//SA2ModLoader.h"
#include "..//Chao.h"
#include "../al_social.h"
#include "../al_world.h"
#include "../ALifeSDK_Functions.h"
#include "alsbhv.h"
#include <random>

#define OTHERCHAO social, !flipped ? SOCIAL_CHAO2 : SOCIAL_CHAO1
#define MAINCHAO  social, !flipped ? SOCIAL_CHAO1 : SOCIAL_CHAO2

struct FACE_KEYFRAME
{
	unsigned char frame;
	unsigned char face;
	unsigned char mouth;
};

struct FACE_ANIM
{
	int frameCount;
	int nodeCount;
	FACE_KEYFRAME* frames;
};
FACE_KEYFRAME face00[] =
{
	{0, 0, 0},
	{10, 0, ChaoMouth_OpenSmile},
	{30, 0, 0},
	{50, 0, ChaoMouth_Open}
};

FACE_KEYFRAME face01[] =
{
	{0,  ChaoEyes_ClosedHappy, 0			 },
	{7,  ChaoEyes_ClosedHappy, ChaoMouth_Open},
	{20, ChaoEyes_Painful,     0},
	{40, ChaoEyes_Painful, ChaoMouth_Open},
	{60, ChaoEyes_ClosedHappy,     0},
};

FACE_KEYFRAME face02[] =
{
	{0,  ChaoEyes_Painful, ChaoMouth_WideOpen},
	{7,  ChaoEyes_Painful, 0},
	{20, ChaoEyes_Painful, ChaoMouth_WideOpen},
	{35, ChaoEyes_Painful, 0}
};

FACE_KEYFRAME face03[] =
{
	{0,  ChaoEyes_HalfClosed, ChaoMouth_WideOpen},
	{7,  ChaoEyes_HalfClosed, 0},
	{20, ChaoEyes_HalfClosed, ChaoMouth_WideOpen},
	{35, ChaoEyes_HalfClosed, 0}
};

FACE_KEYFRAME face04[] =
{
	{0,  ChaoEyes_HalfClosed, ChaoMouth_Open},
	{7,  ChaoEyes_HalfClosed, 0},
	{20, ChaoEyes_HalfClosed, ChaoMouth_Open},
	{35, ChaoEyes_HalfClosed, 0}
};
FACE_KEYFRAME face06[] =
{
	{0,  ChaoEyes_ClosedStraight, ChaoMouth_ClosedSmile},
	{30,  ChaoEyes_ClosedStraight, ChaoMouth_OpenSmile},
};

FACE_KEYFRAME face07[] =
{
	{0,  ChaoEyes_ClosedStraight, 0},
	{15,  ChaoEyes_ClosedStraight, ChaoMouth_Open},
};

FACE_KEYFRAME face08[] =
{
	{0,  ChaoEyes_ClosedUp, ChaoMouth_ClosedSmile},
	{20,  ChaoEyes_ClosedUp, ChaoMouth_Open},
	{40,  ChaoEyes_ClosedUp, ChaoMouth_OpenSmile},
};

FACE_KEYFRAME face09[] =
{
	{0,  ChaoEyes_Tiny, ChaoMouth_Open},
	{7,  ChaoEyes_Tiny, 0},
	{20, ChaoEyes_Tiny, ChaoMouth_Open},
	{35, ChaoEyes_Tiny, 0}
};

FACE_KEYFRAME face0A[] =
{
	{0,  ChaoEyes_ClosedHappy, 0			 },
	{7,  ChaoEyes_ClosedHappy, ChaoMouth_Open},
	{20, ChaoEyes_ClosedHappy,     0},
};

FACE_KEYFRAME face0B[] =
{
	{0,  ChaoEyes_ClosedStraight, 0			 },
	{10,  ChaoEyes_ClosedStraight, ChaoMouth_Open},
};

FACE_KEYFRAME face0C[] =
{
	{0,  0, 0			 },
	{10,  0, ChaoMouth_Open},
};

FACE_KEYFRAME face0E[] =
{
	{0,  ChaoEyes_ClosedStraight, ChaoMouth_ClosedSmile},
};

FACE_ANIM faces[] = {
	{70,4,face00},
	{68,5,face01},
	{40,4,face02},
	{40,4,face03},
	{40,4,face04},
	{40,4,face02}, //face 2 with wrinkle and one less frame?
	{60,2,face06},
	{30,2,face07},
	{60,3,face08},
	{35,4,face09},
	{21,3,face0A},
	{15,2,face0B},
	{15,2,face0C},
	{15,2,0},
	{15,2,face0E},
	{15,2,0},
};

#define VOICEBANK5(a1) 24576 + a1

int ALS_Laugh(SOCIALDATA* data)
{
	if (data->bhvStatus.Mode == 0)
	{
		data->bhvStatus.Mode++;
		//if (((chaowk*)data->chaoPointer->Data1.Chao)->pParamGC->Emotion.Personality[G_PER_KINDNESS] > 30)
			//AL_SetMotionLink(data->chaoPointer, data->parameter1 + 1 + 3);
		//else
		AL_SetMotionLink(data->chaoPointer, data->parameter1 + 5);
		AL_FaceChangeEye(data->chaoPointer, ChaoEyes_Painful);
		AL_FaceChangeMouth(data->chaoPointer, ChaoMouth_ClosedSmile);
		//PlaySoundXYZAlt(VOICEBANK5(47) /*0x48B*/ + (njRandom() * 4.0f), data->chaoPointer->Data1, 1, 140, data->chaoPointer->Data1->Position.x, data->chaoPointer->Data1->Position.y, data->chaoPointer->Data1->Position.z);
		PlaySound_XYZ(VOICEBANK5(47), &data->chaoPointer->Data1.Entity->Position, 0, 0, 110);
		data->bhvStatus.Timer = 1 * 60;
	}
	else
	{
		data->bhvStatus.Timer--;
		if (!data->bhvStatus.Timer)
		{
			//return to default face
			AL_FaceChangeEye(data->chaoPointer, ChaoEyes_Normal);
			AL_FaceChangeMouth(data->chaoPointer, ((chaowk*)data->chaoPointer->Data1.Chao)->Face.MouthDefaultNum);
			return 1;
		}
	}
	return 0;
}
int ALS_DoYouAgree(SOCIALDATA* data)
{
	if (data->bhvStatus.Mode == 0)
	{
		data->bhvStatus.Mode++;
		AL_SetMotionLink(data->chaoPointer, data->parameter1 + 3);
		AL_FaceChangeEye(data->chaoPointer, ChaoEyes_Normal);
		AL_FaceChangeMouth(data->chaoPointer, ChaoMouth_Open);
		data->bhvStatus.Timer = 1 * 60;
		//PlaySoundXYZAlt(VOICEBANK5(92), data->chaoPointer->Data1, 1, 140, data->chaoPointer->Data1->Position.x, data->chaoPointer->Data1->Position.y, data->chaoPointer->Data1->Position.z);
		PlaySound_XYZ(VOICEBANK5(92), &data->chaoPointer->Data1.Entity->Position, 0, 0, 110);
	}
	else
	{
		data->bhvStatus.Timer--;
		if (!data->bhvStatus.Timer)
		{
			//return to default face
			AL_FaceChangeEye(data->chaoPointer, ChaoEyes_Normal);
			AL_FaceChangeMouth(data->chaoPointer, ((chaowk*)data->chaoPointer->Data1.Chao)->Face.MouthDefaultNum);
			return 1;
		}
	}
	return 0;
}
int ALS_DoNotAgree(SOCIALDATA* data)
{
	if (data->bhvStatus.Mode == 0)
	{
		data->bhvStatus.Mode++;
		AL_SetMotionLink(data->chaoPointer, data->parameter1 + 4);
		AL_FaceChangeEye(data->chaoPointer, ChaoEyes_Painful);
		AL_FaceChangeMouth(data->chaoPointer, ChaoMouth_ClosedFrown);
		data->bhvStatus.Timer = 1 * 60;
		//PlaySoundXYZAlt(VOICEBANK5(89)/*0x4B5*/, data->chaoPointer->Data1, 1, 140, data->chaoPointer->Data1->Position.x, data->chaoPointer->Data1->Position.y, data->chaoPointer->Data1->Position.z);
		PlaySound_XYZ(VOICEBANK5(89), &data->chaoPointer->Data1.Entity->Position, 0, 0, 110);
	}
	else
	{
		data->bhvStatus.Timer--;
		if (!data->bhvStatus.Timer)
		{
			//return to default face
			AL_FaceChangeEye(data->chaoPointer, ChaoEyes_Normal);
			AL_FaceChangeMouth(data->chaoPointer, ((chaowk*)data->chaoPointer->Data1.Chao)->Face.MouthDefaultNum);
			return 1;
		}
	}
	return 0;
}
int ALS_Agree(SOCIALDATA* data)
{
	if (data->bhvStatus.Mode == 0)
	{
		data->bhvStatus.Mode++;
		AL_SetMotionLink(data->chaoPointer, data->parameter1 + 6);
		AL_FaceChangeEye(data->chaoPointer, ChaoEyes_ClosedHappy);
		AL_FaceChangeMouth(data->chaoPointer, ChaoMouth_None);
		data->bhvStatus.Timer = 1 * 60;
		//PlaySoundXYZAlt(VOICEBANK5(94)/*0x4BA*/, data->chaoPointer->Data1, 1, 140, data->chaoPointer->Data1->Position.x, data->chaoPointer->Data1->Position.y, data->chaoPointer->Data1->Position.z);
		PlaySound_XYZ(VOICEBANK5(94), &data->chaoPointer->Data1.Entity->Position, 0, 0, 110);
	}
	else
	{
		data->bhvStatus.Timer--;
		if (!data->bhvStatus.Timer)
		{
			//return to default face
			AL_FaceChangeEye(data->chaoPointer, ChaoEyes_Normal);
			AL_FaceChangeMouth(data->chaoPointer, ((chaowk*)data->chaoPointer->Data1.Chao)->Face.MouthDefaultNum);
			return 1;
		}
	}
	return 0;
}
static const void* const SE_CallV2_TIMERPtr = (void*)0x437590;
static inline void SE_CallV2_TIMER(ObjectMaster* obj, int a1, NJS_VECTOR* a2, char a4, char a5, __int16 a6)
{
	__asm
	{
		push dword ptr[a6]
		push dword ptr[a5]
		push dword ptr[a4]
		mov esi, a2
		mov edi, a1
		mov ebx, obj
		call SE_CallV2_TIMERPtr
		add esp, 12
	}
}
void __cdecl ALS_NegativeTalkFace(SOCIALDATA* data)
{
	data->bhvStatus.SubTimer--;
	int multiplier = 1;
	if (data->bhvStatus.SubTimer <= 0)
		data->bhvStatus.SubTimer = 60 * multiplier;

	if (data->bhvStatus.SubTimer % (60 * multiplier) == 0)
		if (CurrentChaoArea == NextChaoArea)
			SE_CallV2_TIMER(data->chaoPointer, VOICEBANK5(75) + (int)(njRandom() * 20.0f), &data->chaoPointer->Data1.Entity->Position, 0, 110, 100);
	//PlaySoundXYZAlt(/*0x4A7*/VOICEBANK5(75) + (njRandom() * 20.0f), data->chaoPointer->Data1, 1, 140, data->chaoPointer->Data1->Position.x, data->chaoPointer->Data1->Position.y, data->chaoPointer->Data1->Position.z);

	if (data->bhvStatus.SubTimer % (30 * multiplier) == 0)
		AL_FaceSetMouth(data->chaoPointer, ChaoMouth_Open, 15 * multiplier);
}
void __cdecl ALS_TalkFace(SOCIALDATA* data)
{
	FACE_ANIM* anim = &faces[data->parameter2];

	FACE_KEYFRAME* foundFrame = nullptr;

	int frames = data->bhvStatus.SubTimer % (anim->frameCount * 2);
	for (int ind = 0; ind < anim->nodeCount; ind++)
	{
		if (anim->frames && frames >= (anim->frames[ind].frame * 2))
		{
			foundFrame = &anim->frames[ind];
		}
	}
	if (foundFrame)
	{
		AL_FaceChangeEye(data->chaoPointer, foundFrame->face);
		if (foundFrame->mouth == 0)
			AL_FaceChangeMouth(data->chaoPointer, data->chaoPointer->Data1.Chao->Face.MouthDefaultNum);
		else
			AL_FaceChangeMouth(data->chaoPointer, foundFrame->mouth);
	}

	data->bhvStatus.SubTimer++;
	int multiplier = 1;
	//if (data->bhvStatus.SubTimer <= 0)
		//data->bhvStatus.SubTimer = 60 * multiplier;

	if (data->bhvStatus.SubTimer % (60 * multiplier) == 0)
		if (CurrentChaoArea == NextChaoArea)
			SE_CallV2_TIMER(data->chaoPointer, VOICEBANK5(75) + (int)(njRandom() * 20.0f), &data->chaoPointer->Data1.Entity->Position, 0, 110, 100);
	//PlaySoundXYZAlt(/*0x4A7*/VOICEBANK5(75) + (njRandom() * 20.0f), data->chaoPointer->Data1, 1, 140, data->chaoPointer->Data1->Position.x, data->chaoPointer->Data1->Position.y, data->chaoPointer->Data1->Position.z);

	//if (data->bhvStatus.SubTimer % (30 * multiplier) == 0)
	//	AL_FaceSetMouth(data->chaoPointer, ChaoMouth_Open, 15 * multiplier);
}

int ALS_Talk(SOCIALDATA* data)
{
	if (data->bhvStatus.Mode == 0)
	{
		data->bhvStatus.Mode++;
		AL_SetMotionLink(data->chaoPointer, data->parameter1 + 0);
		data->bhvStatus.Timer = 5 * 60;
		data->bhvStatus.SubTimer = 0;
		data->parameter2 = (int)(njRandom() * 10.0f);
	}
	else
	{
		data->chaoPointer->Data1.Chao->MotionTable.frameIncreaseSpeed_ = 0.75f + (((AL_EmotionGetValue(data->chaoPointer, EM_PER_AGRESSIVE) + 100) / 200.0f) * 0.45f);
		if (njRandom() < 0.005f)
			AL_SetMotionLink(data->chaoPointer, data->parameter1 + 2);
		data->bhvStatus.Timer--;
		if (!data->bhvStatus.Timer)
		{
			AL_FaceChangeEye(data->chaoPointer, ChaoEyes_Normal);
			AL_FaceChangeMouth(data->chaoPointer, ((chaowk*)data->chaoPointer->Data1.Chao)->Face.MouthDefaultNum);
			return 1;
		}
		ALS_TalkFace(data);
	}

	return 0;
}

int ALS_NegativeTalk(SOCIALDATA* data)
{
	if (data->bhvStatus.Mode == 0)
	{
		data->bhvStatus.Mode++;
		AL_SetMotionLink(data->chaoPointer, data->parameter1 + 0);
		data->bhvStatus.Timer = 5 * 60;
		data->bhvStatus.SubTimer = 0;
		AL_FaceChangeEye(data->chaoPointer, ChaoEyes_Mean);
		AL_FaceChangeMouth(data->chaoPointer, ChaoMouth_ClosedFrown);
		if (njRandom() > 0.5f)
			data->parameter1 = 432;
	}
	else
	{
		if (njRandom() < 0.005f)
		{
			if (njRandom() > 0.3f)
			{
				if (data->parameter1 != 425)
				{
					if (njRandom() > 0.5f)
						AL_SetMotionLink(data->chaoPointer, 515);
					else
						AL_SetMotionLink(data->chaoPointer, 480);
				}
				else
				{
					//if (njRandom() > 0.5f)
					AL_SetMotionLink(data->chaoPointer, 562);
					//else
						//AL_SetMotionLink(data->chaoPointer, 63);
				}
			}
			else
				AL_SetMotionLink(data->chaoPointer, data->parameter1 + 2);
		}
		data->bhvStatus.Timer--;
		if (!data->bhvStatus.Timer)
		{

			return 1;
		}
		ALS_NegativeTalkFace(data);
	}

	return 0;
}

int ALS_NegativeListen(SOCIALDATA* data)
{
	if (data->bhvStatus.Mode == 0)
	{
		data->bhvStatus.Mode++;
		AL_SetMotionLink(data->chaoPointer, data->parameter1 + 0);
		AL_FaceChangeEye(data->chaoPointer, ChaoEyes_Mean);
		AL_FaceChangeMouth(data->chaoPointer, ChaoMouth_None);
		//data->bhvStatus.Timer = 1 * 60;
	}
	else
	{
		AL_SetMotionSpd(data->chaoPointer, 0.2f);
	}
	return 0;
}

int ALS_Listen(SOCIALDATA* data)
{
	if (data->bhvStatus.Mode == 0)
	{
		data->bhvStatus.Mode++;
		AL_SetMotionLink(data->chaoPointer, data->parameter1 + 0);
		AL_FaceChangeEye(data->chaoPointer, ChaoEyes_Normal);
		AL_FaceChangeMouth(data->chaoPointer, ChaoMouth_None);
		//data->bhvStatus.Timer = 1 * 60;
	}
	else
	{
		AL_SetMotionSpd(data->chaoPointer, 0.2f);
	}
	return 0;
}

int ALS_GoodBye(SOCIALDATA* data)
{
	if (data->bhvStatus.Mode == 0)
	{
		data->bhvStatus.Mode++;
		AL_SetMotionLink(data->chaoPointer, 416);
		AL_FaceChangeEye(data->chaoPointer, ChaoEyes_ClosedHappy);
		AL_FaceChangeMouth(data->chaoPointer, ChaoMouth_None);
		data->bhvStatus.Timer = 60;

		//sound incase i actually code it
		//if(data->chaoPointer == Social_GetActor(social))
	}
	else
	{
		data->bhvStatus.Timer--;
		if (!data->bhvStatus.Timer)
		{
			//return to default face
			//AL_FaceChangeEye(data->chaoPointer, ChaoEyes_Normal);
			//AL_FaceChangeMouth(data->chaoPointer, ((chaowk*)data->chaoPointer->Data1)->Face.MouthDefaultNum);
			return 1;
		}
	}
	return 0;
}

//original rotation frames on the original version of the sassy animation
/*
NJS_MKEY_Alt stru_37EB918[20] =
{
  { 0, { 0, 0, 0 } },
  { 1, { 0, 0, 0 } },
  { 2, { 0, 809, 0 } },
  { 3, { 0, 3640, 0 } },
  { 4, { 65503, 10113, 0 } },
  { 5, { 65469, 18608, 0 } },
  { 6, { 0, 25485, 0 } },
  { 7, { 262, 29207, 0 } },
  { 8, { 627, 31311, 0 } },
  { 9, { 910, 32767, 0 } },
  { 10, { 1031, 35862, 0 } },
  { 11, { 910, 32767, 0 } },
  { 12, { 0, 0, 0 } },
  { 13, { 65461, 63337, 0 } },
  { 14, { 0, 3640, 0 } },
  { 15, { 65480, 14335, 0 } },
  { 16, { 0, 25485, 0 } },
  { 17, { 436, 30340, 0 } },
  { 18, { 910, 32767, 0 } },
  { 19, { 910, 32767, 0 } }
};
*/

int ALS_SassyBye(SOCIALDATA* data)
{
	if (data->bhvStatus.Mode == 0)
	{
		data->bhvStatus.Mode++;
		AL_SetMotionLink(data->chaoPointer, 450);
		AL_FaceChangeEye(data->chaoPointer, ChaoEyes_Mean);
		AL_FaceChangeMouth(data->chaoPointer, ChaoMouth_None);
		data->bhvStatus.Timer = 45;
		if(data->actorIndex == 0)
			PlaySound_XYZ(VOICEBANK5(55), &data->chaoPointer->Data1.Entity->Position, 0, 0, 110);
	}
	else if (data->bhvStatus.Mode == 1)
	{
		if(data->bhvStatus.Timer > (45 - 20))
			data->chaoPointer->Data1.Entity->Rotation.y += NJM_DEG_ANG(180.0f / 20.0f);
		data->bhvStatus.Timer--;
		if (!data->bhvStatus.Timer)
		{
			AL_SetMotionLink(data->chaoPointer, 100);
			data->bhvStatus.Mode++;
			data->bhvStatus.Timer = 120;
		}
	}
	else if (data->bhvStatus.Mode == 2)
	{
		AL_ForwardSpd(data->chaoPointer, 0.015f);
		data->bhvStatus.Timer--;
		if (!data->bhvStatus.Timer)
		{
			return 1;
		}
	}
	return 0;
}

int ALS_Lock(SOCIALDATA* data)
{
	return 0;
}

void ALS_ChatAgree(ObjectMaster* social, bool flipped)
{
	Social_QueueBehavior(MAINCHAO, ALS_DoYouAgree);

	if (njRandom() > 0.5f)
		Social_QueueBehavior(OTHERCHAO, ALS_Agree);
	else
		Social_QueueBehavior(OTHERCHAO, ALS_DoNotAgree);
}

void ALS_ChatLaugh(ObjectMaster* social, bool flipped)
{
	Social_QueueBehavior(OTHERCHAO, ALS_Laugh);
	if (njRandom() > 0.75f)
		Social_SetSync(MAINCHAO); //chance to laugh together
}

void ALS_ChatGoodbye(ObjectMaster* social, bool flipped)
{
	Social_QueueBehavior(social, SOCIAL_CHAO1, ALS_GoodBye); //goodbye
	Social_SetSync(social, SOCIAL_CHAO2);                    //sync goodbye action together
}

void ALS_ChatSassybye(ObjectMaster* social, bool flipped)
{
	Social_QueueBehavior(social, SOCIAL_CHAO1, ALS_SassyBye); //goodbye
	Social_SetSync(social, SOCIAL_CHAO2);                    //sync goodbye action together
}

float ALS_AngryChance(ObjectMaster* chao, ObjectMaster* otherChao)
{
	ChaoDataBase* data = chao->Data1.Chao->pParamGC;
	ChaoDataBase* otherData = otherChao->Data1.Chao->pParamGC;

	float chance = 0;
	if (AL_EmotionGetValue(chao, EM_PER_CALM) < -30)
		chance += 0.02f;
	if (AL_EmotionGetValue(chao, EM_PER_KINDNESS) > 45)
		chance -= 0.01f;
	if (AL_EmotionGetValue(chao, EM_PER_AGRESSIVE) > 60)
		chance += 0.035f;
	if (AL_EmotionGetValue(chao, EM_PER_FICKLE) > 50)
		chance += 0.015f;
	bool foundDislikedFruit = false;
	switch (data->Emotion.FavoriteFruit)
	{
	case TASTE_LS_DT:
	case TASTE_LC_DT:
		//dislike triangle
		if (otherData->Emotion.FavoriteFruit == TASTE_LT_DS ||
			otherData->Emotion.FavoriteFruit == TASTE_LT_DC)
			foundDislikedFruit = true;
		break;
	case TASTE_LT_DS:
	case TASTE_LC_DS:
		//dislike square
		if (otherData->Emotion.FavoriteFruit == TASTE_LS_DT ||
			otherData->Emotion.FavoriteFruit == TASTE_LS_DC)
			foundDislikedFruit = true;
		break;
	case TASTE_LS_DC:
	case TASTE_LT_DC:
		//dislike circle
		if (otherData->Emotion.FavoriteFruit == TASTE_LC_DT ||
			otherData->Emotion.FavoriteFruit == TASTE_LC_DS)
			foundDislikedFruit = true;
		break;

	}
	if (foundDislikedFruit)
		chance += njRandom() * 0.045f;

	//0.015 + 0.035 + 0.02 + (0.03 or a bit more from favorite fruit) = 0.1
	return chance;
}

void ALS_ChatSetup(ObjectMaster* a1, ObjectMaster* a2)
{
	ObjectMaster* social = Social_Create(ALS_Listen);

	ALW_CommunicationOff(a1);

	//set up "actors"
	Social_SetActor(social, SOCIAL_CHAO1, a1);
	Social_SetActor(social, SOCIAL_CHAO2, a2);

	//parameter here is used as "starting animation index"
	if (njRandom() >= 0.5f)
		Social_SetParameter(social, SOCIAL_CHAO1, 425);
	else
		Social_SetParameter(social, SOCIAL_CHAO1, 432);

	if (njRandom() >= 0.5f)
		Social_SetParameter(social, SOCIAL_CHAO2, 425);
	else
		Social_SetParameter(social, SOCIAL_CHAO2, 432);

	//set up initial default animations
	//AL_SetMotionLink(Social_GetActor(social, SOCIAL_CHAO1), 425 + 3);
	//AL_SetMotionLink(Social_GetActor(social, SOCIAL_CHAO2), 425 + 3);
	bool angry = false;
	bool flipped = 0; //this value is so that the conversation goes back and forth
	int talkingTopicCount = 4 + (int)(njRandom() * 6.99f);

	for (int i = 0; i < talkingTopicCount; i++)
	{
		bool doYouAgree = njRandom() > 0.6f;

		if (njRandom() > 0.5f)
			flipped = !flipped;

		if (angry)
		{
			Social_QueueBehavior(MAINCHAO, ALS_NegativeTalk);
			if (njRandom() < 0.25f)
			{
				angry = false;
				Social_SetIdle(social, ALS_Listen);
			}
		}
		else
		{
			Social_QueueBehavior(MAINCHAO, ALS_Talk);
			if (doYouAgree)
			{
				ALS_ChatAgree(social, flipped);
			}
			else
			{
				if (!angry)
				{
					if (njRandom() < ALS_AngryChance(
							Social_GetActor(OTHERCHAO),
							Social_GetActor(MAINCHAO)
						))
					{
						Social_QueueBehavior(OTHERCHAO, ALS_NegativeTalk);
						angry = true;
						Social_SetIdle(social, ALS_NegativeListen);
					}
					else if (njRandom() > 0.75f)
					{
						ALS_ChatLaugh(social, flipped);
					}
				}
			}
		}

	}
	if (!angry)
	{
		ALS_ChatGoodbye(social, flipped);
	}
	else
	{
		ALS_ChatSassybye(social, flipped);
	}
	//i didnt use social system + sync for handshake because im not sure how to handle the "Stepback" part

	//Social_QueueBehavior(social, SOCIAL_CHAO1, ALS_Lock);
	//Social_QueueBehavior(social, SOCIAL_CHAO2, ALS_Lock);
}

int ALBHV_TalkTest(ObjectMaster* a1)
{
	ALS_ChatSetup(a1, ALW_GetLockOnTask(a1));
	return 0;
}

int ALBHV_HandShake(ObjectMaster* a1)
{
	chaowk* wk = (chaowk*)(a1->Data1.Chao);
	if (wk->Behavior.Mode == 0)
	{
		AL_SetMotionLink(a1, 402);
		wk->Behavior.Timer = 60;
		wk->Behavior.Mode++;
	}
	else
	{
		wk->Behavior.Timer--;
		if (!wk->Behavior.Timer)
		{
			//return to default face
			//AL_FaceChangeEye(a1, ChaoEyes_Normal);
			//AL_FaceChangeMouth(a1, );
			//ALBHV_TalkTest(a1);
			return 1;
		}
	}
	return 0;
}
int ALBHV_StepBack(ObjectMaster* a1)
{
	chaowk* wk = (chaowk*)(a1->Data1.Chao);
	if (wk->Behavior.Mode == 0)
	{
		AL_SetMotionLinkStep(a1, 100, 15);
		wk->Behavior.Timer = 75;
		wk->Behavior.Mode++;
	}
	else
	{
		AL_ForwardSpd(a1, -0.015f);
		wk->Behavior.Timer--;
		if (!wk->Behavior.Timer)
		{
			//return to default face
			//AL_FaceChangeEye(a1, ChaoEyes_Normal);
			//AL_FaceChangeMouth(a1, );
			//ALBHV_TalkTest(a1);
			return 1;
		}
	}
	return 0;
}

int ALBHV_Talk(ObjectMaster* a1)
{
	ObjectMaster* otherChao = ALW_GetLockOnTask(a1); //hardcoded select second chao
	if (otherChao == a1)
	{
		// stop if trying to talk to self
		return 1;
	}

	ALW_LockOn(a1, otherChao);
	ALW_LockOn(otherChao, a1);
	ALW_CommunicationOn(a1, otherChao);

	AL_SetBehavior(otherChao, ALBHV_WaitForSocialArrive); //wait for chao to arrive
	AL_SetNextBehavior(otherChao, ALBHV_HandShake);       //shake hands
	AL_SetNextBehavior(otherChao, ALBHV_LockUp);          //wait until other chao steps back and inits talking

	AL_SetBehavior(a1, ALBHV_GoToSocial);                 //go to selected chao and turn to it
	AL_SetNextBehavior(a1, ALBHV_HandShake);              //shake hands
	AL_SetNextBehavior(a1, ALBHV_StepBack);               //step back a bit so that theyre not super close to eachother when talking
	AL_SetNextBehavior(a1, ALBHV_TalkTest);               //init talking

	ALW_SendCommand(a1, ALW_CMD_GO);
	return 0;
}