#pragma once

#include "stdafx.h"

#include <vector>
#include <optional>

using NavSysPathResult = std::vector<NJS_POINT3>;

uint32_t NavSysAddPath(const NJS_POINT3* pStartPos, const NJS_POINT3* pEndPos);
std::optional<NavSysPathResult> NavSysGetResult(const uint32_t queryIndex);

task* GetNavSysTask();
void NavSysCreate();