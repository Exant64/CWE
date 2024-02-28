#pragma once

enum {
	PMSG_FLAG_SELECTION = BIT_0,
	PMSG_FLAG_CENTER = BIT_1, //some sort of centering thing based on the scale calculated with "SCALE_THING"
	PMSG_FLAG_USE_STRING = BIT_2,
	PMSG_FLAG_SCALE_THING = BIT_3 //limits size to whats specified in fontSize, if its higher than that it gets smaller to fit
};

#pragma pack(push, 8)
struct __declspec(align(4)) MessageFontThing
{
	signed __int16 signed0;
	__int16 gap2;
	__int16 field_4;
	__int16 field_6;
	signed __int16* psigned8;
};
#pragma pack(pop)

#pragma pack(push, 8)
struct AL_KinderPMessage
{
	char a;
	char flags;
	char field_2;
	char field_3;
	int color;
	int field_8;
	int msgID;
	int field_10;
	float posX;
	float posY;
	float posZ;
	float field_20;
	float fontSize;
	const char* string;
};
#pragma pack(pop)

void AlMsgFontCreateCStr(int a1, int a2, int a3, int a4);
void DrawFontThing(__int16* a1, float a2, float a3, float a4);
void AlMsgFontDrawRegionScale2(signed int a1, MessageFontThing* a2, float a3, float a4, float a5, float a6, float a7, float a8, float a9);
void AlMsgFontDelete(MessageFontThing* a1);

ObjectMaster* AL_KinderPMessageExec_Load(ObjectMaster* a1, AL_KinderPMessage* a2);