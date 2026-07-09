#include "stdafx.h"
#include <Chao.h>
#include "ninja_functions.h"
#include "al_texlist.h"
#include "alg_kinder_ortho.h"
#include "ALifeSDK_Functions.h"
#include <ChaoMain.h>

DataArray(CHS_BILL_INFO, StatText, 0x13128B0, 10);

static void AL_ChaoCounterDisplayer(task* tp) {
	if (ssGameMode != MD_GAME_PAUSE) {
		return;
	} 
	
	if((SWDATA[0] & BTN_X) || (SWDATA[0] & BTN_Y)) {
		return;
	}

	int shaderBackup = *(int*)0x01A5579C;

	NJS_QUAD_TEXTURE quadTex;
	quadTex.x1 = 0;
	quadTex.x2 = 20;
	quadTex.y1 = 70 - 7;
	quadTex.y2 = 90 - 7;
	quadTex.u1 = 0;
	quadTex.u2 = 1;
	quadTex.v2 = 1;
	quadTex.v1 = 0;

	njSetTexture(&AL_ITEM_TEXLIST);
	sub_781CB0(1);
	*(unsigned int*)0x1A54FF0 = 0xFFFFFFFF;
	njSetTextureNum(0, 0, 0, 0);

	DrawQuadTexture((int)&quadTex, 1);

	// some sort of blending value in chDrawBillBoardSR?
	*(int*)0x1AED260 = 0;

	const int chaoCount = nbWorldEntry[0];
	const float x_pos = 30;
	const float y_pos = 400;

	chSetBillboardColor(1, 1, 1, 1);
	chDrawBillboardSR(&StatText[chaoCount / 10], x_pos, y_pos, -1.5, 1.0, 1.0, 0, -1);
	chDrawBillboardSR(&StatText[chaoCount % 10], x_pos + 12, y_pos, -1.5, 1.0, 1.0, 0, -1);

	SetShaderType(shaderBackup);
}

void AL_ChaoCounterCreate() {
	task* tp = CreateElementalTask(0, LEV_4, [] (task*) {}, "AL_ChaoCounter");

	tp->disp = AL_ChaoCounterDisplayer;
}