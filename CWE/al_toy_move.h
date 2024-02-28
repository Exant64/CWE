#pragma once

#include "c_colli.h"

enum TOY_MOVE_STATE {
	TOY_MOVE_IDLE = 0,
	TOY_MOVE_STATIC = 1,
	TOY_MOVE_DYNAMIC = 2,
	TOY_MOVE_HOLDP = 3,

};
struct AL_TOY_MOVE
{
	TOY_MOVE_STATE state;
	int timer;
	float floatVal;
	int flag;
};

extern CCL_INFO pickupableColli;
void AL_Toy_Move_Update(ObjectMaster* a1);
void AL_Toy_Move_Init(ObjectMaster* p, CCL_INFO* col);
void AL_Toy_Moveable_Init();
void AL_Toy_Move_Register(ObjectMaster* obj, __int16 a3);