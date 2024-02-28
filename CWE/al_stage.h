#pragma once
bool AL_IsGarden();
int AL_GetStageNumber();
//credit to shad for the original enum that had all stages
enum eCHAO_STAGE_NUMBER
{
    CHAO_STG_NONE = 0,
    CHAO_STG_NEUT = 1,
    CHAO_STG_HERO = 2,
    CHAO_STG_DARK = 3,
    CHAO_STG_RACE = 4,
    CHAO_STG_ENTRANCE = 5,
    CHAO_STG_KINDER = 6,
    CHAO_STG_LOBBY = 7,
    CHAO_STG_RACE_2P = 8,
    CHAO_STG_STADIUM = 9,
    CHAO_STG_KARATE = 10,
    CHAO_STG_KARATE_2P = 11,
    CHAO_STG_ODEKAKE = 12
};