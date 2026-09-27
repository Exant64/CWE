#include "stdafx.h"
#include <Chao.h>
#include <random>
#include <AL_ModAPI.h>
#include "albhv.h"
#include "asmutil.h"

int __cdecl ALBHV_EggChao(task* a1)
{
	chaowk* wk = GET_CHAOWK(a1);
	AL_BEHAVIOR* bhv = &wk->Behavior;

	switch (bhv->Mode)
	{
	case 0:
		AL_SetMotionLinkStep(a1, 492, 15);
		bhv->Timer = 0;
		bhv->Mode++;
		bhv->SubTimer = (4 + (int)(njRandom() * 4.f));
		break;
	case 1:
		if (!AL_IsMotionEnd(a1))
			break;
		
		if (++bhv->Timer < bhv->SubTimer)
			break;

		return BHV_RET_FINISH;
	}
	return BHV_RET_CONTINUE;
}

void __cdecl EggChaoSpawnEgg(AL_GENE* a1, CHAO_PARAM_GC* chaoData, int a3, NJS_VECTOR* position, int a5)
{
	if (njRandom() <= 0.01f)
	{
		if (a1)
		{
			chaoData->gene = *a1;
		}
		chaoData->type = TYPE_CHILD;
		chaoData->ClassNum = -1;
		task* chao = CreateChaoExtra(chaoData, 0, 0, position, a5);
		chaoData->body.FormNum = AL_FORM_EGG_FOOT;
		AL_SetBehavior(chao, ALBHV_EggChao);
	}
	else {
		CreateEgg(a1, chaoData, a3, position, a5);
	}
}

void __cdecl sub_550620(task* a1)
{
	if (GET_CHAOPARAM(a1)->body.FormNum != 1)
	{
		memset(GET_CHAOPARAM(a1), 0, sizeof(CHAO_PARAM_GC));
		a1->exec = DestroyTask;
	}
}

static void ASM_FUNC sub_550620Hook() {
	ASM_PUSH(esi); // a1

	ASM_PUSH(esi); // a1
	// Call your __cdecl function here:
	ASM_CALL (sub_550620);
	ASM_ESP_ADD(1); // a1

	ASM_POP(esi); // a1
	ASM_RET(0);
}

void __cdecl EggChao_ALO_Delete(task* a1)
{
	if (GET_CHAOPARAM(a1)->body.FormNum != 1)
		ALW_CancelEntry(a1);
}

static void ASM_FUNC EggChao_ALO_Delete_Hook() {
	ASM_PUSH(esi); // a1

	ASM_PUSH(esi); // a1
	// Call your __cdecl function here:
	ASM_CALL (EggChao_ALO_Delete);
	ASM_ESP_ADD(1); // a1

	ASM_POP(esi); // a1
	ASM_RET(0);
}

void __cdecl EggChaoReincarnationEgg(AL_GENE* a1, CHAO_PARAM_GC* chaoData, int a3, NJS_VECTOR* position, int a5)
{
	GET_CWEPARAM(chaoData)->ForceReincarnate = false;
	
	if (chaoData->body.FormNum == 1)
	{
		if (a1)
		{
			chaoData->gene = *a1;
		}
		chaoData->type = TYPE_CHILD;
		chaoData->ClassNum = -1;
		chaoData->body.FormNum = AL_FORM_EGG_FOOT;
	}
	else
		CreateEgg(a1, chaoData, a3, position, a5);
}

void __cdecl EggChaoCrawl(MOTION_CTRL* a1, int a2)
{
	chaowk* data1 = (chaowk*)((int)a1 - 0xAC);
	if (data1->pParamGC->body.FormNum == 1)
		Chao_RegAnimationTbl(a1, "alm_egg_crawl_start");
	else
		SetMotionLink(a1, a2);
}

static void ASM_FUNC EggChaoCrawlHook() {
	ASM_PUSH(edx); // a2
	ASM_PUSH(eax); // result

	// Call your __cdecl function here:
	ASM_CALL (EggChaoCrawl);

	ASM_POP(eax); // result
	ASM_POP(edx); // a2
	ASM_RET(0);
}

void __cdecl EggChao_KeepOrDie(task* a1)
{
	if (GET_CHAOPARAM(a1)->body.FormNum == 1)
	{
		a1->exec = ChaoExecutor;
		AL_SetBehavior(a1, ALBHV_Think);
	}
	else
		a1->exec = DestroyTask;
}

ASM_FUNC void LoadCocoon(task* a1, char a2) {
    // arguments
    ASM_PUSH(      ASM_ESP(2+0+0) ); // a2
    ASM_MOVE( eax, ASM_ESP(1+1+0) ); // a1

    // call
    ASM_CALL_R( edx, 0x00568CD0 );

    // end arguments
    ASM_ESP_ADD( 1 );

    // return
    ASM_RET( 0 );
}

enum MayuKind
{
	MayuKind_Grow = 0x0,
	MayuKind_Succeed = 0x1,
	MayuKind_Death = 0x2,
};
int MayuFieldTimer = 0x0C8 + 0x708 + 0x384 + 0x384;
void CreateMayuField(task* a1, char a2)
{
	NJS_VECTOR* pos;
	LoadCocoon(a1, a2);
	pos = &a1->twp->pos;//ALO_GetMayuWatchPos(a1, &pos);
	switch (a2)
	{
	case MayuKind_Grow:
		AL_ChildFieldCreateT(a1, CI_KIND_AL_MAYU_GROW_FIELD, pos, 50.0f, MayuFieldTimer);
		break;
	case MayuKind_Death:
		AL_ChildFieldCreateT(a1, CI_KIND_AL_MAYU_DEATH_FIELD, pos, 50.0f, MayuFieldTimer);
		break;
	case MayuKind_Succeed:
		AL_ChildFieldCreateT(a1, CI_KIND_AL_MAYU_SUCCEED_FIELD, pos, 50.0f, MayuFieldTimer);
		break;
	}
}

static void ASM_FUNC CreateMayuHook() {
	ASM_PUSH(ASM_ESP(1)); // a2
	ASM_PUSH(eax); // a1

	// Call your __cdecl function here:
	ASM_CALL (CreateMayuField);

	ASM_ESP_ADD( 1 ); // a1<eax> is also used for return value
	ASM_ESP_ADD( 1 ); // a2
	ASM_RET(0);
}

void ALBHV_Life_Init() {
	//egg chao code
	WriteCall((void*)0x0059C2D7, (void*)EggChaoSpawnEgg);

	WriteCall((void*)0x005685A4, (void*)EggChaoReincarnationEgg);
	WriteCall((void*)0x00568591, (void*)EggChao_ALO_Delete_Hook);
	WriteJump((void*)0x00550620, (void*)sub_550620Hook);
	WriteData((int*)0x00569173, (int)EggChao_KeepOrDie);

	WriteCall((void*)0x005A32A8, (void*)EggChaoCrawlHook);
	WriteJump((void*)0x0054B151, (void*)EggChaoCrawlHook);

	//cocoon reactions
	WriteCall((void*)0x00568DE4, (void*)CreateMayuHook);
	WriteCall((void*)0x00569048, (void*)CreateMayuHook);
	WriteCall((void*)0x0569059, (void*)CreateMayuHook);
}