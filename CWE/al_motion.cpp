#include "stdafx.h"

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

unsigned int __cdecl AL_IsMotionStop(ObjectMaster* a1)
{
	return a1->Data1.Chao->MotionTable.flag & 8;
}
void AL_SetMotionLink(ObjectMaster* a1, int index)
{
	Chao_Animation(&a1->Data1.Chao->MotionTable, index);
}
void AL_SetMotionLinkStep(ObjectMaster* a1, int index, int spd)
{
	Chao_PlayAnimationSpeed(&a1->Data1.Chao->MotionTable, index, spd);
}
void __cdecl AL_SetMotionSpd(ObjectMaster* a1, float a2)
{
	a1->Data1.Chao->MotionTable.frameIncreaseSpeed_ = a2;
}
int __cdecl AL_IsMotionEnd(ObjectMaster* a1)
{
	return a1->Data1.Chao->MotionTable.flag & 1;
};
int __cdecl AL_IsMotionChange(ObjectMaster* a1)
{
	return a1->Data1.Chao->MotionTable.flag & 2;
};
