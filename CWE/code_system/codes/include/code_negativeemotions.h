#pragma once
#include "../../cwe_code.h"
#include "al_behavior/al_intention.h"
#include "al_emotion.h"
#include <al_stage.h>
#include <Chao.h>
#include <FunctionHook.h>

PATCH_CODE(NegativeEmotions);

static FunctionHook<int, task*> ALBHV_DakkoDislike_t(0x566300);
static FunctionHook<int, task*> ALBHV_Eat_t(0x5607C0);
static FunctionHook<int, task*> ALBHV_Akanbe_t(0x5A1800);
static FunctionHook<int, task*> ALBHV_MesoMeso_t(0x59FA50);
static FunctionHook<int, task*> ALBHV_HungryDada_t(0x59EF40);
static FunctionHook<int, task*> ALBHV_Koke_t(0x562D50);

static void AngerCryAdd(task* tp, int base, float multi) {
    const auto personality = AL_EmotionGetValue(tp, EM_PER_AGRESSIVE);
    
    float personalityBaseScore = (1.f + personality / float(GET_GLOBAL()->EmotionPersonalityLimit)) / 2.f;
    AL_ScoreRandomize(&personalityBaseScore);
    AL_EmotionAdd(tp, (personalityBaseScore < 0.5f) ? EM_MD_SORROW : EM_MD_ANGER, base);

    const float persScale = NJM_MAX(10, abs(personality)) / float(GET_GLOBAL()->EmotionPersonalityLimit);
    AL_EmotionAdd(tp, (personality < 0) ? EM_MD_SORROW : EM_MD_ANGER, int(persScale * multi));
}

static int ALBHV_DakkoDislike_r(task* tp) {
    AL_BEHAVIOR* bhv = &GET_CHAOWK(tp)->Behavior;

    if(!bhv->Mode) {
        AngerCryAdd(tp,  int(9 + njRandom() * 9), 30);
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
        AngerCryAdd(tp,  int(njRandom() * 15), njRandom() * 105);
    }

    return ALBHV_Eat_t.Original(tp);
}

static int ALBHV_Akanbe_r(task* tp) {
    AL_BEHAVIOR* bhv = &GET_CHAOWK(tp)->Behavior;

    if(!bhv->Mode) {
        AngerCryAdd(tp, int(15 + njRandom()), 45);
    }

    return ALBHV_Akanbe_t.Original(tp);
}

static int ALBHV_MesoMeso_r(task* tp) {
    AL_BEHAVIOR* bhv = &GET_CHAOWK(tp)->Behavior;

    if(!bhv->Mode && njRandom() < 0.2f) {
        AngerCryAdd(tp, int(45 + njRandom() * 75),90);
    }

    return ALBHV_MesoMeso_t.Original(tp);
}

static int ALBHV_HungryDada_r(task* tp) {
    AL_BEHAVIOR* bhv = &GET_CHAOWK(tp)->Behavior;

    if(!bhv->Mode && njRandom() < 0.2f) {
        AngerCryAdd(tp, int(30 + njRandom() * 15), njRandom() * 90);
    }

    return ALBHV_HungryDada_t.Original(tp);
}

static int ALBHV_Koke_r(task* tp) {
    AL_BEHAVIOR* bhv = &GET_CHAOWK(tp)->Behavior;

    if(!bhv->Mode && njRandom() < 0.75f) {
        const float calm = NJM_MAX(10, -AL_EmotionGetValue(tp, EM_PER_CALM)) / 100.f;
        const float regain = NJM_MAX(10, -AL_EmotionGetValue(tp, EM_PER_REGAIN)) / 100.f;

        AL_EmotionAdd(tp, EM_MD_SURPRISE, int(30 + njRandom() * 15 + njRandom() * 90 * calm));
        AL_EmotionAdd(tp, EM_MD_PAIN, int(30 + njRandom() * 15 + njRandom() * 90 * regain));
    }

    return ALBHV_Koke_t.Original(tp);
}

void NegativeEmotions::Init() {
    ALBHV_HungryDada_t.Hook(ALBHV_HungryDada_r);
    ALBHV_MesoMeso_t.Hook(ALBHV_MesoMeso_r);
    ALBHV_Akanbe_t.Hook(ALBHV_Akanbe_r);
    ALBHV_Eat_t.Hook(ALBHV_Eat_r);
    ALBHV_DakkoDislike_t.Hook(ALBHV_DakkoDislike_r);

    ALBHV_Koke_t.Hook(ALBHV_Koke_r);
}