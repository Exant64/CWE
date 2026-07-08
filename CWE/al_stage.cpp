#include "stdafx.h"
#include "SA2ModLoader.h"
#include "al_stage.h"

bool AL_IsGarden() {
	return ChaoStageNumber >= CHAO_STG_NEUT && ChaoStageNumber <= CHAO_STG_DARK;
}

int AL_GetStageNumber() {
	return ChaoStageNumber;
}