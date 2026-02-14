#pragma once

#include <cwe_api.h>

extern CWE_API CWE_API_Main;

void CWE_API_FindMods();

void CWE_API_EarlyInit();
void CWE_API_LateInit();
void CWE_API_EarlyLoad();
void CWE_API_Load();