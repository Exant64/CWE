#include "stdafx.h"

#include "alg_kinder_he.h"
#include "al_odekake.h"
#include "ninja_functions.h"
#include <cstdio>

#include "al_ode_menu.h"
#include "al_texlist.h"

static ChaoHudThingB NameMenuSprites[] = {
	{1, 128, 32, 0, 0.01f, 0.5f, 0.5f, &NAME_ODE_TEXLIST, 0}, //text
	{1, 128, 32, 136.0f / 256.0f, 0.01f, 1, 0.5f, &NAME_ODE_TEXLIST, 0}, //grey text
	{1, 32, 32, 0.01f, 0.01f, 0.99f, 1, &NAME_ODE_TEXLIST, 1} //icon
};

static void AL_OdekakeName(ODE_MENU_MASTER_WORK* a1);

CWE_API_ODEKAKE_ENTRY OdekakeNameEntry = { AL_OdekakeName, nullptr, ODE_FLAGS_REQUIRE_CHAO, &NameMenuSprites[2], &NameMenuSprites[0], &NameMenuSprites[1], nullptr, nullptr };

char NazukeyaBuff[0x60 + 4 + sizeof(AL_NAME)];
ObjectMaster* nazukeyaObj = 0;
FunctionPointer(void, sub_5827A0, (int a1), 0x5827A0);

static void Nazukeya_Main(ObjectMaster *a1) {
	sub_582F60((char*)NazukeyaBuff);
}

static void Nazukeya_Display(ObjectMaster *a1) {
	sub_5827A0((int)NazukeyaBuff);
}

//asdsd
FunctionPointer(int, njReleaseTexture, (NJS_TEXLIST* arg0), 0x0077F9F0);
static void AL_OdekakeName(ODE_MENU_MASTER_WORK* a1) {
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
		AL_OdeMenuSetNextStage(0);
		AL_OdeMenuChangeStage();
		break;
	}
}