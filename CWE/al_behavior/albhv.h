#pragma once
#include "../al_world.h"
#include "albhv_music.h"
#include "albhv_bully.h"
#include "albhv_fear.h"

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
void AL_CalcIntentionScore_MayuReact(task* tp, float* pMaxScore);
signed int __cdecl ALBHV_GoToFloat(task* a1);
signed int __cdecl ALBHV_GoToBoat(task* a1);
int ALBHV_GoToCoffin(task* tp);
void __cdecl  AL_CalcIntentionScore_LToy(task* a1, float* a2);
signed int ALBHV_RideBoat(task* a1);
signed int ALBHV_RideFloat(task* a1);
signed int __cdecl ALBHV_GoToHorse(task* a1);
int ALBHV_Capture(task* tp);

#define ALBHV_GoToLockOn_p 0x56BA80
#define ALBHV_PickUpLockOn_p 0x56BA80
#define ALBHV_Notice_p 0x56B480
#define ALBHV_Touch_p 0x56BBA0
#define ALBHV_GoToAim_p 0x56B560
#define ALBHV_TurnToAim_p 0x56B500
#define ALBHV_TurnToLockOn_p 0x56B6C0

#define ALBHV_RideHorse_p 0x598F40
#define ALBHV_WatchTV_p 0x598890
#define ALBHV_ListenRadicase_p 0x598B90

#define ALBHV_Move_p 0x562D20

#define ALBHV_Move ((BHV_FUNC)uintptr_t(ALBHV_Move_p))
#define ALBHV_Think ((BHV_FUNC)uintptr_t(0x00565BE0))
#define ALBHV_GoToAim ((BHV_FUNC)uintptr_t(ALBHV_GoToAim_p))
#define ALBHV_TurnToAim ((BHV_FUNC)uintptr_t(ALBHV_TurnToAim_p))
#define ALBHV_TurnToLockOn ((BHV_FUNC)uintptr_t(ALBHV_TurnToLockOn_p))
#define ALBHV_GoToLockOn ((BHV_FUNC)uintptr_t(ALBHV_GoToLockOn_p))
#define ALBHV_PickUpLockOn ((BHV_FUNC)uintptr_t(0x5613C0))
#define ALBHV_Cry ((BHV_FUNC)uintptr_t(0x59FCA0))
#define ALBHV_HoldThink ((BHV_FUNC)uintptr_t(0x569340))
#define ALBHV_Notice ((BHV_FUNC)uintptr_t(ALBHV_Notice_p))
#define ALBHV_Touch ((BHV_FUNC)uintptr_t(ALBHV_Touch_p))
#define ALBHV_ListenMusic ((BHV_FUNC)uintptr_t(0x0059E710))

#define ALBHV_RideHorse ((BHV_FUNC)uintptr_t(ALBHV_RideHorse_p))
#define ALBHV_WatchTV ((BHV_FUNC)uintptr_t(ALBHV_WatchTV_p))
#define ALBHV_ListenRadicase ((BHV_FUNC)uintptr_t(ALBHV_ListenRadicase_p))

#define ALBHV_Koke ((BHV_FUNC)0x562D50)
#define ALBHV_Bikkuri ((BHV_FUNC)0x5647B0)

template <BHV_FUNC bhv>
int __cdecl ALBHV_ToyMoveCheck(task* a1)
{
	if (ALW_LockOnPickedUp(a1))
		return 3;
	return bhv(a1);
}

template <uint32_t bhv>
int __cdecl ALBHV_ToyMoveCheck(task* a1)
{
	if (ALW_LockOnPickedUp(a1))
		return 3;
	return BHV_FUNC(bhv)(a1);
}