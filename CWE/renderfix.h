#pragma once
#include "rfapi.h"

extern RFAPI_CORE* rfapi_core;

bool RenderFix_IsEnabled();

void RenderFixBackwardsCompatibilityDrawObject(NJS_OBJECT* pObject);
void RenderFixBackwardsCompatibilityDrawModel(NJS_CNK_MODEL* pModel);

void rfCnkNormalDrawObject(NJS_OBJECT* pObject);
void rfCnkNormalDrawModel(NJS_CNK_MODEL* pModel);

void RenderFix_Init(const HelperFunctions& helper);