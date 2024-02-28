#pragma once
#include "SA2ModLoader.h"
#include "alg_kinder_bl.h"
#include "memory.h"

#pragma pack(push, 8)
struct __declspec(align(4)) AL_MSGWarnKinderCoMessageThing
{
	char mActive;
	char mOpen;
	char field_2;
	char field_3;
	float mPosX;
	float mPosY;
	float mSizeX;
	float mSizeY;
	KinderCoMessageThing* mWin;
};
#pragma pack(pop)

DataArray(AL_MSGWarnKinderCoMessageThing, AL_MSGWarnKinderMessageArray, 0x01A259E8, 4);
static void AlMsgWarnCreate(int winid, float px, float py, float sx, float sy)
{
	AL_MSGWarnKinderCoMessageThing& warnWin = AL_MSGWarnKinderMessageArray[winid];
	
	if (!warnWin.mActive)
	{
		*(int*)0x1A557C8 = 0;

		warnWin.mActive = 1;
		warnWin.mPosX = px;
		warnWin.mPosY = py;		
		warnWin.mSizeX = sx;
		warnWin.mSizeY = sy;
		
		warnWin.mWin = (KinderCoMessageThing * )syMalloc(sizeof(KinderCoMessageThing), "..\\..\\src\\Chao\\al_msg_warn.c", 198);
		AlMsgWinCreate(warnWin.mWin);
		AlMsgWinSetPos(warnWin.mWin, px + 16, py + 16, -1);
		AlMsgWinSetSize(warnWin.mWin, sx - 32, sy - 40);
	}
}
static void AlMsgWarnOpen(int winid) {
	AlMsgWinOpen(AL_MSGWarnKinderMessageArray[winid].mWin);
	AL_MSGWarnKinderMessageArray[winid].mOpen = 1;
}
static void AlMsgWarnAddLineC(int winid, const char* str)
{
	//TextColor[0] = (NJS_BGRA)0xFF;
	*(int*)0x1A267D0 = 0xFFFFFFFF;
	AlMsgWinAddLineC(
		AL_MSGWarnKinderMessageArray[winid].mWin,
		str,
		TextLanguage == 0);
}
static void AlMsgWarnWaitClose(int winid)
{
	__int16 v0; // dx
	Message380* v1; // eax

	v0 = AL_MSGWarnKinderMessageArray[winid].mWin->wordsLeftMaybe;
	v1 = &AL_MSGWarnKinderMessageArray[winid].mWin->pointerToSomething0x380->array[v0];
	AL_MSGWarnKinderMessageArray[winid].mWin->wordsLeftMaybe = v0 + 1;
	v1->Mode = 5;
	v1->ButtonPress = (Buttons)(Buttons_X | Buttons_A | Buttons_B);
	v1->ButtonOn = 0;
}
static void AlMsgWarnDelete(int winid)
{
	if (AL_MSGWarnKinderMessageArray[winid].mActive)
	{
		AlMsgWinDelete(AL_MSGWarnKinderMessageArray[winid].mWin);
		syFree(AL_MSGWarnKinderMessageArray[winid].mWin, "..\\..\\src\\Chao\\al_msg_warn.c", 213);

		AL_MSGWarnKinderMessageArray[winid].mActive = 0;
		*(int*)0x1A557C8 = 1;
	}
}