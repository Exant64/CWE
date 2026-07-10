#pragma once
#undef SA2MODLOADER_H
#include "rfapi.h"
#define SA2MODLOADER_H

extern const RFAPI_CORE* rfapi_core;

bool RenderFix_IsEnabled();

void RenderFixBackwardsCompatibilityDrawObject(NJS_CNK_OBJECT* pObject);
void RenderFixBackwardsCompatibilityDrawModel(NJS_CNK_MODEL* pModel);
void RenderFixBackwardsCompatibilityDrawMotion(NJS_CNK_OBJECT* pObject, NJS_MOTION* pMotion, float frame);

struct RenderFixBackwardsCompatibilityGuard {
    Control3D ctrl {NJD_CONTROL_3D_CNK_CONSTANT_ATTR, NJD_CONTROL_3D_CONSTANT_TEXTURE_MATERIAL};

    RenderFixBackwardsCompatibilityGuard();
    ~RenderFixBackwardsCompatibilityGuard();
};

#define RF_BACKWARDS_COMPAT_GUARD() RenderFixBackwardsCompatibilityGuard _bwcompatguard{}

void rfCnkNormalDrawObject(NJS_CNK_OBJECT* pObject);
void rfCnkNormalDrawModel(NJS_CNK_MODEL* pModel);

void RenderFix_VersionCheck(const HelperFunctions& helper);