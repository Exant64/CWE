#pragma once

#include <string>
#include <cwe_api.h>

extern CWE_API_REGISTER_MOTION AL_ModAPI_Motion;

// needed to define legacy struct
extern "C" __declspec(dllexport) int RegisterChaoAnimation(std::string name, MOTION_TABLE* pMotionTable);
bool SetChaoMotionTransition(const std::string& from, const std::string& to);
size_t GetChaoMotionIndex(const std::string& name);
MOTION_TABLE* GetChaoMotionTable(size_t index);

void AL_ModAPI_Motion_Init();
void AL_ModAPI_Motion_Update();