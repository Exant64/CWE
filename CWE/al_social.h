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
	task* chaoPointer;
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
void ALS_ChatSetup(task* a1, task* a2);
int ALBHV_Social(task* a1);
task* Social_Create(SBHV_FUNC idleFunc);
task* Social_GetActor(task* a1, int a2);
void Social_SetActor(task* a1, int a2, task* chao);
void Social_SetParameter(task* a1, int a2, int param);
void Social_QueueBehavior(task* a1, int a2, SBHV_FUNC func);
void Social_SetIdle(task* a1, SBHV_FUNC a2);
void Social_SetSync(task* a1, int a2);