#include "stdafx.h"
#include "SA2ModLoader.h"
#include "al_stage.h"

bool AL_IsGarden() {
	return CurrentChaoArea >= CHAO_STG_NEUT && CurrentChaoArea <= CHAO_STG_DARK;
}

int AL_GetStageNumber() {
	return CurrentChaoArea;
}