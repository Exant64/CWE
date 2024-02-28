#include "stdafx.h"
#include "include/code_animal_reuse.h"
#include <c_colli.h>

FastcallFunctionPointer(signed int, sub_56D170, (int a1, ObjectMaster* a2), 0x56D170);
signed int __fastcall MinimalHold(int a1, ObjectMaster* a2)
{
	int test = sub_56D170(a1, a2);
	if (!test)
	{
		CCL_Disable(a2, 1);
	}
	return test;
}

void AnimalReuse::Init() {
	WriteData((char*)0x548944, (char)0x00); //set used flag to 0 instead of 1
	WriteData((char*)0x54894D, (char)0xEB); //jump over itemsave nulling code
	WriteCall((void*)0x548931, MinimalHold); //hooking "held by chao" check function
	WriteData<4>((char*)0x00548996, (char)0x90); //nop instruction that sets status (status related to being unpickupable?)
}