#include "stdafx.h"

#include "ALifeSDK_Functions.h"
#include "ninja_functions.h"
#include "ChaoMain.h"
#include "al_sandhole.h"
#include <vector>
#include "AL_ModAPI.h"
#include "alo_obakehead.h"

DataPointer(float, flt_1312CBC, 0x1312CBC);
DataArray(int, dword_19468A0, 0x19468A0, 4);

std::vector<std::pair<NJS_OBJECT*, NJS_TEXLIST*>> MaskObjObjectList;

bool IsAltDrawHead(int headgear) {
	return headgear == SA2BHat_Bucket ||
		headgear == SA2BHat_EmptyCan ||
		headgear == SA2BHat_FlowerPot ||
		headgear == SA2BHat_Pan;
}

void __cdecl ALO_ObakeHeadExecutor_Display_(task *a1)
{
	task *v1; // ebx
	float v28; // st6
	float a3; // ST04_4

	v1 = a1;
	taskwk* work = a1->twp;
	if (Scaletask_XYZ(a1, 2.5f, 2.5f, 2.0f))
	{
		njSetTexture(&AL_BODY);
		DoLighting(LightIndex);
		njPushMatrixEx();
		njTranslateEx(&work->pos);
		if (work->flag < 0)
			njTranslate(NULL, 0, -0.35f, 0);
		RotateY(work->ang.y);
		njPushMatrixEx();
		if (ALO_Field_Find_(v1, 1, 150))
			njControl3D |= 0x2400u;
		ALO_ObakeHeadDraw<true>(work->ang.x);
		njControl3D &= ~0x2400u;
		njPopMatrixEx();
		if (flt_1312CBC > (double)*(float *)&v1->UnknownA_ptr->field_30)
		{
			if (ALO_Field_Find_(v1, 1, 150))
			{
				v28 = 0.1f;
			}
			else
			{
				v28 = 0.4f;
			}
			a3 = v28;
			njTranslate(NULL, 0.0, a3, 0.0);
			njScale(NULL, 1.5, 0.7f, 1.5f);
		}
		njPopMatrixEx();
		DoLighting(LightIndexBackupMaybe);
	}
}

void ALO_ObakeHead_Init()
{
	WriteJump(ALO_ObakeHeadExecutor_Display, ALO_ObakeHeadExecutor_Display_);
}