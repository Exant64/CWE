#pragma once
struct CCL_INFO
{
	char kind;
	char form;
	char push;
	char damage;
	int attr;
	NJS_POINT3 center;
	float a;
	float b;
	float c;
	float d;
	int angx;
	int angy;
	int angz;
};

void CCL_Enable(ObjectMaster* a1, int a2);
void CCL_Disable(ObjectMaster* a1, int a2);