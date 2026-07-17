#pragma once

#include "c_colli.h"

void ObjectMovableInitialize(taskwk* a1, int a2);

void SaveToyPos();

extern CCL_INFO pickupableColli;
void AL_Toy_Move_Update(task* a1);
void AL_Toy_Move_Init(task* tp, const CCL_INFO* pInfo, size_t count);
void AL_Toy_Moveable_Init();
void AL_Toy_Move_Register(task* obj, __int16 a3);