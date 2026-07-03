#include "stdafx.h"

#include "Chao.h"
#include "albhv.h"

#include <memory.h>
#include "al_behavior.h"

enum {
    HIT_TYPE_PUNCH,
    HIT_TYPE_GENKOTU,
    HIT_TYPE_KICK,
    NB_HIT_TYPE
};

// this task is a hack to make sure every chao stops once any of them get interrupted
// doing a 3 way "a - > b -> c -> a" type setup with ALW_AttentionOn doesn't work because
// AL_SetBehavior only sets ALW_CMD_CHANGE if the tasks are communicating with eachother
// to work around this, we allocate 3 dummy alw_entries in this task, create "fake" communication with them
// and if any of them signal ALW_CMD_CHANGE, send it to everyone and destroy itself

static void BullyStopperScenario (task* tp) {
	al_entry_work* pEntry = (al_entry_work*)tp->UnknownA_ptr;

	for(size_t i = 0; i < 3; i++) {
		if(pEntry[i].command != ALW_CMD_CHANGE) {
			continue;
		}

		// if we recieved change command, broadcast to all
		// then clear commu to prevent dead pointer
		for(size_t j = 0; j < 3; j++) {
			if(!pEntry[j].pCommu) { 
				continue;
			}

			pEntry[j].pCommu->command = ALW_CMD_CHANGE;
			pEntry[j].pCommu->pCommu = nullptr;
		}

		DeleteObject_(tp);
		break;
	}
}

static void CreateBullyStopperScenario(task* pBully, task* pVictim, task* pStopper) {
	task* tp = CreateElementalTask((LoadObj)0, 4, BullyStopperScenario, "BullyStopManager");

	// create 3 fake entries that each chao will communicate with
	al_entry_work* pEntry = ALLOC_ARRAY(3, al_entry_work);
	tp->UnknownA_ptr = (ObjUnknownA*)pEntry;

	for(size_t i = 0; i < 3; i++) {
		ALW_ResetEntry(&pEntry[i]);
	}

	pEntry[0].pCommu = (al_entry_work*)pBully->UnknownA_ptr;
	pEntry[1].pCommu = (al_entry_work*)pVictim->UnknownA_ptr;
	pEntry[2].pCommu = (al_entry_work*)pStopper->UnknownA_ptr;

	for(size_t i = 0; i < 3; i++) {
		pEntry[i].pCommu->pCommu = &pEntry[i];

		// this is needed so that any "stuck" commands get reset
		// to prevent the behaviors from instantly ending
		pEntry[i].pCommu->command = 0;
	}
}

static int ALBHV_BullyPostureChangeStand(task* tp) {
	if (ALW_RecieveCommand(tp) == ALW_CMD_CHANGE) {
		return BHV_RET_BREAK;
	}

	return ALBHV_PostureChangeStand(tp);
}

static int ALBHV_GetHit(task* tp);

static int ALBHV_HitChao(task* tp) {
    AL_BEHAVIOR* bhv = &GET_CHAOWK(tp)->Behavior;

	task* pLockOn = ALW_GetLockOnTask(tp);

	if(!pLockOn) {
		return BHV_RET_BREAK;
	}

    switch(bhv->Mode) {
        case 0:
			AL_SetMotionLink(tp, 100);
			bhv->Mode++;
			[[fallthrough]];
		case 1:
			if (ALW_TurnToLockOn(tp, 384) < 384) {
				bhv->Mode++;
			}
			break;

		case 2: {
            const int dmgType = int(njRandom() * NB_HIT_TYPE);

            AL_FaceChangeEye(tp, ChaoEyes_Mean);

            switch(dmgType) {
                default:
                case HIT_TYPE_PUNCH:
                    AL_SetMotionLink(tp, ALM_PUNCH);
                    break;

                case HIT_TYPE_GENKOTU:
                    AL_SetMotionLink(tp, ALM_GENKOTU);
                    break;

                case HIT_TYPE_KICK:
                    AL_SetMotionLink(tp, ALM_KICK);
                    break;
            }

			PlaySound_XYZ(0x601E, &GET_CHAOWK(tp)->entity.pos, 0, 0, 110);

			// this may feel hacky, but I decided it's the best way to sync up the sequence
			// if you think about it, it's the same thing as a behaviorinterrupt
			AL_SetBehaviorWithFreeWork(pLockOn, ALBHV_GetHit, dmgType);

            bhv->Mode++;
		}
		[[fallthrough]];
        case 3:
			if(AL_IsMotionEnd(tp)) {
				return BHV_RET_FINISH;
			}
            break;
    }

    return BHV_RET_CONTINUE;
}

static int ALBHV_WaitForCommuGetHit(task* tp) {
	if (ALW_RecieveCommand(tp) == ALW_CMD_CHANGE) {
		return BHV_RET_BREAK;
	}

	AL_BEHAVIOR* bhv = &GET_CHAOWK(tp)->Behavior;

    switch(bhv->Mode) {
        case 0:
			AL_SetMotionLink(tp, 100);
			AL_FaceChangeEye(tp, ChaoEyes_Normal);
			AL_FaceChangeMouth(tp, ChaoMouth_None);

			bhv->Timer = 75;
			bhv->Mode++;
			break;
		case 1:
			AL_ForwardSpd(tp, -0.015f);

			if (!--bhv->Timer) {
				// this is necessary to prevent this chao from ending the behavior sequence for the rest of the chao
				ALW_CommunicationOff(tp);

				return BHV_RET_FINISH;
			}
	}

	return BHV_RET_CONTINUE;
}

static int ALBHV_WaitForStopper(task* tp) {
	if (ALW_RecieveCommand(tp) == ALW_CMD_CHANGE) {
		return BHV_RET_BREAK;
	}
	
	AL_BEHAVIOR* bhv = &GET_CHAOWK(tp)->Behavior;

    switch(bhv->Mode) {
        case 0:
			ALW_LockOn(tp, (task*)AL_BehaviorGetUserData(tp));

			AL_IconSet(tp, 1, 45);
			AL_FaceChangeEye(tp, ChaoEyes_Tiny);
			AL_SetMotionLink(tp, 100);

			bhv->Mode++;
			[[fallthrough]];
		case 1: {
			if (ALW_TurnToLockOn(tp, 384) < 384) {
				AL_FaceChangeEye(tp, ChaoEyes_Painful);

				if(njRandom() < 0.7f) {
					PlaySound_XYZ(0x602D, &GET_CHAOWK(tp)->entity.pos, 0, 0, 110);
				}

				if (njRandom() < 0.5f) {
					AL_SetMotionLink(tp, ALM_BURUBURU);
				} else {
					AL_SetMotionLink(tp, ALM_BURUBURU_AGO);
				}

				bhv->Mode++;
			}
		} break;

		case 2:
			if(!--bhv->LimitTimer) {
				return BHV_RET_BREAK;
			}
			break;
    }

	return BHV_RET_CONTINUE;
}

static int ALBHV_GetHit(task* tp) {
    AL_BEHAVIOR* bhv = &GET_CHAOWK(tp)->Behavior;

    switch(bhv->Mode) {
        case 0:
            AL_FaceChangeEye(tp, ChaoEyes_Painful);
			AL_FaceChangeMouth(tp, ChaoMouth_None);

            switch(bhv->FreeWork) {
                default:
                case HIT_TYPE_PUNCH:
                    AL_SetMotionLink(tp, ALM_PUNCH_YARARE);
                    break;

                case HIT_TYPE_GENKOTU:
                    AL_SetMotionLink(tp, ALM_GENKOTU_YARARE);
                    break;

                case HIT_TYPE_KICK:
                    AL_SetMotionLink(tp, ALM_KICK_YARARE);
                    break;
            }

			
            if(njRandom() < 0.5f) {
				PlaySound_XYZ(24617, &GET_CHAOWK(tp)->entity.pos, 0, 0, 110);
            }
            else {
				PlaySound_XYZ(24685, &GET_CHAOWK(tp)->entity.pos, 0, 0, 110);
            }

            bhv->Mode++;
            [[fallthrough]];
        case 1:
            if(AL_IsMotionEnd(tp)) {
                Uint32 emotion;

                emotion = (Uint32)(njRandom() * 100.f + 50.f);
                AL_EmotionAdd(tp, EM_MD_ANGER, emotion);

                emotion = (Uint32)(njRandom() * 100.f + 50.f);
                AL_EmotionAdd(tp, EM_MD_SORROW, emotion);

                emotion = (Uint32)(njRandom() * 100.f + 50.f);
                AL_EmotionAdd(tp, EM_MD_FEAR, emotion);

                emotion = (Uint32)(njRandom() * 100.f + 50.f);
                AL_EmotionAdd(tp, EM_MD_SURPRISE, emotion);

                emotion = (Uint32)(njRandom() * 100.f + 50.f);
                AL_EmotionAdd(tp, EM_MD_PAIN, emotion);
                
				bhv->Timer = 0;
				bhv->Mode++;
            }
            break;

		case 2:
			if(++bhv->Timer > 70) {
				return BHV_RET_FINISH;
			}

			break;
    }

    return BHV_RET_CONTINUE;
}

static int ALBHV_SetGetHitLockOn(task* tp) {
	task* pLockOn = ALW_GetLockOnTask(tp);

	if (!pLockOn || ALW_RecieveCommand(tp) == ALW_CMD_CHANGE) {
		return BHV_RET_BREAK;
	}
	
	AL_SetBehavior(pLockOn, ALBHV_GetHit);

	return BHV_RET_FINISH;
}

static const BHV_FUNC ALBHV_BullyAllowed[] = {
	(BHV_FUNC)0x5A3580, (BHV_FUNC)0x5A3460, (BHV_FUNC)0x5A3380, (BHV_FUNC)0x5A3280, //walking bhavs
	(BHV_FUNC)0x05A1BA0 //resting
};

static int ALBHV_AfraidWait(task* tp) {
	enum {
		AFRAIDWAIT_MD_PST_STAND,
		AFRAIDWAIT_MD_START_TURN,
		AFRAIDWAIT_TURN,
		AFRAIDWAIT_FEAR,
		AFRAIDWAIT_FEAR_STEPBACK_INIT,
		AFRAIDWAIT_FEAR_STEPBACK,
		AFRAIDWAIT_WAIT
	};

	if (ALW_RecieveCommand(tp) == ALW_CMD_CHANGE) {
		return BHV_RET_BREAK;
	}

	chaowk* work = GET_CHAOWK(tp);
	AL_BEHAVIOR* bhv = &work->Behavior;

	switch (bhv->Mode) {
		case AFRAIDWAIT_MD_PST_STAND:
			AL_FaceChangeEye(tp, ChaoEyes_Painful);
			AL_FaceChangeMouth(tp, ChaoMouth_ClosedFrown);

			AL_IconSet(tp, 1, 45);

			switch (AL_GetMotionPosture(tp)) {
				case AL_PST_SIT:
					AL_SetMotionLinkStep(tp, ALM_GENKI_TATI, 40);
					break;
				case AL_PST_LIE:
					AL_SetMotionLinkStep(tp, ALM_JUMP_UP, 40);
					break;
				case AL_PST_FUSE:
					AL_SetMotionLinkStep(tp, ALM_KOKEOKI, 35);
					break;
				case AL_PST_STAND:
				default:
					break;
			}

			bhv->LimitTimer = 1800;
			bhv->Mode = AFRAIDWAIT_MD_START_TURN;
		case AFRAIDWAIT_MD_START_TURN:
			if (!AL_IsMotionEnd(tp)) break;

			AL_SetMotionLink(tp, 100);
			bhv->Mode = AFRAIDWAIT_TURN;
			[[fallthrough]];
		case AFRAIDWAIT_TURN: 
			if (ALW_TurnToLockOn(tp, 384) < 384) {
				if(njRandom() < 0.7f) {
					PlaySound_XYZ(0x602D, &GET_CHAOWK(tp)->entity.pos, 0, 0, 110);
				}

				if (njRandom() < 0.5f) {
					bhv->Mode = AFRAIDWAIT_FEAR;
				} else {
					bhv->Mode = AFRAIDWAIT_FEAR_STEPBACK_INIT;
				}
			}

			break;

		case AFRAIDWAIT_FEAR:
			if (njRandom() < 0.5f) {
				AL_SetMotionLink(tp, ALM_BURUBURU);
			} else {
				AL_SetMotionLink(tp, ALM_BURUBURU_AGO);
			}

			bhv->Mode = AFRAIDWAIT_WAIT;
			break;

		case AFRAIDWAIT_FEAR_STEPBACK_INIT:
			if (njRandom() < 0.5f) {
				AL_SetMotionLink(tp, ALM_ATOZUSARI_BURUBURU);
			} else {
				AL_SetMotionLink(tp, ALM_ATOZUSARI);
			}

			bhv->Mode = AFRAIDWAIT_FEAR_STEPBACK;
			bhv->Timer = 120;
			[[fallthrough]];
		case AFRAIDWAIT_FEAR_STEPBACK:
			if(!--bhv->Timer) {
				bhv->Mode = AFRAIDWAIT_FEAR;
				break;
			}
			
			AL_ForwardAcc(tp, -0.015f);
			break;

		case AFRAIDWAIT_WAIT:
			break;
	}

	if (--bhv->LimitTimer <= 0) {
        AL_SetBehavior(tp, ALBHV_Think);
    }

	return BHV_RET_CONTINUE;
}

int ALBHV_ConfidentWait(task* tp) {
	if (ALW_RecieveCommand(tp) == ALW_CMD_CHANGE) {
		return BHV_RET_BREAK;
	}

	chaowk* work = GET_CHAOWK(tp);
	AL_BEHAVIOR* bhv = &work->Behavior;

	const float maxDist = 30.f;

	switch (bhv->Mode) {
	case 0:
		AL_FaceChangeEye(tp, ChaoEyes_ClosedStraight);
		AL_FaceChangeMouth(tp, ChaoMouth_None);

		AL_IconSet(tp, 1, 45);

		bhv->Mode++;

		break;
	case 1:
		switch (AL_GetMotionPosture(tp)) {
		case AL_PST_SIT:
			AL_SetMotionLinkStep(tp, ALM_GENKI_TATI, 40);
			break;
		case AL_PST_LIE:
			AL_SetMotionLinkStep(tp, ALM_JUMP_UP, 40);
			break;
		case AL_PST_FUSE:
			AL_SetMotionLinkStep(tp, ALM_KOKEOKI, 35);
			break;
		case AL_PST_STAND:
		default:
			break;
		}
		bhv->Mode++;
		break;
	case 2:
		if (!AL_IsMotionEnd(tp)) break;

		AL_SetMotionLink(tp, 100);
		bhv->Mode++;
		[[fallthrough]];
	case 3:
		int ang = ALW_TurnToLockOn(tp, 384);
		if (abs(ang) < 384) {
			AL_SetMotionLink(tp, ALM_K_FIGHTING_TAIKI);

			bhv->Mode++;
		}

		break;
	}

	const float dist = ALW_CalcDistFromLockOn(tp);
	if (dist >= 0 && dist < 4) {
		return BHV_RET_FINISH;
	}

	return BHV_RET_CONTINUE;
}

static bool IsChaoAttackCapable(task* tp) {
	
}

static task* FindChaoToBully(task* tp) {
    chaowk* work = GET_CHAOWK(tp);
    task* pSelectedChao = NULL;

	for (size_t i = 0; i < ALW_CountEntry(0) && !pSelectedChao; i++) {
		task* pChao = GetChaoObject(0, i);
		if (!pChao || pChao == tp) continue;
		
		BHV_FUNC func = AL_GetBehavior(pChao);

		//not in water 
		if (pChao->twp->pos.y + 2.0 < pChao->EntityData2->field_DC) continue;

		//go through allowed behaviors
		for (size_t j = 0; j < LengthOfArray(ALBHV_BullyAllowed); j++) {
			if (func == ALBHV_BullyAllowed[j]) {
				float dist = CheckDistance(&work->entity.pos, &pChao->twp->pos);

				if (dist < 100.f) {
					pSelectedChao = pChao;
                }
                
				break;
			}
		}
	}

	return pSelectedChao;
}

static task* FindStopperChao(task* pBullyChao, task* pVictimChao) {
    task* pSelectedChao = NULL;
	float distClosest = 100.f;

	for (size_t i = 0; i < ALW_CountEntry(0) && !pSelectedChao; i++) {
		task* pChao = GetChaoObject(0, i);
		chaowk* work = GET_CHAOWK(pChao);

		if (!pChao || pChao == pBullyChao || pChao == pVictimChao) continue;
		
		if (AL_IsChild(pChao)) continue;
		if (!AL_IsHero2(pChao) && AL_EmotionGetValue(pChao, EM_PER_KINDNESS) < 10) continue;
		
		//not in water 
		if (pChao->twp->pos.y + 2.0 < pChao->EntityData2->field_DC) continue;

		const BHV_FUNC func = AL_GetBehavior(pChao);
		
		//go through allowed behaviors
		for (size_t j = 0; j < LengthOfArray(ALBHV_BullyAllowed); j++) {
			if (func == ALBHV_BullyAllowed[j]) {
				const float dist = CheckDistance(&GET_CHAOWK(pBullyChao)->entity.pos, &work->entity.pos);

				if (dist < distClosest) {
					pSelectedChao = pChao;
					distClosest = dist;
                }
                
				break;
			}
		}
	}

	return pSelectedChao;
}

static int ALBHV_StopperGoToLockOn(task* tp) {
	if (ALW_RecieveCommand(tp) == ALW_CMD_CHANGE) {
		return BHV_RET_BREAK;
	}

	return ALBHV_GoToLockOn(tp);
}

static int ALBHV_AngryGoToLockOn(task* tp) {
    enum { MD_GTLO_WALK, MD_GTLO_WALK2, MD_GTLO_ARRIVE };

	if (ALW_RecieveCommand(tp) == ALW_CMD_CHANGE) {
		return BHV_RET_BREAK;
	}

    chaowk* work = GET_CHAOWK(tp);
    AL_BEHAVIOR* bhv = &work->Behavior;
    float dist;

    switch (bhv->Mode) {
        case MD_GTLO_WALK:
            AL_FaceChangeEye(tp, ChaoEyes_Mean);

            bhv->LimitTimer = 1800;

            if (bhv->Timer++ > 30) {
                AL_SetMotionLink(tp, ALM_ANGER_WALK);
                ++bhv->Mode;
            }
            break;
        case MD_GTLO_WALK2:
            ALW_TurnToLockOn(tp, 384);
            AL_ForwardAcc(tp, GET_GLOBAL()->WalkAcc * 0.8f);

            dist = ALW_CalcDistFromLockOn(tp);
            if (dist < 0.0) {
                AL_SetBehavior(tp, ALBHV_Think);
            } else if (dist < 4.0) {
                return BHV_RET_FINISH;
            }
    }

    AL_SetMotionSpd(tp, 1.5);

    if (--bhv->LimitTimer <= 0) {
        AL_SetBehavior(tp, ALBHV_Think);
    }

    return BHV_RET_CONTINUE;
}

static Uint32 IntentionAngerBullyTrigger = 80;
static float IntentionAngerBullyMinScore = 0.7f;
static float IntentionAngerBullyMaxScore = 0.99f;

void AL_CalcIntentionScore_Bully(task* tp, float* pMaxScore) {
	float score = 0.0f;
    Uint32 angerTrigger = IntentionAngerBullyTrigger;
    Uint32 value = AL_EmotionGetValue(tp, EM_MD_ANGER);

    if (AL_EmotionGetValue(tp, EM_PER_KINDNESS) > 60) {
        return;
	}

	task* pVictimChao = FindChaoToBully(tp);
	if (!pVictimChao) return;

	bool trigger = false;

	if (value > angerTrigger) {
		if (AL_EmotionGetValue(tp, EM_PER_AGRESSIVE) > 0 || njRandom() < 0.5f) {
			score = AL_CalcMoodScoreTypeA(value, angerTrigger);
			score += (100 + AL_EmotionGetValue(tp, EM_PER_AGRESSIVE)) / 200.f * (1.f - IntentionAngerBullyMinScore);
			score *= IntentionAngerBullyMaxScore;
			AL_ScoreRandomize(&score);
		}

		if(score > *pMaxScore) {
			trigger = true;

			*pMaxScore = score;
		}
	}

	if(!trigger) return;

    task* pStopperChao = NULL;
	if (njRandom() < 0.4f) {
		pStopperChao = FindStopperChao(tp, pVictimChao);
	}

    ALW_LockOn(tp, pVictimChao);
    ALW_LockOn(pVictimChao, tp);

	const int angerDecrease = max(0, AL_EmotionGetValue(tp, EM_PER_AGRESSIVE)) - 100;
	AL_EmotionAdd(tp, EM_MD_ANGER, angerDecrease);

	const bool canDefendThemselves = (GET_CHAOPARAM(pVictimChao)->Skill[3] - GET_CHAOPARAM(tp)->Skill[3]) > 500 || 
		(GET_CHAOPARAM(pVictimChao)->KarateInfo - GET_CHAOPARAM(tp)->KarateInfo) >= 5;

	if (!canDefendThemselves && pStopperChao) {
		// warning: not the prettiest, no existing systems were really appropriate for this
		// but i didn't wanna invent a new one just for this one thing

		AL_SetBehavior(pStopperChao, ALBHV_BullyPostureChangeStand);
    	AL_SetNextBehavior(pStopperChao, ALBHV_StopperGoToLockOn);
    	AL_SetNextBehavior(pStopperChao, ALBHV_HitChao);

		AL_SetBehavior(tp, ALBHV_BullyPostureChangeStand);
		AL_SetNextBehavior(tp, ALBHV_AngryGoToLockOn);
		AL_SetNextBehaviorWithUserData(tp, ALBHV_WaitForStopper, pStopperChao);

		AL_SetBehavior(pVictimChao, ALBHV_AfraidWait);
		AL_SetNextBehavior(pVictimChao, ALBHV_WaitForCommuGetHit);

		ALW_LockOn(pStopperChao, tp);

		CreateBullyStopperScenario(tp, pVictimChao, pStopperChao);

		return;
	}

	if(canDefendThemselves) {
		AL_SetBehavior(tp, ALBHV_BullyPostureChangeStand);
		AL_SetNextBehavior(tp, ALBHV_AngryGoToLockOn);

		AL_SetBehavior(pVictimChao, ALBHV_ConfidentWait);
		AL_SetNextBehavior(pVictimChao, ALBHV_HitChao);

		ALW_CommunicationOn(tp, pVictimChao);
		ALW_SendCommand(tp, 0);
		ALW_SendCommand(pVictimChao, 0);

		ALW_LockOn(tp, pVictimChao);
    	ALW_LockOn(pVictimChao, tp);

		return;
	}

    AL_SetBehavior(tp, ALBHV_PostureChangeStand);
    AL_SetNextBehavior(tp, ALBHV_AngryGoToLockOn);
    AL_SetNextBehavior(tp, ALBHV_HitChao);
	
    AL_SetBehavior(pVictimChao, ALBHV_AfraidWait);

    ALW_CommunicationOn(tp, pVictimChao);
	ALW_SendCommand(tp, 0);
	ALW_SendCommand(pVictimChao, 0);

	ALW_LockOn(tp, pVictimChao);
    ALW_LockOn(pVictimChao, tp);
}