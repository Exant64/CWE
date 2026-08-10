#pragma once
#include "stdafx.h"

enum {
    XYZS_ASS_XL = 0x0,
    XYZS_ASS_XU = 0x1,
    XYZS_ASS_YL = 0x2,
    XYZS_ASS_YU = 0x3,
    XYZS_ASS_ZL = 0x4,
    XYZS_ASS_ZU = 0x5,
    XYZS_ASS_NMB = 0x6,
};

#define GET_MOVE_WORK(tp) ((MOVE_WORK*)tp->mwp)
ThiscallFunctionPointer(float, MOV_DistFromAim, (task* a1), 0x007968A0);

int MOV_TurnToPlayer2(task *a1, int a3, int a2);
float MOV_Dist2FromPlayer(task* tp, int player_num);
void MOV_Control(task* eax0);
void sub_54B230(task* eax0, float a2);
void ObjectMovableInitialize(taskwk* a1, int a2);
void MoveFunc2(task* a1);

float MOV_DistFromAimXZ(task* tp);
int MOV_TurnToAim2(task* tp, int ang);
void MOV_SetVelo(task* tp, NJS_VECTOR* pVelo);
void MOV_SetAimPos(task* tp, NJS_POINT3* pPos);
