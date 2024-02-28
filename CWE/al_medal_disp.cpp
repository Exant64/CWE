#include "stdafx.h"
#include "al_medal_disp.h"
#include "ninja_functions.h"
#include "ChaoMain.h"
void AL_MedalDisplayExecutor_Display(ObjectMaster * a1)
{
	njPushMatrixEx();
	njSetTexture(&stru_1366AD4);
	njTranslateV(&a1->Data1.Entity->Position);
	njRotateY(MatrixBase, a1->Data1.Entity->Rotation.y);
	njCnkDrawObject(BigMedals[a1->Data1.Entity->field_6]);
	njPopMatrixEx();
}

void AL_MedalDisplayExecutor_Main(ObjectMaster * a1)
{
	a1->Data1.Entity->Rotation.y += 100;
}

ObjectMaster* AL_MedalDisplayExecutor_Load(int a1, NJS_VECTOR* position)
{
	ObjectMaster* loadedObj = LoadObject(2, "AL_MedalDisplayExecutor", AL_MedalDisplayExecutor_Main, LoadObj_Data1);
	loadedObj->DisplaySub = AL_MedalDisplayExecutor_Display;
	loadedObj->Data1.Entity->Position = *position;
	loadedObj->Data1.Entity->field_6 = a1;
	return loadedObj;
}
