#pragma once
#include "rfapi.h"

extern RFAPI_CORE* rfapi_core;

bool RenderFix_IsEnabled();

void RenderFixBackwardsCompatibilityDrawObject(NJS_OBJECT* pObject);
void RenderFixBackwardsCompatibilityDrawModel(NJS_CNK_MODEL* pModel);
void RenderFixBackwardsCompatibilityDrawMotion(NJS_OBJECT* pObject, NJS_MOTION* pMotion, float frame);

struct RenderFixBackwardsCompatibilityGuard {
    Control3D ctrl {NJD_CONTROL_3D_CNK_CONSTANT_ATTR, NJD_CONTROL_3D_CONSTANT_TEXTURE_MATERIAL};

    RenderFixBackwardsCompatibilityGuard();
    ~RenderFixBackwardsCompatibilityGuard();
};

#define RF_BACKWARDS_COMPAT_GUARD() RenderFixBackwardsCompatibilityGuard _bwcompatguard{}

void rfCnkNormalDrawObject(NJS_OBJECT* pObject);
void rfCnkNormalDrawModel(NJS_CNK_MODEL* pModel);

void RenderFix_Init(const HelperFunctions& helper);