#pragma once
#include "save/save_item.h"

void  AL_SetBehaviorWithTimer(task* a1, int a2, int a3);
void  Chao_BehaviourQueue(task* a1, int a2);

void AL_SetNextBehaviorWithUserData(task* tp, BHV_FUNC Func, void* pUserData);
void* AL_BehaviorGetUserData(task* tp);

void AL_BehaviorSetFreeWork(task* tp, int info);
int AL_BehaviorGetFreeWork(task* tp);
void AL_SetBehaviorWithFreeWork(task* tp, BHV_FUNC Func, int info);

extern "C" __declspec(dllexport) void AL_SetAccessory(task* a1, int type);
extern "C" __declspec(dllexport) int AL_GetAccessory(task* a1, int type);
void AL_SetAccessory(CHAO_PARAM_CWE* pParamCwe, int type);
void AL_SetAccessory(task* a1, const AccessorySaveInfo* saveInfo, int type);

void AL_Behavior_Init();

void AL_GrabObjectBothHands(task* a2, task* a1);