#include "stdafx.h"

#include "ninja_functions.h"
#include "ALifeSDK_Functions.h"
NJS_TEXNAME JOBJ_TEXNAMES[32];
NJS_TEXLIST JOBJ_TEXLIST = {JOBJ_TEXNAMES, 32};

const char* JoyCarryFiles[] = 
{
	"squashthingleft",
	"squashthingright",
	"task2_obj1",
	"task3_obj1",
	"task3_obj2",
	"task4_obj1",
	"task4_obj2",
	"task4_obj3",
	"task4_obj4",
	"task4_obj5",
	"task4_obj6",
	"task4_obj7",
	"task5_obj1",
	"task6_obj1",
	"task7_obj1",
	"tasklast_obj1",
	"GbaStatusDisplay",
	"lever1",
	"lever1_omo",
	"objBase1",
	"objBase2",
	"pipe1",
	"Clock1",
	"Clock2",
	"Clock3"
};

NJS_OBJECT* JoyCarryModels[32];

void JoyCarry_Display(ObjectMaster* a1)
{
	SetShaders(1);
	njPushMatrixEx();
	njTranslate(NULL, 0, 7, 0);
	njSetTexture(&JOBJ_TEXLIST);
	for (int i = 0; i < 25; i++)
		if (JoyCarryModels[i])
			chCnkDrawObject(JoyCarryModels[i]);
		else PrintDebug("%d \n", i);
	njPopMatrixEx();
}
void JoyCarry_Main(ObjectMaster *a1){}
void JoyCarry_Init()
{
	for (int i = 0; i < 25; i++)
	{
		//std::string str(JoyCarryFiles[i]);
		//str += ".sa2mdl";
		//ModelInfo* info = new ModelInfo(str.c_str());
		//JoyCarryModels[i] = info->getmodel();
	}
	LoadTextureList((char*)"JOYobj", &JOBJ_TEXLIST);
	LoadObject(5, "JoyCarryTest", JoyCarry_Main, LoadObj_Data1)->DisplaySub = JoyCarry_Display;
}