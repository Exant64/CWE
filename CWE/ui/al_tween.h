#pragma once
#include "stdafx.h"
#include "al_tween.h"
#include "../memory.h"

#include <math.h>
#include <corecrt_math_defines.h>

#include "al_tween_types.h"
#include "al_tween_interp.h"

typedef void(*TweenCallback)(task* pParent);

template <typename T>
struct TweenData {
	TweenInterpolation InterpolationMethod;
	T* Value;
	T Original;
	T Target;
	int FrameCount;
	int Timer;
	TweenCallback Callback;
	task** TweenPointer;
	task* Parent;
};

#define GET_TWEEN_DATA(tp) ((TweenData<T>*)(tp->Data2.Undefined))

// so the idea is to template specialize the interpolate functions 
// for stuff like NJS_POINT2 and colors and such

template <typename T>
static void TweenInterpolateValue(TweenData<T>* data, T* value, const T& original, const T& target, float time, float duration) {
	static_assert(false, "unimplemented tween type");
}

template <>
static void TweenInterpolateValue(TweenData<float>* data, float* value, const float& original, const float& target, float time, float duration) {
	*value = data->InterpolationMethod(time, original, target - original, duration);
}

template <>
static void TweenInterpolateValue(TweenData<NJS_POINT2>* data, NJS_POINT2* value, const NJS_POINT2& original, const NJS_POINT2& target, float time, float duration) {
	value->x = data->InterpolationMethod(time, original.x, target.x - original.x, duration);
	value->y = data->InterpolationMethod(time, original.y, target.y - original.y, duration);
}

template <>
static void TweenInterpolateValue(TweenData<NJS_POINT3>* data, NJS_POINT3* value, const NJS_POINT3& original, const NJS_POINT3& target, float time, float duration) {
	value->x = data->InterpolationMethod(time, original.x, target.x - original.x, duration);
	value->y = data->InterpolationMethod(time, original.y, target.y - original.y, duration);
	value->z = data->InterpolationMethod(time, original.z, target.z - original.z, duration);
}

template <typename T>
static void TweenExecutor(ObjectMaster* tp) {
	// i can't use auto because intellisense is a fucking idiot and it doesn't autocomplete it
	TweenData<T>* tween = GET_TWEEN_DATA(tp);

	if (tween->Timer == tween->FrameCount + 1) {
		tp->MainSub = DeleteObject_;

		if (tween->TweenPointer) {
			*tween->TweenPointer = NULL;
		}

		if (tween->Callback) {
			tween->Callback(tween->Parent);
		}

		return;
	}

	TweenInterpolateValue<T>(
		tween,
		tween->Value,
		tween->Original,
		tween->Target,
		tween->Timer,
		tween->FrameCount
	);

	tween->Timer++;
}

static void TweenDestructor(ObjectMaster* tp) {
	// technically the callback call could go in here too, but I feel more comfortable with putting this logic in the mainsub
	FREE(tp->Data2.Undefined);
}

template <typename T>
void TweenSetTaskPointer(task* tween, task** ptp) {
	TweenData<T>* work = GET_TWEEN_DATA(tween);
	work->TweenPointer = ptp;
}

template <typename T>
ObjectMaster* CreateTween(ObjectMaster* parent, const EASE_TYPE easingMode, const INTERP_TYPE interpolation, T* whatToChange, const T& target, const int frameCount, const TweenCallback callback) {
	ObjectMaster* tp;

	if (parent == nullptr) {
		tp = LoadObject(4, "AL_Tween", TweenExecutor<T>, (LoadObj)0);
	}
	else {
		tp = LoadChildObject((LoadObj)0, TweenExecutor<T>, parent);
	}

	tp->Data2.Undefined = ALLOC(TweenData<T>);
	tp->DeleteSub = TweenDestructor;

	TweenData<T>* tween = GET_TWEEN_DATA(tp);
	tween->InterpolationMethod = TweenInterpolationMethods[interpolation][easingMode];
	tween->Value = whatToChange;
	tween->Original = *tween->Value;
	tween->Target = target;
	tween->Timer = 0;
	tween->FrameCount = frameCount;
	tween->Callback = callback;
	tween->TweenPointer = NULL;
	tween->Parent = parent;

	return tp;
}