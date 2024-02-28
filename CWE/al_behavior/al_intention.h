#pragma once

#pragma pack(push, 8)
struct __declspec(align(4)) CHAO_GLOBAL
{
	int CalcBuyo;
	int DispWorldEntry;
	int DispOnScreen;
	int DispScreenFrame;
	int DispField;
	unsigned __int16 InitMulPotentialSkill;
	unsigned __int16 InitBaseLife;
	unsigned __int16 InitMulLife;
	unsigned __int16 InitMulForm;
	unsigned __int16 InitMulAttr;
	__int16 field_1E;
	float WalkAcc;
	float WalkSlowAcc;
	float RunAcc;
	float BoundFriction;
	float BoundFloor;
	unsigned __int16 SkillLimit;
	unsigned __int16 LifeMaxLimit;
	char EmotionMoodLimit;
	char field_39;
	unsigned __int16 EmotionStateLimit;
	char EmotionPersonalityLimit;
	char field_3D;
	char field_3E;
	char field_3F;
	float DeformLimit;
	float GrowthLimit;
	float GrowthMayuPoint;
	float NormalBoundary;
	float NeutralBoundary;
	float AddGrowthTime;
	float TimeAddGrowthChild;
	float TimeAddGrowthAdult;
	float TimeAddAttrChild;
	float TimeAddAttrAdult;
	float PartsSetProb;
	float PartsRemoveProb;
	float CamDistShadowCutLev1;
	float CamDistShadowCutLev2;
	float CamDistShadowCutLev3;
	float CamDistShadowCutLev4;
	float CamDistShadowCutLev5;
	unsigned int MayuWaitTime;
	unsigned int MayuFadeInTime;
	unsigned int MayuTypeChageTime;
	unsigned int MayuFadeOutTime;
	unsigned int MayuRelaxTime;
	float MayuExpandRatio;
	float MayuShrinkRatio;
	float MayuInitAlpha;
	__int16 SucceedBoundaryUserLike;
	unsigned __int16 SucceedBoundaryStress;
	unsigned __int16 SucceedBoundaryCondition;
	unsigned __int16 SucceedBoundaryMood;
	float FruitAddGrowthChild;
	float FruitAddGrowthAdult;
	unsigned __int16 FruitAddStaminaChild;
	unsigned __int16 FruitAddStaminaAdult;
	int FruitAddRecoverLife;
	int FruitAddLifeMax;
	int FruitAddUserLike;
	float FruitAddAttrChild;
	float FruitAddAttrAdult;
	float FruitDislikeSubAttr;
	float FruitPakuriSubAttr;
	float MinimalAddFormChild;
	float MinimalAddFormAdult;
	float MinimalAddAttrChild;
	float MinimalAddAttrAdult;
	float MinimalAddGrowthChild;
	float MinimalAddGrowthAdult;
	int MinimalAddUserLike;
	float DakkoAddAttr;
	float DamageSubAttr;
	float NadeAddAttr;
	float ThrownSubAttr;
	int DamageAddLife;
	int DamageAddLifeMax;
	unsigned __int16 NestFlowerInterval;
	unsigned __int16 nbNestFlower;
	float NestRange;
	unsigned __int16 NestWaitTime;
	unsigned __int16 SeedClock;
	unsigned __int16 SaplingClock;
	unsigned __int16 TreeClock;
	unsigned __int16 WaterSub;
	__int16 field_11A;
	float TreeFruitAddGrowth;
	unsigned __int16 FruitMaxNum;
	unsigned __int16 KwSubFear;
	unsigned __int16 KwAddDistance;
	unsigned __int16 SkillWalk;
	unsigned __int16 SkillRun;
	unsigned __int16 SkillRun2;
	float SkillRunAccBase;
	float SkillRunAccRatio;
	unsigned __int16 SkillSwimInukaki;
	unsigned __int16 SkillSwimBataashi;
	unsigned __int16 SkillSwimCrawl;
	__int16 field_13A;
	float SkillSwimAccBase;
	float SkillSwimAccRatio;
	unsigned __int16 ParamSubPleasure;
	unsigned __int16 ParamSubAnger;
	unsigned __int16 ParamSubSorrow;
	unsigned __int16 ParamSubFear;
	unsigned __int16 ParamSubSurprise;
	unsigned __int16 ParamSubPain;
	unsigned __int16 ParamSubRelax;
	unsigned __int16 ParamAddSleepy;
	unsigned __int16 ParamSubSleepy;
	unsigned __int16 ParamAddSleepDepth;
	unsigned __int16 ParamSubSleepDepth;
	unsigned __int16 ParamWakeUpSleepDepth;
	unsigned __int16 ParamAddHunger;
	unsigned __int16 ParamSubHunger;
	unsigned __int16 ParamAddBreed;
	unsigned __int16 ParamAddTediousCuriosity;
	unsigned __int16 ParamAddTediousAggressive;
	unsigned __int16 ParamAddTediousCalm;
	unsigned __int16 ParamSubTediousWalk;
	unsigned __int16 ParamSubTediousSwim;
	unsigned __int16 ParamAddLonely;
	unsigned __int16 ParamAddLonelyGreet;
	unsigned __int16 ParamAddLonelySuri;
	unsigned __int16 ParamAddLonelyAttack;
	unsigned __int16 ParamAddTireNormal;
	unsigned __int16 ParamSubTireSleep;
	unsigned __int16 ParamSubTireRest;
	unsigned __int16 ParamAddTireWalk;
	unsigned __int16 ParamAddTireRun;
	unsigned __int16 ParamAddTireSwim;
	unsigned __int16 ParamSubTireSwimStop;
	unsigned __int16 ParamSubNourishment;
	unsigned __int16 ParamAddConditionN;
	unsigned __int16 ParamConditionTriggerAddN;
	unsigned __int16 ParamSubConditionN;
	unsigned __int16 ParamConditionTriggerSubN;
	unsigned __int16 ParamSubConditionS;
	unsigned __int16 ParamConditionTriggerS;
	unsigned __int16 ParamSubConditionP;
	unsigned __int16 ParamConditionTriggerP;
	unsigned __int16 ParamAddConditionRest;
	unsigned __int16 ParamAddThirsty;
	unsigned __int16 ParamSubThirstySwim;
	unsigned __int16 ChaoTimerBaseInterval;
	unsigned __int16 ChaoTimerLifeInterval;
	unsigned __int16 ChaoTimerEmotionInterval;
	unsigned __int16 ChaoTimerMoodInterval;
	unsigned __int16 ChaoTimerKnowledgeInterval;
	unsigned __int16 ChaoTimerIllInterval;
	__int16 field_1A6;
	float IntentionScoreRandomRatio;
	float IntentionMinScore;
	unsigned int IntentionAngerTrigger;
	unsigned int IntentionAngerPlayerTrigger;
	float IntentionAngerMinScore;
	float IntentionAngerMaxScore;
	unsigned int IntentionFearTrigger;
	unsigned int IntentionFearPlayerTrigger;
	float IntentionFearMinScore;
	float IntentionFearMaxScore;
	unsigned int IntentionSorrowTrigger;
	float IntentionSorrowMinScore;
	float IntentionSorrowMaxScore;
	unsigned int IntentionPainTrigger;
	float IntentionPainMinScore;
	float IntentionPainMaxScore;
	unsigned int IntentionPleasureTrigger;
	float IntentionPleasureMinScore;
	float IntentionPleasureMaxScore;
	unsigned int IntentionHungerGooMinInterval;
	unsigned int IntentionHungerGooIntervalRange;
	unsigned int IntentionHungerGooTrigger;
	float IntentionHungerGooMaxScore;
	unsigned int IntentionHungerFruitGoToEatTrigger;
	float IntentionHungerFruitGoToEatMaxScore;
	unsigned int IntentionHungerFruitSearchTrigger;
	float IntentionHungerFruitSearchMaxScore;
	unsigned int IntentionAkubiMinInterval;
	unsigned int IntentionAkubiIntervalRange;
	unsigned int IntentionAkubiTrigger1;
	unsigned int IntentionAkubiTrigger2;
	unsigned int IntentionAkubiTrigger3;
	float IntentionAkubiMaxScore;
	unsigned int IntentionPlayerGreetLonelyTrigger;
	float IntentionPlayerGreetMaxScore;
	unsigned int IntentionPlayerSuriSuriLonelyTrigger;
	float IntentionPlayerSuriSuriMaxScore;
	unsigned int IntentionSleepTrigger;
	float IntentionSleepMaxScore;
	unsigned int IntentionHimaTrigger;
	float IntentionHimaMaxScore;
	unsigned int IntentionThirstyTrigger;
	float IntentionThirstyMaxScore;
	unsigned int IntentionApproachNestTrigger;
	float IntentionApproachNestMaxScore;
	unsigned int IntentionJyouroThirstyTrigger;
	float IntentionJyouroMaxScore;
	unsigned int IntentionRestTrigger1;
	unsigned int IntentionRestTrigger2;
	float IntentionRestMaxScore;
	unsigned int IntentionRestEndTrigger;
	unsigned int RaceIntervalSubStamina;
	unsigned int RaceSubStaminaSwim;
	unsigned int RaceSubStaminaFly;
	unsigned int RaceSubStaminaRun;
	unsigned int RaceSubStaminaClimb;
	unsigned int RaceSubStaminaTree;
	unsigned int RaceSubStaminaOther;
	unsigned int RaceAddStaminaSit;
	unsigned int RaceCheerInterval;
	unsigned int RaceCheerDecAimStamina;
	unsigned int RaceCheerDecStamina;
	float RaceTurboRatioSwim;
	float RaceTurboRatioFly;
	float RaceTurboRatioRun;
	float RaceTurboRatioClimb;
	float RaceTurboRatioTree;
	float RaceTurboRatioOther;
};
#pragma pack(pop)

DataPointer(CHAO_GLOBAL, ChaoGlobal, 0x01312C48);

enum
{
	INT_TIMER_PLAYER,
	INT_TIMER_CHAO,
	INT_TIMER_GREET,
	INT_TIMER_SING,
	INT_TIMER_MUSIC,
	INT_TIMER_DANCE,
	INT_TIMER_ART,
	INT_TIMER_TOY,
	INT_TIMER_LTOY,
	INT_TIMER_MINIMAL,
	INT_TIMER_TV,
	INT_TIMER_RADICASE,
	INT_TIMER_BOX,
	INT_TIMER_BALL,
	INT_TIMER_GOO,
	INT_TIMER_AKUBI,
	INT_TIMER_COCOONREACT, //why is there 128 fields, i dont know, but thanks sega
	NB_INT_TIMER,
};

Uint32 AL_SetIntervalTimer(ObjectMaster* a1, Uint16 TimerKind, Uint32 timer);

void __cdecl AL_CalcIntentionScore_Chat(ObjectMaster* a1, float* a2);
void AL_CalcIntentionScore_Tree(ObjectMaster* a1, float* a2);
float __cdecl AL_CalcScoreTypeA(signed int value, signed int minimum);
void AL_IntentionInit();
void __cdecl AL_ScoreRandomize(float* a1);