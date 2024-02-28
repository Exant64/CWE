#include "stdafx.h"
#include "include/code_sick.h"

void ChaoSick::Init() {
	switch (m_param) {
	case 1:
		WriteData((double*)0x00905A90, (double)0.001666666648816317);
		break;
	case 2:
		WriteData((double*)0x00905A90, (double)0.01666666648816317);
		break;
	case 3:
		WriteData((double*)0x00905A90, (double)0.1666666648816317);
		break;
	}
}