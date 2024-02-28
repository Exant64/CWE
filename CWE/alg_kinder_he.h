#pragma once

#include "alg_kinder_ortho.h"
#include "alg_kinder_bl.h"

#pragma pack(push, 8)
struct __declspec(align(4)) HealthCenter
{
	int gap0;
	int field_4;
	ObjectMaster* field_8;
	int field_C;
	int field_10;
	char openedMedicalChart;
	char medicalChartMenu;
	char field_16;
	char field_17;
	ObjectMaster* medicalChartChao_;
	int Chao_DisplaySub;
	int Chao_field_20;
	ObjectMaster* medicalChartChao;
	unsigned char gap28[20];
	KinderCoMessageThing* pkindercomessagething3C;
	unsigned char gap40[32];
	const char* dword60;
};
#pragma pack(pop)

void DrawMedicalChartText(const char* TextPtr, float XPos, float YPos, float TextBoxLength, float TextSize, char TextAlignment);

void __cdecl ConditionText(HealthCenter* a2);
FastcallFunctionPointer(void, AL_HealthCenter_SetMessage, (HealthCenter* a1, int a2), 0x058EB90);
void __fastcall InitDoctorHook(HealthCenter* a1, int a2);
void __fastcall DoctorMessage(HealthCenter* a1, int a2);

DataArray(ChaoHudThing, HealthCenter_MenuBlue, 0x008A1DC0, 20);
DataArray(ChaoHudThing, HealthCenter_MenuYellow, 0x008A2150, 20);
DataArray(ChaoHudThing, HealthCenter_MenuPink, 0x008A2480, 20);

void __cdecl sub_58F980(ObjectMaster *a1);
void DisplayHealthCenterMedicalChartCall();
void PurchaseGradesHook();
void __cdecl sub_58EA40(HealthCenter* a1);
extern __int16 word_8A2754[];
void alg_kinder_he_Init();