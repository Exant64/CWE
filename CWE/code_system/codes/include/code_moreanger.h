#pragma once
#include "../../cwe_code.h"
#include <al_stage.h>
#include <Chao.h>
#include <FunctionHook.h>

PATCH_CODE(MoreAnger);

static FunctionHook<int, task*> ALBHV_DakkoDislike_t(0x566300);
static FunctionHook<int, task*> ALBHV_Eat_t(0x5607C0);
static FunctionHook<int, task*> ALBHV_Akanbe_t(0x5A1800);
static FunctionHook<int, task*> ALBHV_MesoMeso_t(0x59FA50);
static FunctionHook<int, task*> ALBHV_HungryDada_t(0x59EF40);

static int ALBHV_DakkoDislike_r(task* tp) {
    AL_BEHAVIOR* bhv = &GET_CHAOWK(tp)->Behavior;

    if(!bhv->Mode) {
        const float aggressive = max(10, AL_EmotionGetValue(tp, EM_PER_AGRESSIVE)) / 100.f;

        AL_EmotionAdd(tp, EM_MD_ANGER, 3 * int(3 + njRandom() * 3 + aggressive * 10));
    }

    return ALBHV_DakkoDislike_t.Original(tp);
}

static int ALBHV_Eat_r(task* tp) {
    enum {
        MD_SUTERU_START = 9
    };

    AL_BEHAVIOR* bhv = &GET_CHAOWK(tp)->Behavior;

    // start mode of fruit throwing away
    if(bhv->Mode == MD_SUTERU_START) {
        const float aggressive = max(10, AL_EmotionGetValue(tp, EM_PER_AGRESSIVE)) / 100.f;

        AL_EmotionAdd(tp, EM_MD_ANGER, 3 * int(njRandom() * 5 + njRandom() * 35 * aggressive));
    }

    return ALBHV_Eat_t.Original(tp);
}

static int ALBHV_Akanbe_r(task* tp) {
    AL_BEHAVIOR* bhv = &GET_CHAOWK(tp)->Behavior;

    if(!bhv->Mode) {
        const float aggressive = max(10, AL_EmotionGetValue(tp, EM_PER_AGRESSIVE)) / 100.f;

        AL_EmotionAdd(tp, EM_MD_ANGER, 3 * int(5 + njRandom() * 15 * aggressive));
    }

    return ALBHV_Akanbe_t.Original(tp);
}

static int ALBHV_MesoMeso_r(task* tp) {
    AL_BEHAVIOR* bhv = &GET_CHAOWK(tp)->Behavior;

    if(!bhv->Mode && njRandom() < 0.2f) {
        const float aggressive = max(10, AL_EmotionGetValue(tp, EM_PER_AGRESSIVE)) / 100.f;

        AL_EmotionAdd(tp, EM_MD_ANGER, 3 * int(15 + njRandom() * 25 + 30 * aggressive));
    }

    return ALBHV_MesoMeso_t.Original(tp);
}

static int ALBHV_HungryDada_r(task* tp) {
    AL_BEHAVIOR* bhv = &GET_CHAOWK(tp)->Behavior;

    if(!bhv->Mode && njRandom() < 0.2f) {
        const float aggressive = max(10, AL_EmotionGetValue(tp, EM_PER_AGRESSIVE)) / 100.f;

        AL_EmotionAdd(tp, EM_MD_ANGER, 3 * int(10 + njRandom() * 5 + njRandom() * 30 * aggressive));
    }

    return ALBHV_HungryDada_t.Original(tp);
}

void MoreAnger::Init() {
    ALBHV_HungryDada_t.Hook(ALBHV_HungryDada_r);
    ALBHV_MesoMeso_t.Hook(ALBHV_MesoMeso_r);
    ALBHV_Akanbe_t.Hook(ALBHV_Akanbe_r);
    ALBHV_Eat_t.Hook(ALBHV_Eat_r);
    ALBHV_DakkoDislike_t.Hook(ALBHV_DakkoDislike_r);
}