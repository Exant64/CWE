#include "stdafx.h"
#include "SA2ModLoader.h"
#include "al_msg_win.h"

static const int AlMsgWinCreatePtr = 0x00579BB0;
void AlMsgWinCreate(KinderCoMessageThing* self)
{
	__asm
	{
		mov esi, self
		call AlMsgWinCreatePtr
	}
}

static const int AlMsgWinAddLineCPtr = 0x00579E30;
void AlMsgWinAddLineC(KinderCoMessageThing* self , const char* cstr, int ja)
{
	__asm
	{
		mov ebx, cstr
		mov esi, self
		push ja
		call AlMsgWinAddLineCPtr
		add esp, 4
	}
}

static const int AlMsgWinOpenPtr = 0x579D80;
void AlMsgWinOpen(KinderCoMessageThing* self) {
	__asm {
		mov eax, self
		call AlMsgWinOpenPtr
	}
}

void AlMsgWinSetPos(KinderCoMessageThing* self, float px, float py, float pz)
{
	self->mPosX = px;
	self->mPosY = py;
	self->mPosZ = pz;
}

void AlMsgWinSetSize(KinderCoMessageThing* self, float sx, float sy)
{
	self->mSizeX = sx;
	self->mSizeY = sy;
}

const int MsgDialogPtr = 0x0057A060;
void MsgDialog(KinderCoMessageThing* a1) {
	__asm {
		mov ebx, a1
		call MsgDialogPtr
	}
}