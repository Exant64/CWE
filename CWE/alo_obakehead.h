#pragma once

#include "ninja_functions.h"
#include "ChaoMain.h"
#include "brightfixapi.h"
#include <vector>

extern std::vector<std::pair<NJS_OBJECT*, NJS_TEXLIST*>> MaskObjObjectList;

bool IsAltDrawHead(int headgear);
template <bool UseGardenModel = false, bool AltDrawCheck = true>
void ALO_ObakeHeadDraw(int headgear) {
	if (headgear >= 85)
	{
		njSetTexture(MaskObjObjectList[headgear - 85].second);
		chCnkDrawObject(MaskObjObjectList[headgear - 85].first);
	}
	else
	{
		njSetTexture(&AL_BODY);
		NJS_OBJECT** list = (NJS_OBJECT**)GetDllData("MaskObjObjectList");
		if (headgear >= SA2BHat_NormalEggShell && headgear < 85)
		{
			if (UseGardenModel) {
				BrightFixPlus_ShinyCheck(1);
				njTranslate(NULL, 0.0f, 0.4f, 0.0f);
				ColorEggModel((NJS_CNK_MODEL*)0x125CC14, headgear - SA2BHat_NormalEggShell);
			}
			else
				ColorEggModel(list[16]->chunkmodel, headgear - SA2BHat_NormalEggShell);
		}
		else if(list[headgear])
		{
			if (AltDrawCheck && IsAltDrawHead(headgear))
				njCnkDrawObject(list[headgear]);
			else
				chCnkDrawObject(list[headgear]);
		}
	}
}

void ALO_ObakeHead_Init();