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

Sint8 SE_Call_Timer(int tone, const void* id, int pri, int volofs, int timer);
Sint8 SE_CallV2_Timer(int tone, const void* id, int pri, int volofs, int timer, const NJS_POINT3* pPos);

#define AL_SE_CallV2(a1, a3, a4, a5, a2) if(!AL_IsChangeStageRequested()) SE_CallV2(a1, a3, a4, a5, a2)
#define AL_SE_CallV2_TIMER(obj, a1, a2, a4, a5, a6) if(!AL_IsChangeStageRequested()) SE_CallV2_Timer(obj, a1, a2, a4, a5, a6)