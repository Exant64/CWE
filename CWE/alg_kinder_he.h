#pragma once

#include "alg_kinder_ortho.h"
#include "alg_kinder_bl.h"

struct HealthCenter {
	int gap0;
	int field_4;
	task* field_8;
	int field_C;
	int field_10;
	char openedMedicalChart;
	char medicalChartMenu;
	char field_16;
	char field_17;
	task* medicalChartChao_;
	task_exec Chao_DisplaySub;
	task_exec Chao_field_20;
	task* medicalChartChao;
	int gap28;
	float field_2C;
	float field_30;
	float field_34;
	int field_38;
	KinderCoMessageThing* pkindercomessagething3C;
	unsigned char gap40[32];
	const char* dword60;
	task* pMedicine;
};

void DrawMedicalChartText(const char* TextPtr, float XPos, float YPos, float TextBoxLength, float TextSize, char TextAlignment);

void alg_kinder_he_Init();