#pragma once
#include "../al_world.h"
#include "albhv_music.h"

int ALBHV_PostureChangeStand(task* tp);
int ALBHV_PostureChangeSit(task* tp);

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

#define BHVFunc(NAME, ADDRESS) FunctionPointer(int,NAME,(ObjectMaster*),ADDRESS);
BHVFunc(ALBHV_Think, 0x00565BE0);
BHVFunc(ALBHV_GoToAim, 0x56B560);
BHVFunc(ALBHV_TurnToAim, 0x56B500);
BHVFunc(ALBHV_TurnToLockOn, 0x56B6C0);
BHVFunc(ALBHV_GoToLockOn, 0x56BA80);
BHVFunc(ALBHV_Cry, 0x59FCA0);
BHVFunc(ALBHV_HoldThink, 0x569340);

template <BHV_FUNC bhv>
int __cdecl ALBHV_ToyMoveCheck(ObjectMaster* a1)
{
	if (ALW_LockOnPickedUp(a1))
		return 3;
	return bhv(a1);
}