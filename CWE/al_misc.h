#pragma once
#include "stdafx.h"

Bool AL_IsOnScreen2(task *tp, float radius, float OffsetY);
Bool AL_IsOnScreen3(task *tp, float radius, float HalfHeight, float OffsetY);

void __cdecl AL_ForwardAcc(task* a1, float a2);
void __cdecl AL_ForwardSpd(task* a1, float a2);
void __cdecl AL_Brake(task* a1, float a2);