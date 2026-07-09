#include "stdafx.h"
#include <Chao.h>
#include <random>
#include <AL_ModAPI.h>
#include <ALifeSDK_Functions.h>
#include "albhv.h"

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
static void __declspec(naked) sub_550620Hook()
{
	__asm
	{
		push esi // a1

		// Call your __cdecl function here:
		call sub_550620

		pop esi // a1
		retn
	}
}

void __cdecl EggChao_ALO_Delete(task* a1)
{
	if (GET_CHAOPARAM(a1)->body.FormNum != 1)
		ALW_CancelEntry(a1);
}
static void __declspec(naked) EggChao_ALO_Delete_Hook()
{
	__asm
	{
		push esi // a1

		// Call your __cdecl function here:
		call EggChao_ALO_Delete

		pop esi // a1
		retn
	}
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
		Chao_Animation(a1, a2);
}
static void __declspec(naked) EggChaoCrawlHook()
{
	__asm
	{
		push edx // a2
		push eax // result

		// Call your __cdecl function here:
		call EggChaoCrawl

		pop eax // result
		pop edx // a2
		retn
	}
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


const int LoadCocoonPtr = 0x00568CD0;
void LoadCocoon(task* a1, char a2)
{
	__asm
	{
		mov eax, a1
		push dword ptr[a2]
		call LoadCocoonPtr
		add esp, 4
	}
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
static void __declspec(naked) CreateMayuHook()
{
	__asm
	{
		push[esp + 04h] // a2
		push eax // a1

		// Call your __cdecl function here:
		call CreateMayuField

		add esp, 4 // a1<eax> is also used for return value
		add esp, 4 // a2
		retn
	}
}

void ALBHV_Life_Init() {
	//egg chao code
	WriteCall((void*)0x0059C2D7, EggChaoSpawnEgg);

	WriteCall((void*)0x005685A4, EggChaoReincarnationEgg);
	WriteCall((void*)0x00568591, EggChao_ALO_Delete_Hook);
	WriteJump((void*)0x00550620, sub_550620Hook);
	WriteData((int*)0x00569173, (int)EggChao_KeepOrDie);

	WriteCall((void*)0x005A32A8, EggChaoCrawlHook);
	WriteJump((void*)0x0054B151, EggChaoCrawlHook);

	//cocoon reactions
	WriteCall((void*)0x00568DE4, CreateMayuHook);
	WriteCall((void*)0x00569048, CreateMayuHook);
	WriteCall((void*)0x0569059, CreateMayuHook);
}