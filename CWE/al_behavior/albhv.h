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

#define BHVFunc(NAME, ADDRESS) FunctionPointer(int,NAME,(task*),ADDRESS);
BHVFunc(ALBHV_Think, 0x00565BE0);
BHVFunc(ALBHV_GoToAim, 0x56B560);
BHVFunc(ALBHV_TurnToAim, 0x56B500);
BHVFunc(ALBHV_TurnToLockOn, 0x56B6C0);
BHVFunc(ALBHV_GoToLockOn, 0x56BA80);
BHVFunc(ALBHV_Cry, 0x59FCA0);
BHVFunc(ALBHV_HoldThink, 0x569340);

template <BHV_FUNC bhv>
int __cdecl ALBHV_ToyMoveCheck(task* a1)
{
	if (ALW_LockOnPickedUp(a1))
		return 3;
	return bhv(a1);
}