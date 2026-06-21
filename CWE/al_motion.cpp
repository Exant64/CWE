#include "stdafx.h"
#include <Chao.h>

const int Chao_AnimationPtr = 0x00793C40;
void Chao_Animation(MotionTableData* a1, int a2)
{
	__asm
	{
		mov eax, a1
		mov edx, a2
		call Chao_AnimationPtr
	}
}

const int Chao_PlayAnimationSpeedPtr = 0x00793D30;
void  Chao_PlayAnimationSpeed(MotionTableData* result, int a2, unsigned __int16 a3)
{
	__asm
	{
		push dword ptr[a3]
		mov edx, a2
		mov eax, result
		call Chao_PlayAnimationSpeedPtr
		add esp, 4
	}
}

unsigned int __cdecl AL_IsMotionStop(task* a1)
{
	return GET_CHAOWK(a1)->MotionTable.flag & 8;
}
void AL_SetMotionLink(task* a1, int index)
{
	Chao_Animation(&GET_CHAOWK(a1)->MotionTable, index);
}
void AL_SetMotionLinkStep(task* a1, int index, int spd)
{
	Chao_PlayAnimationSpeed(&GET_CHAOWK(a1)->MotionTable, index, spd);
}
void __cdecl AL_SetMotionSpd(task* a1, float a2)
{
	GET_CHAOWK(a1)->MotionTable.frameIncreaseSpeed_ = a2;
}

int AL_GetMotionNum(task* tp) {
	MotionTableData* Ctrl = &GET_CHAOWK(tp)->MotionTable;
	return Ctrl->AnimID;
}

int AL_GetMotionPosture(task* tp) {
	MotionTableData* Ctrl = &GET_CHAOWK(tp)->MotionTable;
	return Ctrl->gap2;
}

int AL_IsMotionEnd(task* a1) {
	return GET_CHAOWK(a1)->MotionTable.flag & 1;
};
