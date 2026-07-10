#pragma once
#include "stdafx.h"

#define GET_MOVE_WORK(tp) ((MOVE_WORK*)tp->mwp)
ThiscallFunctionPointer(float, MOV_DistFromAim, (task* a1), 0x007968A0);

float MOV_DistFromAimXZ(task* tp);
int MOV_TurnToAim2(task* tp, int ang);
void MOV_SetVelo(task* tp, NJS_VECTOR* pVelo);
void MOV_SetAimPos(task* tp, NJS_POINT3* pPos);
