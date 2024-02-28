#pragma once
#include "SA2ModLoader.h"
#include "al_msg_font.h"

#pragma pack(push, 8)
struct __declspec(align(4)) MessageField0
{
	MessageFontThing* dword0;
	float float4;
	float float8;
	float xSize;
	float ySize;
	int field_14;
	float field_18;
};
#pragma pack(pop)


#pragma pack(push, 8)
struct MessageField0Array
{
	MessageField0 array[9];
};
#pragma pack(pop)

#pragma pack(push, 8)
struct __declspec(align(4)) Message380
{
	int Mode;
	Buttons ButtonPress;
	int ButtonOn;
	float floatC;
	float float10;
	float float14;
	int dword18;
};
#pragma pack(pop)


struct Message380Array
{
	Message380 array[32];
};


#pragma pack(push, 8)
struct __declspec(align(4)) KinderCoMessageThing
{
	MessageField0Array* gap0;
	Message380Array* pointerToSomething0x380;
	__int16 someKindaCount;
	__int16 wordsLeftMaybe;
	float mPosZ;
	float mPosX;
	float mPosY;
	float mSizeX;
	float mSizeY;
	float field_20;
	float field_24;
	__int16 field_28;
	char enabled;
	char field_2B;
	int ContinueDotColor;
};
#pragma pack(pop)

FunctionPointer(int, AlMsgWinDelete, (KinderCoMessageThing* self), 0x00579C40);
void AlMsgWinOpen(KinderCoMessageThing* self);
void AlMsgWinCreate(KinderCoMessageThing* self);
void AlMsgWinAddLineC(KinderCoMessageThing* self, const char* cstr, int ja);
void AlMsgWinSetPos(KinderCoMessageThing* self, float px, float py, float pz);
void AlMsgWinSetSize(KinderCoMessageThing* self, float sx, float sy);

void MsgDialog(KinderCoMessageThing* a1);