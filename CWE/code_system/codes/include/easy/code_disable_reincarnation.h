#pragma once
#include "../../../cwe_code.h"

PATCH_CODE(DisableReincarnation);

void DisableReincarnation::Init() {
	// kills "AL_CalcIntentionScore_Dead"
	WriteJump((void*)0x00562A20, nullsub_1);

	// kills Lifespan sets in "AL_CalcParameter"
	WriteData<7>((char*)0x005354B2, (char)0x90);
	WriteData<7>((char*)0x005354C9, (char)0x90);
}