#pragma once
#include "../../../cwe_code.h"
#include <al_stage.h>
#include <FunctionHook.h>
#include <al_chao_info.h>

enum {
	MERGE_HERO_DARK = 1,
	MERGE_ALL = 2
};

class MergeGardens : public Code {
	using Code::Code;
	void OnChaoData(CHAO_PARAM_GC& chao) override;
	void Init() override;
};

void MergeGardens::OnChaoData(CHAO_PARAM_GC& param) {
	if(m_param != MERGE_HERO_DARK) return;

	switch (ChaoNextStageNumber) {
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

static FunctionHook<void> AL_SetChaoOnTheGarden_t(0x531B10);

static void AL_SetChaoOnTheGarden_r() {
	if(AL_IsGarden()) {
		for(auto& param : ChaoInfo::Instance()) {
			if(!param->type) continue;

			param->place = AL_GetStageNumber();
		}
	}

	AL_SetChaoOnTheGarden_t.Original();
}

void MergeGardens::Init() {
	if (m_param != MERGE_ALL) return;

	WriteData<10>((char*)0x0531C89, (char)0x90);
}