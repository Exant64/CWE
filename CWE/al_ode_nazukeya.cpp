#include "stdafx.h"

#include "alg_kinder_he.h"
#include "al_odekake.h"
#include "ninja_functions.h"
#include <cstdio>

char NazukeyaBuff[0x60 + 4 + sizeof(AL_NAME)];
ObjectMaster* nazukeyaObj = 0;
FunctionPointer(void, sub_5827A0, (int a1), 0x5827A0);
void __cdecl Nazukeya_Main(ObjectMaster *a1)
{
	sub_582F60((char*)NazukeyaBuff);
}
void __cdecl Nazukeya_Display(ObjectMaster *a1)
{
	sub_5827A0((int)NazukeyaBuff);
}

//asdsd
FunctionPointer(int, njReleaseTexture, (NJS_TEXLIST* arg0), 0x0077F9F0);
void AL_OdekakeName(ODE_MENU_MASTER_WORK* a1)
{
	int v4;
	switch (a1->mode)
	{
	case 0:
		LoadTextureList((char*)"al_stg_kinder_ad_tex", (NJS_TEXLIST*)0x011D2ACC);
		a1->mode++;
		memset(NazukeyaBuff, 0, 0x60 + 4 + sizeof(AL_NAME));
		NazukeyaBuff[80] = 1;
		NazukeyaBuff[81] = 1;
		memcpy(&NazukeyaBuff[0x60], GBAManager_GetChaoDataPointer()->Name, sizeof(AL_NAME));
		NazukeyaBuff[0x4F] = 0;
		v4 = strlen((const char*)(&NazukeyaBuff[0x60]));
		NazukeyaBuff[82] = v4;
		NazukeyaBuff[83] = v4;
		*(short*)&NazukeyaBuff[84] = 2;
		*(short*)& NazukeyaBuff[86] = 7;
		*(short*)& NazukeyaBuff[88] = 16;
		*(short*)& NazukeyaBuff[90] = 0;
		*(short*)& NazukeyaBuff[92] = 0;
		nazukeyaObj = LoadObject(4, "Nazukeya", Nazukeya_Main, (LoadObj)0);
		nazukeyaObj->field_1C = Nazukeya_Display;
		//obj->field_1C 
		break;
	case 1:	
		if (!NazukeyaBuff[80]) {
			nazukeyaObj->MainSub = DeleteObject_;

			if (NazukeyaBuff[0x51] != 2) {//not "cancel"
				memset(GBAManager_GetChaoDataPointer()->Name, 0, sizeof(AL_NAME));
				memcpy(GBAManager_GetChaoDataPointer()->Name, &NazukeyaBuff[0x60], strlen(&NazukeyaBuff[0x60]));
			}
			//njReleaseTexture((NJS_TEXLIST*)0x011D2ACC);
			a1->mode++;
		}
		break;
	case 2:
		AL_OdekakeMenuMaster_Data_ptr->NextStage = 0;
		if (AL_OdekakeMenuMaster_Data_ptr)
		{
			void(__cdecl * v0)(ODE_MENU_MASTER_WORK*); // eax
			v0 = AL_OdekakeMenuMaster_Data_ptr->mfStageExit;
			if (v0)
			{
				//	v0(AL_OdekakeMenuMaster_Data_ptr);
			}
			AL_OdekakeMenuMaster_Data_ptr->PreStage = AL_OdekakeMenuMaster_Data_ptr->CurrStage;
			AL_OdekakeMenuMaster_Data_ptr->CurrStage = AL_OdekakeMenuMaster_Data_ptr->NextStage;
			AL_OdekakeMenuMaster_Data_ptr->mode = 0;
			AL_OdekakeMenuMaster_Data_ptr->timer = 0;
			AL_OdekakeMenuMaster_Data_ptr->subtimer = 0;
			AL_OdekakeMenuMaster_Data_ptr->state = 0;
			AL_OdekakeMenuMaster_Data_ptr->cursorX = 0;
			AL_OdekakeMenuMaster_Data_ptr->cursorY = 2;
			AL_OdekakeMenuMaster_Data_ptr->EndFlag = 0;
			AL_OdekakeMenuMaster_Data_ptr->mfStageExit = 0;
			AL_OdekakeMenuMaster_Data_ptr->mpStageWork = 0;
		}
		break;
	}
}