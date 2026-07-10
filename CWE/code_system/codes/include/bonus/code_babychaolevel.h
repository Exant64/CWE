#pragma once
#include "../../../cwe_code.h"
#include <al_behavior/al_intention.h>

class StayBabyChaoLvl : public Code {
public:

	using Code::Code;

	//maybe change to OnChaoObject?
	void OnChaoData(CHAO_PARAM_GC& chao) override;
};

void StayBabyChaoLvl::OnChaoData(CHAO_PARAM_GC& chao) {
	if (chao.type != 2) return;
	if (chao.Lev[5] >= 99) return;
	
	for (int i = 0; i < 4; i++) {
		if (chao.Lev[i] < 10) {
			// 1 - 0.016f = 0.984
			float threshold = ChaoGlobal.GrowthMayuPoint - 0.002f;
			if (chao.body.growth > threshold) {
				chao.body.growth = threshold;
			}
		}
	}
}