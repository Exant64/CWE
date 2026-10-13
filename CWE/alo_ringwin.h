#include "stdafx.h"

void ALO_RingWinAdd(int num);

// real name is gTask, was a static variable
DataPointer(task*, gRingWinTask, 0x1AEDBFC);

FunctionPointer(void, ALO_RingWinCreate, (float x, float y), 0x005A6AC0);
VoidFunc(ALO_RingWinDelete, 0x5A6B10);