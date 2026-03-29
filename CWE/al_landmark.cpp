#include "stdafx.h"

const int sub_535010Ptr = 0x535010;
int AL_GetCurrLandAttr(NJS_POINT3* pPos) {
	int rtn;
	__asm
	{
		mov eax, pPos
		call sub_535010Ptr
		mov rtn, eax
	}
}
