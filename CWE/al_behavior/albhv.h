#pragma once
#include "../al_world.h"
#include "albhv_music.h"
#include "albhv_bully.h"

int ALBHV_PostureChangeStand(task* tp);
int ALBHV_PostureChangeSit(task* tp);

int ALBHV_GoToAimTree(task* tp);

void ALBHV_GoToConsole(task* a1);
int __cdecl ALBHV_Accordion(task* a1);
int __cdecl ALBHV_Garagara(task* a1);
int __cdecl ALBHV_BuildStart(task* a1);
signed int __cdecl ALBHV_GoToPiano(task* a1);
signed int __cdecl ALBHV_SandHole(task* a1);
signed int __cdecl AL_DecideBehaviorDance(task* a1);
signed int __cdecl AL_DecideBehaviorSToy(task* a1); 
void __cdecl AL_CalcIntentionScore_JoinDance(task* a1, float* a2);
void __cdecl AL_CalcIntentionScore_JoinSToy(task* a1, float* a2);
void AL_CalcIntentionScore_Mayu(task* tp, float* pMaxScore);
signed int __cdecl ALBHV_GoToFloat(task* a1);
signed int __cdecl ALBHV_GoToBoat(task* a1);
void __cdecl  AL_CalcIntentionScore_LToy(task* a1, float* a2);
signed int ALBHV_RideBoat(task* a1);
signed int ALBHV_RideFloat(task* a1);
signed int __cdecl ALBHV_GoToHorse(task* a1);
int ALBHV_Capture(task* tp);

#define ALBHV_Think ((BHV_FUNC)0x00565BE0)
#define ALBHV_GoToAim ((BHV_FUNC)0x56B560)
#define ALBHV_TurnToAim ((BHV_FUNC)0x56B500)
#define ALBHV_TurnToLockOn ((BHV_FUNC)0x56B6C0)
#define ALBHV_GoToLockOn ((BHV_FUNC)0x56BA80)
#define ALBHV_Cry ((BHV_FUNC)0x59FCA0)
#define ALBHV_HoldThink ((BHV_FUNC)0x569340)
#define ALBHV_Notice ((BHV_FUNC)0x56B480)
#define ALBHV_Touch ((BHV_FUNC)0x56BBA0)
#define ALBHV_ListenMusic ((BHV_FUNC)0x0059E710)

#define ALBHV_RideHorse ((BHV_FUNC)0x598F40)
#define ALBHV_WatchTV ((BHV_FUNC)0x598890)
#define ALBHV_ListenRadicase ((BHV_FUNC)0x598B90)

template <BHV_FUNC bhv>
int __cdecl ALBHV_ToyMoveCheck(task* a1)
{
	if (ALW_LockOnPickedUp(a1))
		return 3;
	return bhv(a1);
}