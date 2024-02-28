#include "stdafx.h"
#include "usercall.h"

int(*AL_EmotionGetValue)(ObjectMaster* tp, int emotion) = GenerateUsercallWrapper<int(*)(ObjectMaster* tp, int emotion)>(rEAX, 0x0054E930, rEAX, rECX);

void* __cdecl Chao_GetFeelingPtr(ObjectMaster* a1, int a2)
{
	AL_EMOTION* v2; // ecx

	v2 = &a1->Data1.Chao->pParamGC->Emotion;
	if ((unsigned int)a2 <= 7)
	{
		return ((unsigned __int8*)&v2->Joy + a2);
	}
	if ((unsigned int)a2 >= 8 && (unsigned int)a2 <= 18)
	{
		return ((unsigned __int16*)&v2->field_124 + a2);// tiredness if 9
	}
	if ((unsigned int)a2 >= 19 || (unsigned int)a2 <= 31)
	{
		return ((char*)&v2->Tiredness + a2 + 1);
	}
	return nullptr;
}


void AL_EmotionAdd(ObjectMaster* a1, int a2, int a3)
{
	void* ptr = Chao_GetFeelingPtr(a1, a2);
	if ((unsigned int)a2 <= 7)
	{
		int val = *(unsigned char*)ptr + a3;
		if (val > 200) val = 200;
		if (val < 0) val = 0;
		*(unsigned char*)ptr = val;
	}
	else if ((unsigned int)a2 >= 8 && (unsigned int)a2 <= 18)
	{
		*(short*)ptr += a3;
		if (*(short*)ptr > 10000)
			*(short*)ptr = 10000;
		if (*(short*)ptr < 0)
			*(short*)ptr = 0;
	}
	else {
		*(char*)ptr += a3;
		if (*(char*)ptr > 100)
			*(char*)ptr = 100;
		if (*(char*)ptr < -100)
			*(char*)ptr = -100;
	}
}

void AL_EmotionSet(ObjectMaster* a1, int a2, int a3)
{
	void* ptr = Chao_GetFeelingPtr(a1, a2);
	if ((unsigned int)a2 <= 7) {
		*(unsigned char*)ptr = a3;
	}
	else if ((unsigned int)a2 >= 8 && (unsigned int)a2 <= 18) {
		*(short*)ptr = a3;
	}
	else {
		*(char*)ptr = a3;
	}
}

