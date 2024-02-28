#include "stdafx.h"

#include <brightfixapi.h>
#include "ALifeSDK_Functions.h"
#include "al_sandhole.h"
#include "alg_kinder_bl.h"
#include "al_behavior/al_behavior.h"
#include "memory.h"

//THEY DIDNT RESET THE SHADERS LOL
void CocoonFix()
{
	SetShaders(1);
	DoLighting(LightIndex);
}

ObjectFunc(EGG_Display, 0x0057B640);
void __cdecl EGG_Display_(ObjectMaster* a1)
{
	BrightFixPlus_ShinyCheck(1);
	EGG_Display(a1);

}

ObjectMaster* dword_1946618[32];
FunctionPointer(void, free_, (void* lpMem), 0x07A80DD);
FunctionPointer(void*, new_, (int size), 0x007A5DA1);
void ChaoExpandPatch() {
	free_((void*)(*(unsigned int*)0x1A55790));
	void* v37 = new_(0x400);
	if (v37)
	{
		memset(v37, 0, 0x400);
	}
	else
	{
		v37 = 0;
	}
	*(unsigned int*)0x1A55790 = (unsigned int)v37;

	WriteData((char*)0x0047E912, (char)32);

	WriteData((int*)0x485C81, ((int)dword_1946618) - 4);

	//addtocollisionlist
	WriteData((int*)0x0047E923, (int)dword_1946618);
	//
	WriteData((int*)0x00485ADF, (int)dword_1946618);
	WriteData((int*)0x00485AE4, (int)dword_1946618);
	//
	WriteData((int*)0x00485C2F, (int)dword_1946618);
	WriteData((int*)0x00485C34, (int)dword_1946618);
	//sub_485c70
	WriteData((int*)0x0485C86, (int)dword_1946618);
	WriteData((int*)0x0485C9B, (int)dword_1946618);
	WriteData((int*)0x0485CD0, (int)dword_1946618);
	WriteData((int*)0x0485CD5, (int)dword_1946618);
}
const int njInitTexturePtr = 0x0042FA60;
void njInitTexture(void* result)
{
	__asm
	{
		mov eax, result
		call njInitTexturePtr
	}
}
const int njInitTextureBufferPtr = 0x042FA90;
void njInitTextureBuffer(void* result)
{
	__asm
	{
		mov eax, result
		call njInitTextureBufferPtr
	}
}

void __cdecl ExpandTextureBuffer()
{
	//nj_texmanage
	void* v0 = syMalloc(0xC000 * 2, "..\\..\\src\\main_gc.c", 2091);
	*(void**)0x1934B00 = v0;
	WriteData((int*)0x0042FA61, (int)2048 * 2);
	njInitTexture(v0);

	//tex buffer
	void* v2 = syMalloc(0x14000 * 2, "..\\..\\src\\main_gc.c", 2092);
	*(void**)0x1934AF8 = v2;
	WriteData((int*)0x0042FA91, (int)2048 * 2);
	njInitTextureBuffer(v2);
}

static void __cdecl BreedPatch(ObjectMaster* a1)
{
	sub_54A690(a1);
	//attentionoff
	al_entry_work* v1 = (al_entry_work*)a1->UnknownA_ptr;
	if (v1)
	{
		v1->flag = 0;
		v1->pCommu = 0;
		v1->CommuID = -1;
	}

}
static void __declspec(naked) BreedPatch_asm()
{
	__asm
	{
		push eax // a1

		// Call your __cdecl function here:
		call BreedPatch

		pop eax // a1
		retn
	}
}

DataPointer(int, BallUsable, 0x01DBE574);
DataPointer(ObjectMaster*, ALO_BallPtr, 0x01DBE570);
signed int __cdecl AL_CheckBallFix(ObjectMaster* a1)
{
	Data1Ptr v1; // eax
	ChaoDataBase* v2; // esi

	if (ALO_BallPtr == nullptr || ALO_BallPtr->Data1.Undefined == nullptr || !BallUsable)
	{
		return 0;
	}
	v1.Chao = (ChaoData1*)a1->Data1.Chao;
	v2 = v1.Chao->pParamGC;
	if (AL_EmotionGetValue(a1, EM_ST_TEDIOUS) <= 4000
		|| CheckDistance(&v1.Chao->entity.Position, &ALO_BallPtr->Data1.Entity->Position) >= 50)
	{
		return 0;
	}

	AL_EmotionAdd(a1, EM_ST_TEDIOUS, -2000);
	AL_SetBehaviorWithTimer(a1, (int)0x0563830, -1);
	return 1;
}
static void __declspec(naked) AL_CheckBallHook()
{
	__asm
	{
		push edi // a1

		// Call your __cdecl function here:
		call AL_CheckBallFix

		pop edi // a1
		retn
	}
}

void Ball_Delete(ObjectMaster* a1) {
	ObjectFunc(j_ALO_Delete, 0x0057B9B0);
	j_ALO_Delete(a1);
	ALO_BallPtr = 0;
	BallUsable = 0;
}


const float hResN = 480.0;
const float vResN = 853.0;

void CWE_Fixes() {
	ChaoExpandPatch();
	ExpandTextureBuffer();

	//fruit growth getting divided by 2 fix
	WriteData((double**)0x005483CE, (double*)0x9054C0);

	//shad's animal sound fix
	WriteData((int*)0x0057C584, 4141); // Penguin, Seal, Otter, Half-fish
	WriteData((int*)0x0057C599, 4139); // Cheetah, Warthog, Bear, Tiger, Dragon
	WriteData((int*)0x0057C5AD, 4140); // Gorilla
	WriteData((int*)0x0057C5C2, 4142); // Peacock, Parrot, Condor, Phoenix
	WriteData((int*)0x0057C5D7, 4143); // Sheep
	WriteData((int*)0x0057C5EB, 4144); // Batman

	//fixes ball usable crash
	WriteJump((void*)0x0563B70, AL_CheckBallHook);
	WriteData((int*)0x0055D678, (int)Ball_Delete);

	WriteCall((void*)0x0059BB76, BreedPatch_asm); //breed patch

	//attempt at fixing fruit-pickup race condition thing
	WriteJump((void*)0x0053052A, (void*)0x530569); //alw_turntolockon
	WriteJump((void*)0x00530596, (void*)0x005305B6);//calcdistfromlockon

	//chaos chao animal ear fix
	NJS_OBJECT** ChaoObjects = (NJS_OBJECT**)GetDllData("AL_RootObject");
	NJS_OBJECT* ChaosChaoRoot = ChaoObjects[108];
	NJS_OBJECT* EarObject = ChaosChaoRoot->child->child->sibling->sibling->sibling->sibling->sibling->child->child->child->sibling->sibling->sibling->sibling->sibling;
	EarObject->ang[1] = 0;

	//egg bright shell fix
	WriteData((int*)0x0057BB0E, (int)EGG_Display_);

	//SHAD
	WriteData((int*)0x005A74C3, (int)&hResN);   //Departure Machine Scrolling fix
	WriteData((int*)0x005A74D1, (int)&vResN);

	//cocoon fix
	WriteCall((void*)0x0568AA1, CocoonFix);

	//tree fix
	WriteData((char*)0x54AEE1, (char)0x6);//change offset they use from the first short to the last one

	//fix env map condition bug in chDrawCnk
	WriteData<6>((char*)0x0056DE7D, (char)0x90);

	//fix pearl price
	HatBMAttributes[SA2BHat_PearlEggShell].SalePrice = 8000;

	//trumpet fix
	((NJS_OBJECT*)0x11C830C)->pos[1] = -0.4f; //adjustment
	((NJS_OBJECT*)0x11C830C)->evalflags &= ~NJD_EVAL_UNIT_POS; //dont ignore position
}