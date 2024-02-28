#pragma once
#include "../../../cwe_code.h"
#include <al_stage.h>

class MergeHeroDark : public Code {
	using Code::Code;
	void OnChaoData(ChaoDataBase& chao) override;
};

void MergeHeroDark::OnChaoData(ChaoDataBase& param) {
	switch (NextChaoArea) {
	case CHAO_STG_HERO:
		if (param.Garden == CHAO_STG_DARK)
			param.Garden = CHAO_STG_HERO;
		break;
	case CHAO_STG_DARK:
		if (param.Garden == CHAO_STG_HERO)
			param.Garden = CHAO_STG_DARK;
		break;
	}
}
