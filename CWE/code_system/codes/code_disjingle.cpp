#include "stdafx.h"
#include "include/code_disjingle.h"

void DisableLevelupJingle::Init() {
	//thanks buster for the original implementation of the code
	WriteCall((void*)0x00565066, nullsub_1);
}