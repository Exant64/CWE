#include "stdafx.h"

float MOV_DistFromAimXZ(task* tp) {
	NJS_POINT3* pos = &tp->Data1.Entity->Position;
	NJS_POINT3* aimPos = &tp->EntityData2->Waypoint;

	NJS_VECTOR v = {
		pos->x - aimPos->x,
		0,
		pos->z - aimPos->z
	};

	return njScalor(&v);
}

const int sub_796910Ptr = 0x796910;
int MOV_TurnToAim2(task* tp, int ang)
{
	int result;
	__asm
	{
		mov eax, tp
		push ang
		call sub_796910Ptr
		add esp, 4
		mov result, eax

	}
	return result;
}

void MOV_SetAimPos(task* tp, NJS_POINT3* pPos) {
	if (!pPos || !tp || !tp->EntityData2) {
		PrintDebug("MOV_SetAimPos error");
		return;
	}

	tp->EntityData2->Waypoint = *pPos;
}

void MOV_SetVelo(task* tp, NJS_VECTOR* pVelo) {
	UnknownData2* pMove = tp->EntityData2;
	pMove->velocity = *pVelo;
}