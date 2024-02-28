#pragma once
#include "../../../cwe_code.h"
#include <al_behavior/al_intention.h>

class StayBabyChaoLvl : public Code {
public:

	using Code::Code;

	//maybe change to OnChaoObject?
	void OnChaoData(ChaoDataBase& chao) override;
};

void StayBabyChaoLvl::OnChaoData(ChaoDataBase& chao) {
	if (chao.Type != 2) return;
	if (chao.StatLevels[5] >= 99) return;
	
	for (int i = 0; i < 4; i++) {
		if (chao.StatLevels[i] < 10) {
			// 1 - 0.016f = 0.984
			float threshold = ChaoGlobal.GrowthMayuPoint - 0.002f;
			if (chao.EvolutionProgress > threshold) {
				chao.EvolutionProgress = threshold;
			}
		}
	}
}