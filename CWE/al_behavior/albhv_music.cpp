#include "stdafx.h"
#include "..//SA2ModLoader.h"
#include "..//Chao.h"
#include "../al_social.h"
#include "../al_world.h"
#include "../ALifeSDK_Functions.h"
#include "../ninja_functions.h"
#include "albhv.h"
#include <random>
#include "al_knowledge.h"
#include "..//AL_ModAPI.h"
#include "al_intention.h"
#include "../data/toy/al_toy_triangle.nja"
#include "../data/toy/alo_accordion.nja"
#include "../data/toy/alo_guitar.nja"
#include "../ChaoMain.h"
#include <al_draw.h>

BHVFunc(ALBHV_ListenMusic, 0x0059E710);

int ALBHV_Guitar(ObjectMaster* tp) {
	AL_BEHAVIOR* bhv = &GET_CHAOWK(tp)->Behavior;

	switch (bhv->Mode) {
	case 0:
		Chao_RegAnimation(tp, "alm_guitar");
		AL_FaceChangeEye(tp, 4);
		AL_FaceChangeMouth(tp, 3);

		sub_5669B0(tp, (int)&object_alo_guitar, AL_PART_HAND_R);

		//hack, normally the function above should ask for the texlist, but since sega always used
		//al_toy, the function got automatically optimized to always use al_toy inside the code
		//so i gotta do it manually
		//todo: fix with decomp function in al_parts or something		
		tp->Data1.Chao->field_524[AL_PART_HAND_R]->toy.texlist = &CWE_OBJECT_TEXLIST;
		
		++bhv->Mode;
		bhv->Timer = (unsigned __int16)(300 + (signed int)(njRandom() * 301.f));
	case 1:
		bhv->SubTimer++;
		if ((bhv->SubTimer % 180) == 0 && njRandom() < 0.5 && CurrentChaoArea == NextChaoArea)
		{
			PlaySound_XYZ((24576 + 162), &tp->Data1.Entity->Position, 0, 0, 56);
		}
		break;
	}

	return ALO_Field_Find_(tp, 1, CI_KIND_AL_RANDOM_MUSIC) == NULL;//if the band/music field is gone, end the action
}

int ALBHV_Triangle(ObjectMaster* tp) {
	AL_BEHAVIOR* bhv = &GET_CHAOWK(tp)->Behavior;

	switch (bhv->Mode) {
	case 0:
		Chao_RegAnimation(tp, "alm_triangle");
		AL_FaceChangeEye(tp, 4);
		AL_FaceChangeMouth(tp, 3);
		sub_5669B0(tp, (int)&Cylinder, AL_PART_HAND_L);
		sub_5669B0(tp, (int)&Cylinder_001, AL_PART_HAND_R);
		++bhv->Mode;
		bhv->Timer = (unsigned __int16)(300 + (signed int)(njRandom() * 301.f));
	case 1:
		bhv->SubTimer++;
		if ((bhv->SubTimer % 180) == 0 && njRandom() < 0.5f && CurrentChaoArea == NextChaoArea)
		{
			PlaySound_XYZ((24576 + 163), &tp->Data1.Entity->Position, 0, 0, 75);
		}
		break;
	}
	return ALO_Field_Find_(tp, 1, CI_KIND_AL_RANDOM_MUSIC) == NULL; //if the band/music field is gone, end the action
}

int ALBHV_Accordion(ObjectMaster* tp) {
	AL_BEHAVIOR* bhv = &GET_CHAOWK(tp)->Behavior;

	switch (bhv->Mode) {
	case 0:
		Chao_RegAnimation(tp, "alm_accordion");
		AL_FaceChangeEye(tp, 4);
		AL_FaceChangeMouth(tp, 3);
		sub_5669B0(tp, (int)&object_alo_accordion, AL_PART_ARM_R);
		++bhv->Mode;
		bhv->Timer = (unsigned __int16)(300 + (signed int)(njRandom() * 301.f));
	case 1:
		bhv->SubTimer++;
		if ((bhv->SubTimer % 180) == 0 && njRandom() < 0.5 && CurrentChaoArea == NextChaoArea)
		{
			PlaySound_XYZ((24576 + 161), &tp->Data1.Entity->Position, 0, 0, 56);
		}
		break;
	}

	return ALO_Field_Find_(tp, 1, CI_KIND_AL_RANDOM_MUSIC) == NULL;
}

extern "C" __declspec(dllexport) const BHV_FUNC ALBHV_MusicFunc_CWE[NB_AL_MUSIC_CWE] = {
	(BHV_FUNC)0x59D8E0,
	(BHV_FUNC)0x59DE90,
	(BHV_FUNC)0x59E2D0,
	(BHV_FUNC)0x59DA40,
	(BHV_FUNC)0x59DD10,
	(BHV_FUNC)0x59D5A0,
	(BHV_FUNC)0x59E120,
	(BHV_FUNC)0x59E500,
	ALBHV_Guitar,
	ALBHV_Triangle,
	ALBHV_Accordion
};

Bool __cdecl AL_DecideBehaviorMusic(task* tp) {
	chaowk* work = GET_CHAOWK(tp);

	int InstList[NB_AL_MUSIC_CWE];
	int count = 0;

	//removed the redundant CCL_ClearSearch call	
	if (!ALO_Field_Find_(tp, 1, CI_KIND_AL_RANDOM_MUSIC)) {
		int i;
		for (i = 0; i < NB_AL_MUSIC_CWE; i++) {
			if (AL_KW_IsMusicFlagOn(tp, i)) {
				InstList[count] = i;
				count++;
			}
		}

		if (count > 0) {
			NJS_POINT3 music_center;
			Uint32 kind = InstList[(int)(njRandom() * (count - 0.00001f))];

			int ang = work->entity.Rotation.y;
			music_center.x = njSin(ang) * 8 + work->entity.Position.x;
			music_center.y = work->entity.Position.y;
			music_center.z = njCos(ang) * 8 + work->entity.Position.z;

			AL_SetBehavior(tp, ALBHV_MusicFunc_CWE[kind]);
			ALOField_Load(tp, CI_KIND_AL_RANDOM_MUSIC, &music_center, 20, (int)(1800 + (njRandom() * 2401.f)));
			return TRUE;
		}
	}

	return FALSE;
}

void AL_CalcIntentionScore_JoinMusic(ObjectMaster* tp, float* pMaxScore) {
	float score = 0.0f;
	Uint32 trigger = GET_GLOBAL()->IntentionHimaTrigger >> 1; //div by 2?
	Uint32 value = AL_EmotionGetValue(tp, EM_ST_TEDIOUS);
	task* pField = ALO_Field_Find_(tp, 1, CI_KIND_AL_RANDOM_MUSIC);

	int InstList[NB_AL_MUSIC_CWE];
	int count = 0;

	if (*pMaxScore < 1.0) {
		if (pField) {
			int i;

			for (i = 0; i < NB_AL_MUSIC_CWE; i++) {
				if (AL_KW_IsMusicFlagOn(tp, i)) {
					InstList[count] = i;
					count++;
				}
			}

			if (value > trigger) {
				if (value < 7000) {
					value += 3000;
				}

				score = AL_CalcScoreTypeA(value, trigger);
				score *= GET_GLOBAL()->IntentionHimaMaxScore;
				AL_ScoreRandomize(&score);
			}

			if (score > *pMaxScore) {
				if (count > 0) {
					Uint32 kind = InstList[(int)(njRandom() * (count - 0.00001f))];

					MOV_SetAimPos(tp, &pField->Data1.Entity->Position);

					AL_SetBehavior(tp, ALBHV_TurnToAim);
					AL_SetNextBehavior(tp, ALBHV_MusicFunc_CWE[kind]);

					AL_EmotionAdd(tp, EM_ST_TEDIOUS, -4000);
					AL_EmotionAdd(tp, EM_ST_LONELY, -4000);
					//AL_ClearIntention(tp);

					if (pField->Parent) {
						AL_EmotionAdd(pField->Parent, EM_ST_LONELY, -4000);
					}
				}
				else {
					if (pField->Parent) {
						MOV_SetAimPos(tp, &pField->Parent->Data1.Entity->Position);
					}
					AL_SetBehavior(tp, ALBHV_TurnToAim);
					AL_SetNextBehavior(tp, ALBHV_PostureChangeSit);
					AL_SetNextBehavior(tp, ALBHV_ListenMusic);
				}
				*pMaxScore = 0.99f;
			}
		}
	}
}