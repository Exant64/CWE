#pragma once

#include "stdafx.h"

#pragma pack(push, 8)
struct __declspec(align(16)) KarateMainExecData
{
	char gap0;
	char field_1;
	char field_2;
	char field_3;
	char field_4;
	char byte5;
	char byte6;
	char Round;
	char opponent;
	char field_9;
	char MenuSelection;
	char field_B;
	task* playerChao;
	task* pointerToSaveThing;
	task* PointerToOtherSaveThing;
	CHAO_SAVE_INFO* field_18;
	CHAO_SAVE_INFO* field_1C;
	int field_20;
	int field_24;
	int field_28;
	int field_2C;
	float field_30;
	int field_34;
	int field_38;
	task* field_3C;
};
#pragma pack(pop)

#pragma pack(push, 8)
struct __declspec(align(8)) KarateChaoExec_Data2
{
	__int16 char0;
	short word2;
	short flag;
	short word6;
	task* pointerToChao;
	void* dwordC;
	CHAO_SAVE_INFO* chaoDataPointer;
	int field_14;
	NJS_VECTOR* BaseTranslationPos;
	NJS_VECTOR* HeadTranslationPos;
	float SwimStat;
	float FlyStat;
	float RunStat;
	float PowerStat;
	int field_30;
	float field_34;
	float zeal;
	float StaminaStat;
	float LuckStat;
	float float44;
};
#pragma pack(pop)

DataPointer(KarateMainExecData*, KarateMainExec_Ptr, 0x01A5D148);