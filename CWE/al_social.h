#pragma once

struct SOCIAL_CONTEXT {
	int Data[4];
};

struct SOCIAL_BHVDATA
{
	int Mode;
	short Timer;
	short SubTimer;
	void* UserData;
};

#define SBHVCOUNT 48

struct SOCIAL_ACTOR
{
	ObjectMaster* chaoPointer;
	SOCIAL_BHVDATA bhvStatus;
	SOCIAL_CONTEXT* socialContext;
	int parameter1;
	int parameter2;
	int actorIndex;
};

typedef int(__cdecl* SBHV_FUNC)(SOCIAL_ACTOR*);

struct SBHV_FUNCDATA
{
	size_t actorindex;
	SBHV_FUNC func;
	size_t syncindex;
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
	SOCIAL_ACTOR actorData[SOCIAL_CHAOCOUNT];
	SBHV_FUNCDATA bhvFuncs[SBHVCOUNT];
	SOCIAL_CONTEXT socialContext;
	SBHV_FUNC idleFunc;
	int nbBhvFuncEntry;
	int CurrBhvFuncNum;
};

void ALS_ChatSetup(ObjectMaster* a1, ObjectMaster* a2);
ObjectMaster* Social_Create(SBHV_FUNC idleFunc);
task* Social_GetActor(task* tp, size_t actorIndex);

void Social_SetActor(task* tp, size_t actorIndex, task* pChao);
void Social_SetParameter(task* tp, size_t actorIndex, int param);

void Social_QueueBehavior(task* tp, size_t actorIndex, SBHV_FUNC func);
void Social_SetIdle(task* tp, SBHV_FUNC pFunc);
void Social_SetSync(task* tp, size_t actorIndex);