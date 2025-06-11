#pragma once
#include "stdafx.h"

ThiscallFunctionPointer(float, MOV_DistFromAim, (ObjectMaster* a1), 0x007968A0);

float MOV_DistFromAimXZ(task* tp);
int MOV_TurnToAim2(task* tp, int ang);
void MOV_SetVelo(task* tp, NJS_VECTOR* pVelo);
void MOV_SetAimPos(task* tp, NJS_POINT3* pPos);
