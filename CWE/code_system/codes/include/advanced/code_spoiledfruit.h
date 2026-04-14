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
    if(NextChaoArea <= 3) {
        return;
    }

    ChaoObjectData* objData = (ChaoObjectData*)ChaoFruitSlots;
    for (size_t i = 0; i < 24; i++) {
        if(objData[i].Age != 6) continue;

        // turn certain fruit into spoiled variant
        if (objData[i].Type >= 0 && objData[i].Type <= 4) {
            objData[i].Type += 24;
        }
    }
}