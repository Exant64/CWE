#include "stdafx.h"

#include "ALifeSDK_Functions.h"
#include "ninja_functions.h"
#include "ChaoMain.h"
#include "al_sandhole.h"
#include <vector>
#include "AL_ModAPI.h"
#include "alo_obakehead.h"

DataPointer(float, flt_1312CBC, 0x1312CBC);

std::vector<std::pair<NJS_CNK_OBJECT*, NJS_TEXLIST*>> MaskObjObjectList;

bool IsAltDrawHead(int headgear) {
	return headgear == ChaoHat_Bucket ||
		headgear == ChaoHat_EmptyCan ||
		headgear == ChaoHat_FlowerPot ||
		headgear == ChaoHat_Pan;
}

void ALO_ObakeHeadDisplayer_r(task *tp) {
	taskwk* work = tp->twp;
	
	if (AL_IsOnScreen2(tp, 2.5f, 2.0f)) {
		njSetTexture(&AL_BODY);
		DoLighting(LightIndex);

		njPushMatrixEx();
		njTranslateEx(&work->pos);
		if (work->flag < 0) {
			njTranslate(NULL, 0, -0.35f, 0);
		}
		RotateY(work->ang.y);
		njPushMatrixEx();
		if (AL_IsHitKindWithNum(tp, 1, CI_KIND_AL_SHADOW))
			_nj_control_3d_flag_ |= 0x2400u;
		ALO_ObakeHeadDraw<true>(work->ang.x);
		_nj_control_3d_flag_ &= ~0x2400u;
		njPopMatrixEx();

		if (RenderFix_IsEnabled() && flt_1312CBC > GET_ALW_ENTRY_WORK(tp)->CamDist) {
			float y;

			if (AL_IsHitKindWithNum(tp, 1, CI_KIND_AL_SHADOW)) {
				y = 0.1f;
			}
			else {
				y = 0.4f;
			}
			
			njTranslate(NULL, 0, y, 0);
			njScale(NULL, 1.5, 0.7f, 1.5f);
			rfapi_core->pDraw->AL_ShadowDraw();
		}

		njPopMatrixEx();
		DoLighting(LightIndexBackupMaybe);
	}
}

void ALO_ObakeHead_Init() {
	WriteJump((void*)0x54A0F0, ALO_ObakeHeadDisplayer_r);
}