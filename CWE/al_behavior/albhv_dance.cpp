#include "stdafx.h"
#include "..//SA2ModLoader.h"
#include "..//Chao.h"
#include "../al_world.h"
#include "../ALifeSDK_Functions.h"
#include "../ninja_functions.h"
#include <random>
#include "al_knowledge.h"
#include "..//AL_ModAPI.h"
#include "al_intention.h"
#include "../ChaoMain.h"

int ALBHV_DanceHero(ObjectMaster* tp) {
    AL_BEHAVIOR* bhv = &GET_CHAOWK(tp)->Behavior;

    switch(bhv->Mode) {
    case 0:
		AL_FaceChangeEye(tp, 4);
		Chao_RegAnimation(tp, "alm_dance_h");
	    bhv->Mode++;
	}

    if (ALO_Field_Find_(tp, 1, CI_KIND_AL_DANCE))
        return BHV_RET_CONTINUE;
    else
        return BHV_RET_FINISH;
}
int __cdecl ALBHV_DanceDark(ObjectMaster* tp) {
    AL_BEHAVIOR* bhv = &GET_CHAOWK(tp)->Behavior;

    switch (bhv->Mode) {
    case 0:
        AL_FaceChangeEye(tp, 4);
        Chao_RegAnimation(tp, "alm_dance_d");
        bhv->Mode++;
    }

    if (ALO_Field_Find_(tp, 1, CI_KIND_AL_DANCE))
        return BHV_RET_CONTINUE;
    else
        return BHV_RET_FINISH;
}

//broke the naming convention oops
//not the same bhv as the sadx one, since that one is an animal behavior it doesnt check for the dance field, and it ends when the motion ends
int ALBHV_BreakDance(ObjectMaster* tp)
{
    chaowk* work = CHAOWK(tp);
    AL_BEHAVIOR* bhv = &work->Behavior;
    
    switch (bhv->Mode) {
    case 0:
        AL_SetMotionLink(tp, ALM_K_BREAKDANCE);
        bhv->Mode++;
    case 1:
        if (AL_IsMotionEnd(tp)) {
            bhv->Mode = 0;
        }
        break;
    }
    
    if (ALO_Field_Find_(tp, 1, CI_KIND_AL_DANCE))
        return BHV_RET_CONTINUE;
    else
        return BHV_RET_FINISH;
}

extern "C" __declspec(dllexport) BHV_FUNC ALBHV_DanceFunc_CWE[] = { 
    (BHV_FUNC)0x59C870, 
    (BHV_FUNC)0x59C980,
    (BHV_FUNC)0x59CA90,
    (BHV_FUNC)0x59CB20,
    (BHV_FUNC)0x59CBF0,
    ALBHV_DanceHero,
    ALBHV_DanceDark,
    ALBHV_BreakDance
};

Bool AL_DecideBehaviorDance(ObjectMaster* tp)
{
    NJS_POINT3 center;
	int InstList[32];
	int nb = 0;

	for (int i = 0; i < NB_AL_DANCE_CWE; i++) {
		if (AL_KW_IsDanceFlagOn(tp, i)) {
			if (ALBHV_DanceFunc_CWE[i]) {
                InstList[nb++] = i;
			}
		}
	}

    //ugly: we store how many vanilla dances we have unlocked before we add our custom ones to the array
    //this is useful for the "all dances unlocked" check for the breakdance
    int origDanceFlagCount = nb;

    if (nb <= 0) {
        return FALSE;
    }

    center.x = njSin(tp->Data1.Chao->entity.Rotation.y) * 8 + tp->Data1.Chao->entity.Position.x;
    center.y = tp->Data1.Chao->entity.Position.y;
    center.z = njCos(tp->Data1.Chao->entity.Rotation.y) * 8 + tp->Data1.Chao->entity.Position.z;
    
    //i overcomplicated it like this with a switch case to support the original DanceFunc array if any other mod overwrites it
    int kind = InstList[(unsigned int)(njRandom() * ((double)nb - 0.0001f))];
    AL_SetBehavior(tp, ALBHV_DanceFunc_CWE[kind]);
    
	ALOField_Load(tp, CI_KIND_AL_DANCE, &center, 20.0f, (1800 + (int)(njRandom() * 2401.f)));
	return TRUE;
}

void AL_CalcIntentionScore_JoinDance(ObjectMaster* tp, float* pMaxScore) {
    Uint32 trigger = ChaoGlobal.IntentionHimaTrigger;
    float score = 0;
    Uint32 value = AL_EmotionGetValue(tp, EM_ST_TEDIOUS);
    ObjectMaster* pField = ALO_Field_Find_(tp, 1, CI_KIND_AL_DANCE);

    if (*pMaxScore < 1) {
        if (pField) {
            if (value > trigger) {
                if (value < 7000) {
                    value += 3000;
                }

                score = ChaoGlobal.IntentionHimaMaxScore * AL_CalcScoreTypeA(value, trigger);
                AL_ScoreRandomize(&score);
            }
            if (score > *pMaxScore) {
                int kind = -1;
                if (pField->Parent) {
                    BHV_FUNC func = AL_GetBehavior(pField->Parent);

                    for (size_t i = 0; i < NB_AL_DANCE_CWE; i++) {
                        if (func == ALBHV_DanceFunc_CWE[i]) {
                            if (AL_KW_IsDanceFlagOn(tp, i)) {
                                kind = i; 
                            }

                            break;
                        }
                    }
                }

                //vanilla fix: the original check was kind > 0 so it didn't run for shake dance
                if (kind >= 0) {
                    MOV_SetAimPos(tp, &pField->Data1.Entity->Position);

                    AL_SetBehavior(tp, ALBHV_TurnToAim);

                    if (kind < NB_AL_DANCE_CWE) {
                        AL_SetNextBehavior(tp, ALBHV_DanceFunc_CWE[kind]);
                    }

                    AL_EmotionAdd(tp, EM_ST_TEDIOUS, -4000);
                    AL_EmotionAdd(tp, EM_ST_LONELY, -4000);
                    
                    if (pField->Parent) {
                        AL_EmotionAdd(pField->Parent, EM_ST_LONELY, -3000);
                    }
                }
                else {
                    if (pField->Parent) {
                        MOV_SetAimPos(tp, &pField->Parent->Data1.Entity->Position);
                    }

                    AL_SetBehavior(tp, ALBHV_TurnToAim);
                    AL_SetNextBehavior(tp, (BHV_FUNC)0x0055C430);

                    if (njRandom() < 0.5f) {
                        AL_SetNextBehavior(tp, (BHV_FUNC)0x0059CDA0);
                    }
                    else {
                        AL_SetNextBehavior(tp, (BHV_FUNC)0x0059CF50);
                    }
                }
                *pMaxScore = 0.99f;
            }
        }
    }
}