#include "stdafx.h"
#include "include/code_gardenlimit.h"
#include <util.h>

void GardenLimit::Init() {
	int SetLimit = clamp(m_param, 8, 24);
	
	WriteData((unsigned char*)0x00531BA1, (unsigned char)SetLimit);
	WriteData((unsigned char*)0x005628F1, (unsigned char)SetLimit);
}