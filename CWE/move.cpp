#include "stdafx.h"
#include "move.h"

float MOV_DistFromAimXZ(task* tp) {
	NJS_POINT3* pos = &tp->twp->pos;
	NJS_POINT3* aimPos = &GET_MOVE_WORK(tp)->AimPos;

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
	if (!pPos || !tp || !tp->mwp) {
		PrintDebug("MOV_SetAimPos error");
		return;
	}

	GET_MOVE_WORK(tp)->AimPos = *pPos;
}

void MOV_SetVelo(task* tp, NJS_VECTOR* pVelo) {
	GET_MOVE_WORK(tp)->Velo = *pVelo;
}