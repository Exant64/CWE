#include "stdafx.h"
#include "SA2ModLoader.h"
#include "alr_camera_edit.h"
#include "debug.h"
#include "gx.h"
#include "ninja_functions.h"
#include "ChaoMain.h"

DataArray(ALR_Camera_DebugFunc, DebugFunctions, 0x01305D50,16);
ALR_Camera_DebugFunc* currentDebugArray = DebugFunctions;


void DrawTest()
{
	sub_42C170();
	njSetTexture(&AL_EYE_TEXLIST);
	njSetTextureNum(0, 0, 0, 0);
	//GXSetBlendMode(5, 4, 1);
	sub_583C60();
	//DrawChaoHudThing((ChaoHudThing*)0x8A3670, 0xFFFFFFFF);
	//return;
	GXBegin(GX_TRIANGLES, 3);
	GXPosition3f32(1, 0.25f, -1);
	GXColor1u32(0xFFFFFFFF);
	GXTexCoord2f32(0, 0);
	GXPosition3f32(0.5f, 0.25f, -1);
	GXColor1u32(0xFFFFFFFF);
	GXTexCoord2f32(0, 0);
	GXPosition3f32(0.75f, -0.25f, -1);
	GXColor1u32(0xFFFFFFFF);
	GXTexCoord2f32(0, 0);
	GX_End();
}

void ALR_Camera_Edit_Disp(ObjectMaster* a1)
{
	//DrawTest();
	//return;
	if(currentDebugArray[0].name)
		njPrintC(NJM_LOCATION(0, 0), currentDebugArray[0].name);
	for(int i = 0; i < 16; i ++)
	{
		
		if (currentDebugArray[i].name) {
			//if (a1->Data1.Entity->field_2 == i)
				//PrintDebug(">>");
			
			//PrintDebug("\n");
		}
	}
	//DrawDebugStrings();
}



void ALR_Camera_Edit_Main(ObjectMaster *a1)
{
	if (ControllerPointers[0]->press & Buttons_Up)
		a1->Data1.Entity->field_2--;
	if (a1->Data1.Entity->field_2 < 0)
		a1->Data1.Entity->field_2 = 0;
	if (ControllerPointers[0]->press & Buttons_Down)
		a1->Data1.Entity->field_2++;
	if (a1->Data1.Entity->field_2 > 16)
		a1->Data1.Entity->field_2 = 0;
	if (ControllerPointers[0]->press & Buttons_B)
	{
		if (currentDebugArray[a1->Data1.Entity->field_2].name)
		{
			if (currentDebugArray[a1->Data1.Entity->field_2].function)
			{
				if(currentDebugArray == DebugFunctions && a1->Data1.Entity->field_2 == 2)
					currentDebugArray[a1->Data1.Entity->field_2].function(0);
				else 
					currentDebugArray[a1->Data1.Entity->field_2].function(a1);
			}
			if (currentDebugArray[a1->Data1.Entity->field_2].nextMenu)
				currentDebugArray = currentDebugArray[a1->Data1.Entity->field_2].nextMenu;
			a1->Data1.Entity->field_2 = 0;
		}
			
		
	}
}
void ALR_Camera_Edit_Load()
{
	//ObjectMaster* alrCamEdit = LoadObject(1, "ALR_Camera_Editor", ALR_Camera_Edit_Main, LoadObj_Data1);
	ObjectMaster* alrCamEdit = LoadObject(2, "ALR_CameraEditorExecutor", 0, LoadObj_Data1);
	alrCamEdit->field_1C = ALR_Camera_Edit_Disp;
	DebugFunctions[2].function = 0;
}