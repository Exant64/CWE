#include "stdafx.h"
#include "SA2ModLoader.h"
#include "al_msg_font.h"

const int AlMsgFontDrawRegionScale2Ptr = 0x0544820;
void AlMsgFontDrawRegionScale2(signed int a1, MessageFontThing* a2, float a3, float a4, float a5, float a6, float a7, float a8, float a9)
{
	__asm
	{
		mov eax, a1
		mov ecx, a2
		push[a9]
		push[a8]
		push[a7]
		push[a6]
		push[a5]
		push[a4]
		push[a3]
		call AlMsgFontDrawRegionScale2Ptr
		add esp, 28
	}
}

const int AlMsgFontCreateCStrPtr = 0x05444E0;
void AlMsgFontCreateCStr(int a1, int a2, int a3, int a4)
{
	__asm
	{
		mov edx, a1
		mov ecx, a2
		mov esi, a3
		push a4
		call AlMsgFontCreateCStrPtr
		add esp, 4
	}
}

const int AlMsgFontDeletePtr = 0x00544530;
void AlMsgFontDelete(MessageFontThing* a1)
{
	__asm
	{
		mov esi, a1
		call AlMsgFontDeletePtr
	}
}

const int DrawFontThingPtr = 0x00544580;
void DrawFontThing(__int16* a1, float a2, float a3, float a4)
{
	__asm
	{
		push[a4]
		push[a3]
		push[a2]
		mov ebx, a1
		call DrawFontThingPtr
		add esp, 12
	}
}

const int AL_KinderPMessageExec_LoadPtr = 0x00585460;
ObjectMaster* AL_KinderPMessageExec_Load(ObjectMaster* a1, AL_KinderPMessage* a2)
{
	ObjectMaster* retval;
	__asm
	{
		mov eax, a1
		push a2
		call AL_KinderPMessageExec_LoadPtr
		add esp, 4
		mov retval, eax
	}
	return retval;
}