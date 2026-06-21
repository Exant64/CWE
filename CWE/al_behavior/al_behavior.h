#pragma once
#include "save/save_item.h"

void  AL_SetBehaviorWithTimer(task* a1, int a2, int a3);
void  Chao_BehaviourQueue(task* a1, int a2);

extern "C" __declspec(dllexport) void AL_SetAccessory(task* a1, int type);
extern "C" __declspec(dllexport) int AL_GetAccessory(task* a1, int type);
void AL_SetAccessory(task* a1, const AccessorySaveInfo* saveInfo, int type);

void AL_Behavior_Init();

void AL_GrabObjectBothHands(task* a2, task* a1);