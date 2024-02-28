#pragma once
#include "../../cwe_code.h"
#include <al_stage.h>

ONFRAME_CODE(RaceCamSwitch);

void RaceCamSwitch::OnFrame() {

	switch (AL_GetStageNumber()) {
	case CHAO_STG_RACE:
	case CHAO_STG_RACE_2P:
		for (int i = 0; i < 8; i++) {
			if (GetAsyncKeyState(49 + i)) {
				*(Uint8*)0x01DBEC80 = i;
			}
		}
		break;
	}
}