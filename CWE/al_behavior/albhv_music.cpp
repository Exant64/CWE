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
#include <playsound.h>
#include "../data/toy/al_toy_triangle.nja"
#include "../data/toy/alo_accordion.nja"
#include "../data/toy/alo_guitar.nja"
#include "../ChaoMain.h"
#include <al_draw.h>

int ALBHV_Guitar(task* tp) {
	AL_BEHAVIOR* bhv = &GET_CHAOWK(tp)->Behavior;

	switch (bhv->Mode) {
	case 0:
		Chao_RegAnimation(tp, "alm_guitar");
		AL_FaceChangeEye(tp, 4);
		AL_FaceChangeMouth(tp, 3);

		AL_SetItem(tp, AL_PART_HAND_R, &object_alo_guitar, texlist_cwe_object);
		
		++bhv->Mode;
		bhv->Timer = (unsigned __int16)(300 + (signed int)(njRandom() * 301.f));
	case 1:
		bhv->SubTimer++;
		if ((bhv->SubTimer % 180) == 0 && njRandom() < 0.5) {
			AL_SE_CallV2(TONE(6, CWE_SOUND_GUITAR), 0, 0, 56, &tp->twp->pos);
		}
		break;
	}

	return AL_IsHitKindWithNum(tp, 1, CI_KIND_AL_RANDOM_MUSIC) == NULL;//if the band/music field is gone, end the action
}

static int ALBHV_Triangle(task* tp) {
	AL_BEHAVIOR* bhv = &GET_CHAOWK(tp)->Behavior;

	switch (bhv->Mode) {
		case 0:
			AL_SetMotionLinkStep(tp, ALM_CASTANETS_WAIT, 10);

			AL_FaceChangeEye(tp, ChaoEyes_ClosedUp);
			AL_FaceChangeMouth(tp, ChaoMouth_ClosedSmile);
			
			AL_SetItem(tp, AL_PART_HAND_L, &Cylinder, &AL_TOY_TEXLIST);
			AL_SetItem(tp, AL_PART_HAND_R, &Cylinder_001, &AL_TOY_TEXLIST);

			bhv->Mode++;
			bhv->Timer = 300 + int(njRandom() * 301.f);
		case 1:
			// we're reusing the CASTANETS_WAIT animation instead of a custom one
			// code is based off of ALBHV_Castanets in sa2dc decomp
		 	if (bhv->Timer-- <= 0 && AL_IsMotionEnd(tp)) {
                switch (AL_GetMotionNum(tp)) {
                    case ALM_CASTANETS_WAIT:
                        if (njRandom() < 0.6f) {
                            Chao_RegAnimation(tp, "alm_triangle_2h");
							bhv->SubMode = 1;
                        } else {
                            Chao_RegAnimation(tp, "alm_triangle_1h");
							bhv->SubMode = 0;
                        }

                        bhv->Timer = 120 + int(njRandom() * 120.f);
                        bhv->SubTimer = 0;
                        break;
                    default:
                        AL_SetMotionLinkStep(tp, ALM_CASTANETS_WAIT, 10);

                        bhv->Timer = 60 + int(njRandom() * 180.f);
                        bhv->SubTimer = 0;
                        break;
                }
            }

			// reset the sound timer once the animation looped
			if(AL_IsMotionEnd(tp)) {
				bhv->SubTimer = 0;
			}
			else {
            	bhv->SubTimer++;
			}

            if (AL_GetMotionNum(tp) != ALM_CASTANETS_WAIT) {
                if(!bhv->SubMode) {
                    if (bhv->SubTimer == 34) {
                        AL_SE_CallV2(TONE(6, CWE_SOUND_TRIANGLE), 0, 0, 75, &tp->twp->pos);
                    }
				}
                else {
                    if ((bhv->SubTimer == 34 || bhv->SubTimer == 65)) {
                        AL_SE_CallV2(TONE(6, CWE_SOUND_TRIANGLE), 0, 0, 75, &tp->twp->pos);
                    }
				}
            }

			break;
	}

	return AL_IsHitKindWithNum(tp, 1, CI_KIND_AL_RANDOM_MUSIC) == NULL;
}

int ALBHV_Accordion(task* tp) {
	AL_BEHAVIOR* bhv = &GET_CHAOWK(tp)->Behavior;

	switch (bhv->Mode) {
		case 0:
			Chao_RegAnimation(tp, "alm_accordion");
			
			AL_FaceChangeEye(tp, 4);
			AL_FaceChangeMouth(tp, 3);

			AL_SetItem(tp, AL_PART_ARM_R, &object_alo_accordion, &AL_TOY_TEXLIST);

			++bhv->Mode;
			bhv->Timer = (unsigned __int16)(300 + (signed int)(njRandom() * 301.f));
		case 1:
			bhv->SubTimer++;
			if ((bhv->SubTimer % 180) == 0 && njRandom() < 0.5) {
				AL_SE_CallV2(TONE(6, CWE_SOUND_ACCORDION), 0, 0, 56, &tp->twp->pos);
			}
			break;
	}

	return AL_IsHitKindWithNum(tp, 1, CI_KIND_AL_RANDOM_MUSIC) == NULL;
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

Bool AL_DecideBehaviorMusic_r(task* tp) {
	chaowk* work = GET_CHAOWK(tp);

	int InstList[NB_AL_MUSIC_CWE];
	int count = 0;

	//removed the redundant CCL_ClearSearch call	
	if (!AL_IsHitKindWithNum(tp, 1, CI_KIND_AL_RANDOM_MUSIC)) {
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

			int ang = work->ang.y;
			music_center.x = njSin(ang) * 8 + work->pos.x;
			music_center.y = work->pos.y;
			music_center.z = njCos(ang) * 8 + work->pos.z;

			AL_SetBehavior(tp, ALBHV_MusicFunc_CWE[kind]);
			AL_ChildFieldCreateT(tp, CI_KIND_AL_RANDOM_MUSIC, &music_center, 20, (int)(1800 + (njRandom() * 2401.f)));
			return TRUE;
		}
	}

	return FALSE;
}

void AL_CalcIntentionScore_JoinMusic(task* tp, float* pMaxScore) {
	float score = 0.0f;
	Uint32 trigger = GET_GLOBAL()->IntentionHimaTrigger >> 1; //div by 2?
	Uint32 value = AL_EmotionGetValue(tp, EM_ST_TEDIOUS);
	task* pField = AL_IsHitKindWithNum(tp, 1, CI_KIND_AL_RANDOM_MUSIC);

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

					MOV_SetAimPos(tp, &pField->twp->pos);

					AL_SetBehavior(tp, ALBHV_TurnToAim);
					AL_SetNextBehavior(tp, ALBHV_MusicFunc_CWE[kind]);

					AL_EmotionAdd(tp, EM_ST_TEDIOUS, -4000);
					AL_EmotionAdd(tp, EM_ST_LONELY, -4000);
					//AL_ClearIntention(tp);

					if (pField->ptp) {
						AL_EmotionAdd(pField->ptp, EM_ST_LONELY, -4000);
					}
				}
				else {
					if (pField->ptp) {
						MOV_SetAimPos(tp, &pField->ptp->twp->pos);
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