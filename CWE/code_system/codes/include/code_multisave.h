#pragma once
#include <code_system/cwe_code.h>

ONFRAME_CODE(MultiSave);

void MultiSave::OnFrame() {
	switch (*(Uint16*)0x0173D040) {
	case 0x3130:
		*(Uint8*)0x0136606D = 0x5F;
		break;

	case 0x3230:
		*(Uint8*)0x0136606D = 0x31;
		break;

	case 0x3330:
		*(Uint8*)0x0136606D = 0x32;
		break;

	case 0x3430:
		*(Uint8*)0x0136606D = 0x33;
		break;

	case 0x3530:
		*(Uint8*)0x0136606D = 0x34;
		break;

	case 0x3630:
		*(Uint8*)0x0136606D = 0x35;
		break;

	case 0x3730:
		*(Uint8*)0x0136606D = 0x36;
		break;

	case 0x3830:
		*(Uint8*)0x0136606D = 0x37;
		break;

	case 0x3930:
		*(Uint8*)0x0136606D = 0x38;
		break;

	case 0x3031:
		*(Uint8*)0x0136606D = 0x39;
		break;
	}
}