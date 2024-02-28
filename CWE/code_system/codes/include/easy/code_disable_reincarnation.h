#pragma once
#include "../../../cwe_code.h"

PATCH_CODE(DisableReincarnation);

void DisableReincarnation::Init() {
	// kills "AL_CalcIntentionScore_Dead"
	WriteJump((void*)0x00562A20, nullsub_1);
}