#include "stdafx.h"
#include "../cwe_code.h"
#include "include/code_playerspeed.h"

void PlayerSpeed::OnALControl(ObjectMaster* tp) {
	for (int i = 0; i < 2; i++) {
		if (!MainCharObj2[i]) continue;

		switch(MainCharObj2[i]->CharID2) {
		case Characters_Tails:
			MainCharObj2[i]->PhysData.BaseSpeed = 2;
			break;
			
		case Characters_MetalSonic:
			MainCharObj2[i]->PhysData.GroundDecel = -0.06f;
			MainCharObj2[i]->PhysData.BrakeSpeed = -0.18f;
			MainCharObj2[i]->PhysData.AirBrakeSpeed = -0.17f;
			MainCharObj2[i]->PhysData.AirDecel = -0.028f;
			break;
		}
	}
}
