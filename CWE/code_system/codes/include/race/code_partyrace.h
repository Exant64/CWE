#pragma once

#include "../../../cwe_code.h"

#include <al_stage.h>
#include <al_race.h>
#include <al_chao_info.h>

class PartyRace : public Code {
public:
	using Code::Code;

	void Init() override;
    void OnFrame() override;
};

void PartyRace::Init() {
    // kills camera focus changes
    WriteData<5>((uint8_t*)0x00597468, uint8_t(0x90));

    // always jump over cheer logic
    WriteData((uint8_t*)0x0059739D, uint8_t(0xEB));

    // spawn displays for all 8 chao
    WriteData((uint8_t*)0x0055C0F4, uint8_t(0));
    WriteData((uint8_t*)0x0055C11F, uint8_t(8));

    // kill cam change sound
    WriteCall((void*)0x0059745C, nullsub_1);
}

void PartyRace::OnFrame() {
    // from lst code: hack to prevent timer from reaching 480
    *(uint8_t*)0x01A5CE14 = 50;

    if(NextChaoArea == CHAO_STG_RACE) {
        if(RaceMainType <= 1) {
            RaceMainType = 5;
            RaceLevel = 0;
        }
    }

    if(RaceMainType == 5) {
        DataArray(ChaoData*, pSelectedChaoSaveInfoList, 0x01A5CC00, 8);

        for(size_t i = 0; i < 8; ++i) {
            auto info = &ChaoSlots[i];

            if(info->data.Type) {
                pSelectedChaoSaveInfoList[i] = info;
            }
            else {
                pSelectedChaoSaveInfoList[i] = NULL;
            }
        }
    }
}