#pragma once

#include "stdafx.h"

#include <vector>
#include <optional>

enum {
	NAV_AREA_GROUND = 0,
	NAV_AREA_WATER = 1
};

enum {
	NAV_FLAGS_WALK = BIT_0,
	NAV_FLAGS_SWIM = BIT_1
};

using NavSysPathResult = std::vector<NJS_POINT3>;

void NavSysDiscardResult(const uint32_t queryIndex);
uint32_t NavSysQueryPath(const NJS_POINT3* pStartPos, const NJS_POINT3* pEndPos, const uint32_t excludeFlags);
std::optional<NavSysPathResult> NavSysGetResult(const uint32_t queryIndex);

task* GetNavSysTask();
void NavSysCreate();