#include "stdafx.h"
#include "../SA2ModLoader.h"
#include "../ninja_functions.h"
#include "../ALifeSDK_Functions.h"
#include "../Chao.h"
#include <random>
#include "../al_world.h"
#include "al_intention.h"
#include "albhv.h"
#include "albhv_navigation.h"
#include <al_landmark.h>

DataPointer(task*, pRadicaseTask, 0x01AED2E0);
DataPointer(task*, pTVTask, 0x1AED288);

Bool ALO_GetRadicaseListenPos(NJS_POINT3* pPos) {
    if (!pRadicaseTask)
        return FALSE;
    else {
        EntityData1* work = pRadicaseTask->Data1.Entity;
        float mul = 10 + njRandom() * 3;
        int ang = work->Rotation.y + 0x8000 + NJM_DEG_ANG(njRandom() * 80 - 40);

        pPos->x = njSin(ang) * mul + work->Position.x;
        pPos->y = work->Position.y;
        pPos->z = njCos(ang) * mul + work->Position.z;
    }

    return TRUE;
}

Bool ALO_GetTVWatchPos(NJS_POINT3* pPos) {
    if (!pTVTask)
        return FALSE;
    else {
        EntityData1* work = pRadicaseTask->Data1.Entity;
        float mul = 10 + njRandom() * 3;
        int ang = work->Rotation.y + 0x8000 + NJM_DEG_ANG(njRandom() * 80 - 40);
        pPos->x = njSin(ang) * mul + work->Position.x;
        pPos->y = work->Position.y;
        pPos->z = njCos(ang) * mul + work->Position.z;
    }

    return TRUE;
}

int ALBHV_GoToTV(task* tp) {
	task* pToy = AL_GetFoundToyTask(tp);
    NJS_POINT3 pos;

    if(pToy && ALO_GetTVWatchPos(&pos)) {
        ALW_LockOn(tp, pToy);
        MOV_SetAimPos(tp, &pos);

        AL_SetBehavior(tp, ALBHV_ToyMoveCheck<ALBHV_PostureChangeStand>);
        AL_SetNextBehavior(tp, ALBHV_ToyMoveCheck<ALBHV_Notice>);
        AL_SetNextBehavior(tp, ALBHV_ToyMoveCheck<ALBHV_GoToLockOn>);
        AL_SetNextBehavior(tp, ALBHV_ToyMoveCheck<ALBHV_Touch>);
        AL_SetNextBehavior(tp, ALBHV_ToyMoveCheck<ALBHV_GoToAim>);
        AL_SetNextBehavior(tp, ALBHV_ToyMoveCheck<ALBHV_TurnToLockOn>);
        AL_SetNextBehavior(tp, ALBHV_ToyMoveCheck<ALBHV_WatchTV>);

        return BHV_RET_CONTINUE;
    }

    return BHV_RET_FINISH;
}

int ALBHV_GoToRadicase(task* tp) {
	task* pToy = AL_GetFoundToyTask(tp);
    NJS_POINT3 pos;

    if(pToy && ALO_GetRadicaseListenPos(&pos)) {
		ALW_LockOn(tp, pToy);
		MOV_SetAimPos(tp, &pos);

		AL_SetBehavior(tp, ALBHV_ToyMoveCheck<ALBHV_PostureChangeStand>);
		AL_SetNextBehavior(tp, ALBHV_ToyMoveCheck<ALBHV_Notice>);
		AL_SetNextBehavior(tp, ALBHV_ToyMoveCheck<ALBHV_GoToLockOn>);
		AL_SetNextBehavior(tp, ALBHV_ToyMoveCheck<ALBHV_Touch>);
		AL_SetNextBehavior(tp, ALBHV_ToyMoveCheck<ALBHV_GoToAim>);
		AL_SetNextBehavior(tp, ALBHV_ToyMoveCheck<ALBHV_TurnToLockOn>);
		AL_SetNextBehavior(tp, ALBHV_ToyMoveCheck<ALBHV_ListenRadicase>);

		return BHV_RET_CONTINUE;
	}

	return BHV_RET_FINISH;
}

int ALBHV_GoToHorse(task* tp) {
    task* pToy = AL_GetFoundToyTask(tp);
    NJS_POINT3 pos;

    if(pToy && !ALW_IsSheAttentionOtherOne(tp, pToy)) {
        MOV_SetAimPos(tp, &pToy->Data1.Entity->Position);
        ALW_CommunicationOn(tp, pToy);

        AL_SetBehavior(tp, ALBHV_ToyMoveCheck<ALBHV_PostureChangeStand>);
        AL_SetNextBehavior(tp, ALBHV_ToyMoveCheck<ALBHV_Notice>);
        AL_SetNextBehavior(tp, ALBHV_ToyMoveCheck<ALBHV_GoToAim>);
        AL_SetNextBehavior(tp, ALBHV_ToyMoveCheck<ALBHV_TurnToAim>);
        AL_SetNextBehavior(tp, ALBHV_ToyMoveCheck<ALBHV_RideHorse>);

        return BHV_RET_CONTINUE;
    }

    return BHV_RET_FINISH;
}

extern signed int __cdecl ALBHV_GoToPiano(ObjectMaster* a1);
void AL_CalcIntentionScore_LToy(task* tp, float* pMaxScore) {
	float score = 0;
	Uint32 emotion = AL_EmotionGetValue(tp, EM_ST_TEDIOUS);

	// this is INT_TIMER_LTOY on sa2dc, why?
	if (!GET_CHAOWK(tp)->Behavior.IntentionTimer[INT_TIMER_TOY])
	{
		task* pToy = AL_GetFoundToyTask(tp);
		if (pToy) {
			Uint32 trigger = GET_GLOBAL()->IntentionHimaTrigger;
            
            score = AL_CalcScoreTypeA(emotion, trigger);
            score *= GET_GLOBAL()->IntentionHimaMaxScore;
            AL_ScoreRandomize(&score);

			if(score > *pMaxScore) {
				int kind = ((al_entry_work*)pToy->UnknownA_ptr)->kind;

				switch (kind) {
					case ALW_KIND_TV:
						AL_SetBehavior(tp, ALBHV_GoToTV);
						break;
					case ALW_KIND_RADICASE:
						AL_SetBehavior(tp, ALBHV_GoToRadicase);
						break;
					case ALW_KIND_BALL:
						AL_SetBehavior(tp, (BHV_FUNC)0x40AD80);
						break;
					case ALW_KIND_PIANO:
						AL_SetBehavior(tp, ALBHV_GoToPiano);
						break;
					case ALW_KIND_ORGAN:
						AL_SetBehavior(tp, (BHV_FUNC)0x40AD80);
						break;
					case ALW_KIND_HORSE: {
						int landattr = AL_GetCurrLandAttr(&GET_CHAOWK(tp)->entity.Position);
						
						// really weird check (it's there on all versions)
						// we should probably get rid of it?
						if (landattr != LMA_GROUND1 && landattr != LMA_GROUND2 && landattr == LMA_GROUND3) {
                            return;
                        }
                        
						AL_SetBehavior(tp, ALBHV_GoToHorse);
					} break;
					case ALW_KIND_BOX:
						AL_SetBehavior(tp, (BHV_FUNC)0x599770);
						break;
					case ALW_KIND_BOAT:
						AL_SetBehavior(tp, ALBHV_GoToBoat);
						break;
					case ALW_KIND_UKIWA:
						AL_SetBehavior(tp, ALBHV_GoToFloat);
						break;
					default:
						break;
				}

				AL_EmotionAdd(tp, EM_ST_TEDIOUS, -5000);
				*pMaxScore = score;
			}
		}
	}
}