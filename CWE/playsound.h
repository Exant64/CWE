#pragma once

#include "stdafx.h"
#include "al_stage.h"

enum CWE_SOUND {
	CWE_SOUND_ACCORDION = 161,
	CWE_SOUND_GUITAR = 162,
	CWE_SOUND_TRIANGLE = 163,
	CWE_SOUND_SAND = 164,
	CWE_SOUND_RAIN = 165
};

#define TONE(bnk, no) (Sint32)((((bnk)<<12)&0xF000)+(no))

static const void* const SE_CallV2_TIMERPtr = (void*)0x437590;
static inline void SE_CallV2_TIMER(task* obj, int a1, NJS_VECTOR* a2, char a4, char a5, __int16 a6)
{
	__asm
	{
		push dword ptr[a6]
		push dword ptr[a5]
		push dword ptr[a4]
		mov esi, a2
		mov edi, a1
		mov ebx, obj
		call SE_CallV2_TIMERPtr
		add esp, 12
	}
}

#define AL_SE_CallV2(a1, a3, a4, a5, a2) if(!AL_IsChangeStageRequested()) SE_CallV2(a1, a3, a4, a5, a2)
#define AL_SE_CallV2_TIMER(obj, a1, a2, a4, a5, a6) if(!AL_IsChangeStageRequested()) SE_CallV2_TIMER(obj, a1, a2, a4, a5, a6)