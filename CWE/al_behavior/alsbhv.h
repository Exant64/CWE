#pragma once

#include "SA2Functions.h"
signed int __cdecl ALBHV_GoNextToSocial(task* a1);
int __cdecl ALBHV_WaitForSocialArrive(task* a1);
int __cdecl ALBHV_StepBack(task* a1);
int ALBHV_LockUp(task* a1);
signed int __cdecl ALBHV_GoToSocial(task* a1);
int ALBHV_Talk(task* a1);
int ALBHV_InitHoldHands(task* a1);
void ALBHV_Gossip(task* a1, task* a2);
int ALBHV_InitHug(task* a1);

template <BHV_FUNC bhv>
static int ALBHV_SocialCheck(task* tp) {
	if (ALW_RecieveCommand(tp) == ALW_CMD_CHANGE) {
		___OutputDebugString("TEST TEST TEST TEST");
		return BHV_RET_BREAK;
	}
	return bhv(tp);
}

template <uint32_t bhv>
static int ALBHV_SocialCheck(task* tp) {
	if (ALW_RecieveCommand(tp) == ALW_CMD_CHANGE) {
		___OutputDebugString("TEST TEST TEST TEST");
		return BHV_RET_BREAK;
	}
	return BHV_FUNC(bhv)(tp);
}