#pragma once
#include "../../cwe_code.h"

PATCH_CODE(JackInBoxPunishment);
PATCH_CODE(IgnoreMinigameIntel);

void JackInBoxPunishment::Init() {
	WriteData((int*)0x0053F994, (int)170); //shortened from 500 to 170
}

void IgnoreMinigameIntel::Init() {
	WriteData((int*)0x0053F7BF, (int)0x00538EE0);
	WriteData((char*)0x0053F7DE, (char)0xEB);
	WriteData((char*)0x0053F80F, (char)0xEB);
	WriteData((char*)0x0053F840, (char)0xEB);
	WriteData((char*)0x0053F871, (char)0xEB);
}