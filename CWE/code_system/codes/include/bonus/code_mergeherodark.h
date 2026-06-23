#pragma once
#include "../../../cwe_code.h"
#include <al_stage.h>

class MergeHeroDark : public Code {
	using Code::Code;
	void OnChaoData(CHAO_PARAM_GC& chao) override;
};

void MergeHeroDark::OnChaoData(CHAO_PARAM_GC& param) {
	switch (NextChaoArea) {
	case CHAO_STG_HERO:
		if (param.place == CHAO_STG_DARK)
			param.place = CHAO_STG_HERO;
		break;
	case CHAO_STG_DARK:
		if (param.place == CHAO_STG_HERO)
			param.place = CHAO_STG_DARK;
		break;
	}
}
