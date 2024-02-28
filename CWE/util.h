#pragma once
#include <algorithm>
static float lerp(float a, float b, float f) {
	return (a * (1.0f - f)) + (b * f);
}

#define clamp(val, mini,maxi) (std::clamp(val,mini,maxi))