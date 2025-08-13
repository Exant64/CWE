#include "stdafx.h"
#include "renderfix.h"

RFAPI_CORE* rfapi_core = NULL;

bool RenderFix_IsEnabled() {
	return rfapi_core != NULL;
}

void RenderFix_Init(const HelperFunctions& helper) {
	auto* rf = helper.Mods->find("sa2-render-fix");
	if (!rf) return;

	rfapi_core = rf->GetDllExport<RFAPI_CORE*>("rfapi_core");
}