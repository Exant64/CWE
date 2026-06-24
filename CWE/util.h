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

static NJS_POINT3 CalcRadiusCenter;
static float CalcRadius;

static int MaxRadius(NJS_CNK_MODEL* pModel) {
    if (!pModel) {
        return 0;
    }

    NJS_POINT3 modelCenter = pModel->center;
    sub_426CC0(_nj_current_matrix_ptr_, &modelCenter, &modelCenter, 0);

    if (CalcRadius < 0) {
        CalcRadiusCenter = modelCenter;
        CalcRadius = pModel->r;
    }
    else if (pModel->r > 0) {
        NJS_POINT3 d = {
            modelCenter.x - CalcRadiusCenter.x,
            modelCenter.y - CalcRadiusCenter.y,
            modelCenter.z - CalcRadiusCenter.z
        };

        float dist = njUnitVector(&d);

        if (dist + pModel->r > CalcRadius) {
            if (dist + CalcRadius <= pModel->r) {
                CalcRadiusCenter = modelCenter;
                CalcRadius = pModel->r;
            }
            else {
                float newRadius = (dist + pModel->r + CalcRadius) * 0.5f;
                const float difference = (newRadius - CalcRadius);
                NJS_POINT3 newCenter = {
                    .x = CalcRadiusCenter.x + d.x * difference,
                    .y = CalcRadiusCenter.y + d.y * difference,
                    .z = CalcRadiusCenter.z + d.z * difference
                };

                CalcRadiusCenter = newCenter;
                CalcRadius = newRadius;
            }
        }
    }

    return 0;
}

static float CalcMaxRadius(NJS_OBJECT* pObj, NJS_POINT3& center) {
    CalcRadius = -1;

    njPushUnitMatrix();
    ProcessChunkModelsWithCallback(pObj, MaxRadius);
    njPopMatrixEx();

    center = CalcRadiusCenter;
    return CalcRadius;
}