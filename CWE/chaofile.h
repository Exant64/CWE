#pragma once

#include "stdafx.h"

bool SaveChaoFile(const char* const path, const CHAO_SAVE_INFO* pInfo);
CHAO_SAVE_INFO LoadChaoFile(const char* path);