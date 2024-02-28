#include "stdafx.h"
#include <ninja_functions.h>

void __cdecl AL_ForwardAcc(ObjectMaster* a1, float a2)
{
	ChaoData1* v2; // esi
	UnknownData2* v3; // edi

	v2 = a1->Data1.Chao;
	v3 = a1->EntityData2;
	v3->speed.x = njSin(v2->entity.Rotation.y) * a2;
	v3->speed.z = njCos(v2->entity.Rotation.y) * a2;
}
void __cdecl AL_ForwardSpd(ObjectMaster* a1, float a2)
{
	ChaoData1* v2; // esi
	float v3; // st7
	Angle v4; // ecx

	v2 = a1->Data1.Chao;
	v3 = njSin(v2->entity.Rotation.y);
	v4 = v2->entity.Rotation.y;
	v2->entity.Position.x = v3 * a2 + v2->entity.Position.x;
	v2->entity.Position.z = njCos(v4) * a2 + v2->entity.Position.z;
}
void __cdecl AL_Brake(ObjectMaster* a1, float a2)
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