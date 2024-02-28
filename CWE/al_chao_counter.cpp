#include "stdafx.h"
#include <Chao.h>
#include "ninja_functions.h"
#include "al_texlist.h"
#include "alg_kinder_ortho.h"
#include "ALifeSDK_Functions.h"

DataArray(ChaoHudThingB, StatText, 0x13128B0, 10);
void AL_ChaoCounterDisp(ObjectMaster* a1)
{
	if (GameState == GameStates_Pause && !(MenuButtons_Held[0] & Buttons_X && MenuButtons_Held[0] & Buttons_Y))
	{
		int shaderBackup = *(int*)0x01A5579C;
		NJS_QUAD_TEXTURE v9;
		v9.u1 = 0.0;
		v9.u2 = 1.0;
		v9.v2 = 1.0;
		v9.v1 = 0.0;
		njSetTexture(&AL_ITEM_TEXLIST);
		sub_781CB0(1);
		*(unsigned int*)0x1A54FF0 = 0xFFFFFFFF;
		njSetTextureNum(0, 0, 0, 0);
		v9.x1 = 0;
		v9.x2 = 20;
		v9.y1 = 70 - 7;
		v9.y2 = 90 - 7;
		DrawQuadTexture((int)&v9, 1);

		*(int*)0x1AED260 = 0;
		int chaoCount = *(int*)0x1DC0F80;
		float v12 = 30;
		float v16 = 400;
		SetChaoHUDThingBColor(1, 1.0, 1.0, 1.0);
		DrawChaoHudThingB(&StatText[chaoCount / 10], v12, v16, -1.5, 1.0, 1.0, 0, -1);
		float v40 = v12 + 12;
		DrawChaoHudThingB(&StatText[chaoCount % 10], v40, v16, -1.5, 1.0, 1.0, 0, -1);

		SetShaders(shaderBackup);
	}
}

void AL_ChaoCounterCreate() {
	ObjectMaster* tp = LoadObject(4, "AL_ChaoCounter", [] (ObjectMaster*) {}, (LoadObj)0);

	tp->DisplaySub = AL_ChaoCounterDisp;
}