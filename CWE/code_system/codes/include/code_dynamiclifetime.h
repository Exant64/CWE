#pragma once
#include <code_system/cwe_code.h>
#include <al_behavior/al_intention.h>

ONFRAME_CODE(DynamicLifetime);

void DynamicLifetime::OnFrame() {
	Uint8 count = *(Uint8*)0x01DC0F80;
	
	if (count <= 0) return;

	if (count > 16) {
		ChaoGlobal.ChaoTimerLifeInterval = 240;
	}
	else {
		ChaoGlobal.ChaoTimerLifeInterval = 40 + (count * 10);
	}
}