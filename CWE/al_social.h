#pragma once

struct SOCIAL_BHVDATA
{
	int Mode;
	short Timer;
	short SubTimer;
};

#define SBHVCOUNT 48

struct SOCIALDATA
{
	ObjectMaster* chaoPointer;
	SOCIAL_BHVDATA bhvStatus;
	int parameter1;
	int parameter2;
	int actorIndex;
};

typedef int(__cdecl* SBHV_FUNC)(SOCIALDATA*);

struct SBHV_FUNCDATA
{
	unsigned char actorindex;
	SBHV_FUNC func;
	char syncindex;
};

enum
{
	SOCIAL_CHAO1 = 0,
	SOCIAL_CHAO2 = 1,
	SOCIAL_CHAO3 = 2,
	SOCIAL_CHAO4 = 3,
	SOCIAL_CHAOCOUNT = 4
};

struct socialwk
{
	SOCIALDATA data[SOCIAL_CHAOCOUNT];
	SBHV_FUNCDATA bhvFuncs[SBHVCOUNT];
	SBHV_FUNC idleFunc;
	int nbBhvFuncEntry;
	int CurrBhvFuncNum;
};
void ALS_ChatSetup(ObjectMaster* a1, ObjectMaster* a2);
int ALBHV_Social(ObjectMaster* a1);
ObjectMaster* Social_Create(SBHV_FUNC idleFunc);
ObjectMaster* Social_GetActor(ObjectMaster* a1, int a2);
void Social_SetActor(ObjectMaster* a1, int a2, ObjectMaster* chao);
void Social_SetParameter(ObjectMaster* a1, int a2, int param);
void Social_QueueBehavior(ObjectMaster* a1, int a2, SBHV_FUNC func);
void Social_SetIdle(ObjectMaster* a1, SBHV_FUNC a2);
void Social_SetSync(ObjectMaster* a1, int a2);