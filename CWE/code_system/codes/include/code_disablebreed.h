#pragma once
#include "../../cwe_code.h"

PATCH_CODE(DisableBreed);

void DisableBreed::Init() {
	WriteJump((void*)0x00562880, (void*)nullsub_1);
}