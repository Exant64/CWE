#pragma once
#include <cwe_api.h>

// the generic sprite array used throughout the menus, a couple of our menu entries use it too
DataArray(ChaoHudThingB, stru_11BA528, 0x11BA528, 30);

static void AL_OdeMenuSetNextStage(int stage) {
    if (AL_OdekakeMenuMaster_Data_ptr) {
        AL_OdekakeMenuMaster_Data_ptr->NextStage = stage;
    }
}

VoidFunc(AL_OdeMenuChangeStage, 0x57E680);

extern CWE_API_ODEKAKE_ENTRY OdekakeCustomizationEntry;
extern CWE_API_ODEKAKE_ENTRY OdakakeMoveEntry;
extern CWE_API_ODEKAKE_ENTRY OdekakeNameEntry;
extern CWE_API_ODEKAKE_ENTRY OdekakeGuestEntry;