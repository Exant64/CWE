#pragma once
#include "../../../cwe_code.h"
#include "al_parameter.h"
#include "ChaoMain.h"

class DisableLevelLimit : public Code {
public:

	using Code::Code;

	//maybe change to OnChaoObject?
	void OnChaoData(CHAO_PARAM_GC& chao) override;
};

void DisableLevelLimit::OnChaoData(CHAO_PARAM_GC& chao) {
	if (AL_ParameterIsGuest(&chao) && gConfigVal.GuestBlockPlayerRelations) return;
	
	for (int i = 0; i < 5; i++)
		if (chao.Lev[i] == 99)
			chao.Lev[i] = 0;
}