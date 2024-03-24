#include <stdafx.h>
#include <math.h>
#include <corecrt_math_defines.h>

#include "al_tween_types.h"

// these functions are mostly "stolen"/ported from godot source code
// simultaneously they are also ported from http://robertpenner.com/easing/

static float SineEaseIn(float t, float b, float c, float d) {
	return -c * cos(t / d * (M_PI / 2)) + c + b;
}

static float SineEaseOut(float t, float b, float c, float d) {
	return c * sin(t / d * (M_PI / 2)) + b;
}

static float SineEaseInOut(float t, float b, float c, float d) {
	return -c / 2 * (cos(M_PI * t / d) - 1) + b;
}

static float CubicEaseIn(float t, float b, float c, float d) {
	t /= d;
	return c * (t)*t * t + b;
}

static float CubicEaseOut(float t, float b, float c, float d) {
	t = t / d - 1;
	return c * (t * t * t + 1) + b;
}

static float CubicEaseInOut(float t, float b, float c, float d) {
	if ((t /= d / 2) < 1) return c / 2 * t * t * t + b;
	t -= 2;
	return c / 2 * (t * t * t + 2) + b;
}

static float CircEaseIn(float t, float b, float c, float d) {
	return -c * (sqrt(1 - (t /= d) * t) - 1) + b;
}

static float CircEaseOut(float t, float b, float c, float d) {
	t = t / d - 1;
	return c * sqrt(1 - t * t) + b;
}

static float CircEaseInOut(float t, float b, float c, float d) {
	if ((t /= d / 2) < 1) return -c / 2 * (sqrt(1 - t * t) - 1) + b;
	return c / 2 * (sqrt(1 - t * (t - 2)) + 1) + b;
}

static float ExpoEaseIn(float t, float b, float c, float d) {
	if (t == 0) {
		return b;
	}
	return c * pow(2, 10 * (t / d - 1)) + b - c * 0.001;
}

static float ExpoEaseOut(float t, float b, float c, float d) {
	return (t == d) ? b + c : c * (-pow(2, -10 * t / d) + 1) + b;
}

static float ExpoEaseInOut(float t, float b, float c, float d) {
	if (t == 0) return b;
	if (t == d) return b + c;
	if ((t /= d / 2) < 1) return c / 2 * pow(2, 10 * (t - 1)) + b;

	return c / 2 * (-pow(2, -10 * --t) + 2) + b;
}

static float QuadEaseIn(float t, float b, float c, float d) {
	t /= d;
	return c * t * t + b;
}

static float QuadEaseOut(float t, float b, float c, float d) {
	t /= d;
	return -c * t * (t - 2) + b;
}

static float QuadEaseInOut(float t, float b, float c, float d) {
	if ((t /= d / 2) < 1) return ((c / 2) * (t * t)) + b;
	return -c / 2 * (((t - 2) * (t - 1)) - 1) + b; // ???
}

TweenInterpolation TweenInterpolationMethods[INTERP_TYPE_COUNT][EASE_TYPE_COUNT] = {
	{CubicEaseIn, CubicEaseOut, CubicEaseInOut},
	{SineEaseIn, SineEaseOut, SineEaseInOut},
	{CircEaseIn, CircEaseOut, CircEaseInOut},
	{ExpoEaseIn, ExpoEaseOut, ExpoEaseInOut},
	{QuadEaseIn, QuadEaseOut, QuadEaseInOut},
};