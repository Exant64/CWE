#pragma once
#include "../../cwe_code.h"
#include "al_parameter.h"
#include "ChaoMain.h"

class CantHateCharacters : public Code {
public:

	using Code::Code;

	void OnChaoData(CHAO_PARAM_GC& chao) override;
};

void CantHateCharacters::OnChaoData(CHAO_PARAM_GC& chao) {
	if (AL_ParameterIsGuest(&chao) && gConfigVal.GuestBlockPlayerRelations) return;
	
	for (int i = 0; i < 6; i++) {
		if (chao.knowledge.player[i].like < 0) {
			chao.knowledge.player[i].like = 0;
		}
	}
}