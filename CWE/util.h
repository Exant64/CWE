#pragma once
#include <algorithm>
#include <ninja_functions.h>

#define clamp(val, mini,maxi) (std::clamp(val,mini,maxi))

static float lerp(float a, float b, float f) {
	return (a * (1.0f - f)) + (b * f);
}

// from https://keithmaggio.wordpress.com/2011/02/15/math-magician-lerp-slerp-and-nlerp/
static NJS_VECTOR slerp(const NJS_VECTOR& start, const NJS_VECTOR& end, float percent) {
    // Dot product - the cosine of the angle between 2 vectors.
    // Clamp it to be in the range of Acos()
    const float dot = clamp(njInnerProduct(&start, &end), -1.0f, 1.0f);
    
    // Acos(dot) returns the angle between start and end,
    // And multiplying that by percent returns the angle between
    // start and the final result.
    const float theta = acos(dot) * percent;
    NJS_VECTOR RelativeVec = end;
    RelativeVec.x -= start.x * dot;
    RelativeVec.y -= start.y * dot;
    RelativeVec.z -= start.z * dot;

    njUnitVector(&RelativeVec);
    
    return NJS_VECTOR{
        start.x * cosf(theta) + RelativeVec.x * sinf(theta),
        start.y * cosf(theta) + RelativeVec.y * sinf(theta),
        start.z * cosf(theta) + RelativeVec.z * sinf(theta)
    };
}