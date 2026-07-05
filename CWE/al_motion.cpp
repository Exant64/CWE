#include "stdafx.h"
#include <Chao.h>

const int Chao_AnimationPtr = 0x00793C40;
void Chao_Animation(MOTION_CTRL* a1, int a2)
{
	__asm
	{
		mov eax, a1
		mov edx, a2
		call Chao_AnimationPtr
	}
}

const int Chao_PlayAnimationSpeedPtr = 0x00793D30;
void  Chao_PlayAnimationSpeed(MOTION_CTRL* result, int a2, unsigned __int16 a3)
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
	return GET_CHAOWK(a1)->MotionCtrl.flag & 8;
}
void AL_SetMotionLink(task* a1, int index)
{
	Chao_Animation(&GET_CHAOWK(a1)->MotionCtrl, index);
}
void AL_SetMotionLinkStep(task* a1, int index, int spd)
{
	Chao_PlayAnimationSpeed(&GET_CHAOWK(a1)->MotionCtrl, index, spd);
}
void __cdecl AL_SetMotionSpd(task* a1, float a2)
{
	GET_CHAOWK(a1)->MotionCtrl.multi_spd = a2;
}

int AL_GetMotionNum(task* tp) {
	MOTION_CTRL* Ctrl = &GET_CHAOWK(tp)->MotionCtrl;
	return Ctrl->curr_num;
}

int AL_GetMotionPosture(task* tp) {
	MOTION_CTRL* Ctrl = &GET_CHAOWK(tp)->MotionCtrl;
	return Ctrl->posture;
}

int AL_IsMotionEnd(task* a1) {
	return GET_CHAOWK(a1)->MotionCtrl.flag & 1;
};
