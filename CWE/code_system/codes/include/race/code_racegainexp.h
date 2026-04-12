#pragma once

#include <code_system/cwe_code.h>
#include <FunctionHook.h>
#include <al_race.h>
#include <al_parameter.h>
#include <Chao.h>

PATCH_CODE(RaceGainExp);

FunctionHook<void> sub_533090_t(0x533090);
FunctionHook<void> sub_5396C0_t(0x5396C0);

struct ChaoRacePointer {
    int position;
    int finishedRace;
    int displayPosition;
    int displayUpdateTimer;
    ObjectMaster *pChaoObject;
};

DataArray(ChaoRacePointer, stru_1DCFAE0, 0x1DCFAE0, 8);

static bool CanRunStatGain() {
    // only in beginner/jewel race
    if(RaceMainType > 1) return false;

    // safety check for first chao
    if(!stru_1DCFAE0[0].pChaoObject) return false;

    return true;
}

static bool IsStatEligibleForGain(const size_t i) {
    if(i == 4) return RaceMainType <= 4;
    
    if(RaceSubType >= 8) return true;

    switch (i) {
        case 0: 
            return RaceSubType == 0 || RaceSubType == 4;
        case 1:
            return RaceSubType == 1 || RaceSubType == 5;
        case 2:
            return RaceSubType == 2 || RaceSubType == 6;
        case 3:
            return RaceSubType == 3 || RaceSubType == 7;
    }

    return false;
}

static void sub_533090_r() {
    sub_533090_t.Original();

    if(!CanRunStatGain()) return;

    // timer, only happen once a second
    if(*(Uint8*)0x1A0FBD2 != 0) return;

    const auto pParam = GET_CHAOPARAM(stru_1DCFAE0[0].pChaoObject);

    for(size_t i = 0; i < 5; ++i) {
        if(!IsStatEligibleForGain(i)) continue;
        
        // doing this to prevent overflow
        int fract = int(pParam->StatFractions[i]);

        if(fract < 200) {
            fract += 2;
            
            pParam->StatFractions[i] = fract;
        }
    }
}

static void sub_5396C0_r() {
    sub_5396C0_t.Original();

    if(!CanRunStatGain()) return;

    const auto pParam = GET_CHAOPARAM(stru_1DCFAE0[0].pChaoObject);

    for(size_t i = 0; i < 5; ++i) {
        int fract = pParam->StatFractions[i];

        while(fract >= 100) {
            fract -= 100;

            if(++pParam->StatLevels[i] >= 99) {
                pParam->StatLevels[i] = 99;
            }

            sub_535E90(pParam, i);
        }

        pParam->StatFractions[i] = fract;
    }
}

void RaceGainExp::Init() {
    sub_533090_t.Hook(sub_533090_r);
    sub_5396C0_t.Hook(sub_5396C0_r);
}