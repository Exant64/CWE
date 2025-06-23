#pragma once
#include "stdafx.h"

#define DROP_COUNT 1600

struct RAIN_WORK {
	Uint32 color;
	Uint32 timer;
	Uint32 timerLimit;
	Uint16 angle;
	Uint16 startAngle;
	Uint16 targetAngle;

	struct RAIN_DROP {
		NJS_POINT3 startPos;
		Uint8 lifeCount;
		Uint8 scale;
		float collisionY;
		float progress;
	} drops[DROP_COUNT];
};

task* AL_CreateDayNightRain(Uint32 timer, Uint32 color);