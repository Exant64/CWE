#pragma once
struct ALR_Camera_DebugFunc
{
	const char* name;
	struct ALR_Camera_DebugFunc* nextMenu;
	void(__cdecl *function)(ObjectMaster *);
};
void ALR_Camera_Edit_Load();
void DrawTest();