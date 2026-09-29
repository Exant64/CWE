#pragma once

#include "stdafx.h"

bool SaveChaoFile(const wchar_t* const path, const CHAO_SAVE_INFO* pInfo);
CHAO_SAVE_INFO LoadChaoFile(const wchar_t* path);