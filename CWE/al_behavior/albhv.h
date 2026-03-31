#pragma once
#include "../al_world.h"
#include "albhv_music.h"

int ALBHV_PostureChangeStand(task* tp);
int ALBHV_PostureChangeSit(task* tp);

int ALBHV_GoToAimTree(task* tp);

void ALBHV_GoToConsole(ObjectMaster* a1);
int __cdecl ALBHV_Accordion(ObjectMaster* a1);
int __cdecl ALBHV_Garagara(ObjectMaster* a1);
int __cdecl ALBHV_BuildStart(ObjectMaster* a1);
signed int __cdecl ALBHV_GoToPiano(ObjectMaster* a1);
signed int __cdecl ALBHV_SandHole(ObjectMaster* a1);
signed int __cdecl AL_DecideBehaviorDance(ObjectMaster* a1);
signed int __cdecl AL_DecideBehaviorSToy(ObjectMaster* a1); 
void __cdecl AL_CalcIntentionScore_JoinDance(ObjectMaster* a1, float* a2);
void __cdecl AL_CalcIntentionScore_JoinSToy(ObjectMaster* a1, float* a2);
void AL_CalcIntentionScore_Mayu(ObjectMaster* tp, float* pMaxScore);
signed int __cdecl ALBHV_GoToFloat(ObjectMaster* a1);
signed int __cdecl ALBHV_GoToBoat(ObjectMaster* a1);
void __cdecl  AL_CalcIntentionScore_LToy(ObjectMaster* a1, float* a2);
signed int ALBHV_RideBoat(ObjectMaster* a1);
signed int ALBHV_RideFloat(ObjectMaster* a1);
signed int __cdecl ALBHV_GoToHorse(ObjectMaster* a1);
int ALBHV_Capture(ObjectMaster* tp);

#define ALBHV_Think ((BHV_FUNC)0x00565BE0)
#define ALBHV_GoToAim ((BHV_FUNC)0x56B560)
#define ALBHV_TurnToAim ((BHV_FUNC)0x56B500)
#define ALBHV_TurnToLockOn ((BHV_FUNC)0x56B6C0)
#define ALBHV_GoToLockOn ((BHV_FUNC)0x56BA80)
#define ALBHV_PickUpLockOn ((BHV_FUNC)0x5613C0)
#define ALBHV_Cry ((BHV_FUNC)0x59FCA0)
#define ALBHV_HoldThink ((BHV_FUNC)0x569340)
#define ALBHV_Notice ((BHV_FUNC)0x56B480)
#define ALBHV_Touch ((BHV_FUNC)0x56BBA0)
#define ALBHV_ListenMusic ((BHV_FUNC)0x0059E710)

#define ALBHV_RideHorse ((BHV_FUNC)0x598F40)
#define ALBHV_WatchTV ((BHV_FUNC)0x598890)
#define ALBHV_ListenRadicase ((BHV_FUNC)0x598B90)

template <BHV_FUNC bhv>
int __cdecl ALBHV_ToyMoveCheck(ObjectMaster* a1)
{
	if (ALW_LockOnPickedUp(a1))
		return 3;
	return bhv(a1);
}