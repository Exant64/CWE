#pragma once
#include <Chao.h>

enum eSpeechPos {
	SPEECH_POS_LEFT,
	SPEECH_POS_RIGHT,
};

enum {
	SPEECH_ENTRY_FRUIT_SQUARE,
	SPEECH_ENTRY_FRUIT_ROUND,
	SPEECH_ENTRY_FRUIT_TRIANGLE,

	SPEECH_ENTRY_TOY_DUCK,

	NB_SPEECH_ENTRY
};

task* AL_SpeechBubbleCreate(task* pChao, ChaoData* pChaoData, Uint32 entryID, eSpeechPos position, Uint32 spawnTimer, Uint32 aliveTimer);