#pragma once
#include "../../cwe_code.h"

ONFRAME_CODE(QuickExit);

void QuickExit::OnFrame() {
    // intentionally not an & check (and intentionally not limited to Chao World)
    if (per[0]->on != (BTN_L | BTN_R | BTN_Y | BTN_START)) {
        return;
    }
    
    if (ssGameMode == MD_GAME_MAIN || ssGameMode == MD_GAME_PAUSE) {
        ssGameMode = MD_GAME_FADEOUT_EXIT;
    }
}