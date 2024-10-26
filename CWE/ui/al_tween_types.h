#pragma once

typedef float (*TweenInterpolation)(float t, float b, float c, float d);

enum INTERP_TYPE {
	INTERP_CUBIC,
	INTERP_SINE,
	INTERP_CIRC,
	INTERP_EXPO,
	INTERP_QUAD,
	INTERP_ELASTIC,
	INTERP_TYPE_COUNT
};

enum EASE_TYPE {
	EASE_IN,
	EASE_OUT,
	EASE_INOUT,
	EASE_TYPE_COUNT
};
