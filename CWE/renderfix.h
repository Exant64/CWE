#pragma once
#include "renderfix_api.h"

extern RFAPI_CORE* rfapi_core;

bool RenderFix_IsEnabled();

void RenderFix_Init(const HelperFunctions& helper);