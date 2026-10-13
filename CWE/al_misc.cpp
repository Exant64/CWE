#include "stdafx.h"
#include <ninja_functions.h>
#include "Chao.h"
#include "asmutil.h"

void __cdecl AL_ForwardAcc(task* a1, float a2)
{
	chaowk* v2; // esi
	MOVE_WORK* v3; // edi

	v2 = GET_CHAOWK(a1);
	v3 = GET_MOVE_WORK(a1);
	v3->Acc.x = njSin(v2->ang.y) * a2;
	v3->Acc.z = njCos(v2->ang.y) * a2;
}
void __cdecl AL_ForwardSpd(task* a1, float a2)
{
	chaowk* v2; // esi
	float v3; // st7
	Angle v4; // ecx

	v2 = GET_CHAOWK(a1);
	v3 = njSin(v2->ang.y);
	v4 = v2->ang.y;
	v2->pos.x = v3 * a2 + v2->pos.x;
	v2->pos.z = njCos(v4) * a2 + v2->pos.z;
}
void __cdecl AL_Brake(task* a1, float a2)
{
	MOVE_WORK* v2; // eax

	v2 = GET_MOVE_WORK(a1);
	if (v2->Flag & 4)
	{
		v2->Velo.x *= a2;
		v2->Velo.y *= a2;
		v2->Velo.z *= a2;
	}
}

ASM_FUNC Bool AL_IsOnScreen3(task *tp, float radius, float HalfHeight, float OffsetY) {
	// arguments
    ASM_PUSH(      ASM_ESP(4+0+0) ); // OffsetY
    ASM_PUSH(      ASM_ESP(3+1+0) ); // HalfHeight
    ASM_PUSH(      ASM_ESP(2+2+0) ); // radius
    ASM_MOVE( eax, ASM_ESP(1+3+0) ); // tp

    // call
    ASM_CALL_R( edx, 0x0054AC70 );

    // end arguments
    ASM_ESP_ADD( 3 );

    // return
    ASM_RET( 0 );
}

Bool AL_IsOnScreen2(task *tp, float radius, float OffsetY) {
	return AL_IsOnScreen3(tp, radius, radius, OffsetY);
}