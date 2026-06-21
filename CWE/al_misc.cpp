#include "stdafx.h"
#include <ninja_functions.h>
#include "Chao.h"

void __cdecl AL_ForwardAcc(task* a1, float a2)
{
	ChaoData1* v2; // esi
	UnknownData2* v3; // edi

	v2 = GET_CHAOWK(a1);
	v3 = a1->EntityData2;
	v3->speed.x = njSin(v2->entity.ang.y) * a2;
	v3->speed.z = njCos(v2->entity.ang.y) * a2;
}
void __cdecl AL_ForwardSpd(task* a1, float a2)
{
	ChaoData1* v2; // esi
	float v3; // st7
	Angle v4; // ecx

	v2 = GET_CHAOWK(a1);
	v3 = njSin(v2->entity.ang.y);
	v4 = v2->entity.ang.y;
	v2->entity.pos.x = v3 * a2 + v2->entity.pos.x;
	v2->entity.pos.z = njCos(v4) * a2 + v2->entity.pos.z;
}
void __cdecl AL_Brake(task* a1, float a2)
{
	UnknownData2* v2; // eax

	v2 = (UnknownData2*)a1->EntityData2;
	if (v2->field_40 & 4)
	{
		v2->velocity.x *= a2;
		v2->velocity.y *= a2;
		v2->velocity.z *= a2;
	}
}