#include "stdafx.h"
#include "../cwe_code.h"
#include "include/code_playerspeed.h"

void PlayerSpeed::OnALControl(task* tp) {
	for (int i = 0; i < 2; i++) {
		if (!playerpwp[i]) continue;

		switch(playerpwp[i]->character) {
		case PLNO_MILES:
			playerpwp[i]->p.max_x_spd = 2;
			break;
			
		case PLNO_METAL_SONIC:
			playerpwp[i]->p.slow_down = -0.06f;
			playerpwp[i]->p.run_break = -0.18f;
			playerpwp[i]->p.air_break = -0.17f;
			playerpwp[i]->p.air_resist_air = -0.028f;
			break;
		}
	}
}
