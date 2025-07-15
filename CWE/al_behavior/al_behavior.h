#pragma once

void  AL_SetBehaviorWithTimer(ObjectMaster* a1, int a2, int a3);
void  Chao_BehaviourQueue(ObjectMaster* a1, int a2);

extern "C" __declspec(dllexport) void AL_SetAccessory(ObjectMaster* a1, int type);
extern "C" __declspec(dllexport) int AL_GetAccessory(ObjectMaster* a1, int type);

void AL_Behavior_Init();

void AL_GrabObjectBothHands(ObjectMaster* a2, ObjectMaster* a1);