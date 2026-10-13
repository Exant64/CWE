#pragma once

#include <al_world.h>
#include <ChaoMain.h>

#include "../../../cwe_code.h"

class SpoiledFruits : public Code {
	public:
		using Code::Code;
		void OnFrame() override;
};

void SpoiledFruits::OnFrame() {
    // why? (this is ported from the LST)
    if(ChaoNextStageNumber <= 3) {
        return;
    }

    ITEM_SAVE_INFO* objData = AL_GetCurrGardenInfo()->fruit;
    for (size_t i = 0; i < 24; i++) {
        if(objData[i].nbVisit != 6) continue;

        // turn certain fruit into spoiled variant
        if (objData[i].kind >= 0 && objData[i].kind <= 4) {
            objData[i].kind += 24;
        }
    }
}