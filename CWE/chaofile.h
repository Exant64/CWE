#pragma once

#include "stdafx.h"

int SaveChaoFile(const wchar_t* const path, const CHAO_SAVE_INFO* pInfo);
int LoadChaoFile(const wchar_t* path, CHAO_SAVE_INFO& outInfo);