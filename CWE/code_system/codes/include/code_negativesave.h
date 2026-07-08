#pragma once

#include <ChaoMain.h>
#include <code_system/cwe_code.h>

ONFRAME_CODE(NegativeSave);

void NegativeSave::OnFrame() {
	// mostly a direct port but with negative texture/palette filename writing removed
    
    if(ssGameMode != 0) return;

    const auto vanillaSaveSuffix = 0x464C415F;
    const auto negativeSaveSuffix = 0x4B4C425F;

    if(ChaoSaveText == vanillaSaveSuffix) {
        if(SWDATA[0] & Buttons_X) {
            ChaoSaveText = negativeSaveSuffix;
        }
    }
    else if(ChaoSaveText == negativeSaveSuffix) {
        if(!(SWDATA[0] & Buttons_X)) {
            ChaoSaveText = vanillaSaveSuffix;
        }
    }
}