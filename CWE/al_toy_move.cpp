#include "stdafx.h"

#include "ALifeSDK_Functions.h"
#include "al_world.h"
#include "Chao.h"
#include "al_toy_move.h"
#include "al_save.h"
#include "al_behavior/albhv.h"
#include "al_stage.h"

#include "UsercallFunctionHandler.h"
#include "FunctionHook.h"
#include "util.h"
#include "asm_util.h"

enum {
	MD_IDLE = 0,
	MD_STATIC = 1,
	MD_DYNAMIC = 2,
	MD_HOLDP = 3,
};

struct TOY_MOVE_WK {
	Uint8 mode;
	Uint8 smode;
	int timer;
	float floatVal;
};

// HACK: so for the whole toy registering ordeal, the original version delayed the displaysub (because the displaysub needed the entrywork)
// and we couldn't hook the actual ALW_Entry function and do all our business there (because the position and angle set in the load functions  
// runs **after* the ALW_Entry call), so this is an ugly workaround
// we set this pointer when AL_Toy_Move_Init gets called, and then inside our FunctionHook/UsercallHooks, we use it to get the currently being spawned toy's task ptr
// and call ALW_Entry there
static task* pLastToyTask = NULL;

const int MOV_ControlPtr = 0x00796780;
void MOV_Control(task* eax0)
{
	__asm
	{
		mov eax, eax0
		call MOV_ControlPtr
	}
}

const int sub_54B230Ptr = 0x54B230;
void sub_54B230(task* eax0, float a2)
{
	__asm
	{
		mov eax, eax0
		push a2
		call sub_54B230Ptr
		add esp, 4
	}
}

const int sub_47D9E0Ptr = 0x47D9E0;
void ObjectMovableInitialize(taskwk* a1, int a2)
{
	__asm
	{
		mov eax, a1
		mov edx, a2
		call sub_47D9E0Ptr
	}
}

const int MoveFunc2Ptr = 0x00798300;
void MOV_DetectCollision(task* a1)
{
	__asm
	{
		mov esi, a1
		call MoveFunc2Ptr
	}
}

#define ALW_Entry2_p                0x00530750
ASM_FUNC
Sint32
ALW_Entry2(Uint16 category, task* tp, Uint16 kind, void* pSaveInfo)
{
    // save regs
    ASM_PUSH( ebx );

    // arguments
    ASM_PUSH(      ASM_ESP(4+0 +1) ); // pSaveInfo
    ASM_PUSH(      ASM_ESP(3+1 +1) ); // kind
    ASM_MOVE( ebx, ASM_ESP(2+2 +1) ); // tp
    ASM_MOVE( ecx, ASM_ESP(1+2 +1) ); // category

    // call
    ASM_CALL_R( edx, ALW_Entry2_p );

    // end arguments
    ASM_ESP_ADD( 2 );

    // pull regs
    ASM_POP( ebx );

    // return
    ASM_RET( 0 );
}

void AL_Toy_Move_Register(task* obj, __int16 a3)
{
	ITEM_SAVE_INFO* info = NULL;
	
	if(AL_IsGarden()) {
		int saveIndex = a3;
		saveIndex *= 3;
		saveIndex += (AL_GetStageNumber() - 1);

		info = &cweSaveFile.cweToyInfo[saveIndex];
		if (info->place == 0) {
			info->place = AL_GetStageNumber();
			info->pos = obj->twp->pos;
			info->nbVisit = obj->twp->ang.y;
		}
	}
	else {
		___OutputDebugString("AL_Toy_Move_Register: invalid toy area");
	}

	ALW_Entry2(ALW_CATEGORY_TOY, obj, a3, info);

	if (info) {
		obj->twp->pos = info->pos;
		obj->twp->ang.y = info->nbVisit;
	}
}

TOY_MOVE_WK* GetToyMove(task* a1)
{
	return (TOY_MOVE_WK*)((int)a1->mwp + 0x26C);
}

void AL_Toy_Move_Update(task *tp) {
	MOVE_WORK* move = GET_MOVE_WORK(tp);
	TOY_MOVE_WK* toyMove = GetToyMove(tp);

	//todo: "nextaction" for collision changes?
	switch (toyMove->mode)
	{
		case MD_IDLE:
			if (!toyMove->smode) {
				CCL_Disable(tp, 0);
				CCL_Disable(tp, 1);

				toyMove->smode++;
			}

			//run collision for 30 frames, after that become static
			MOV_Control(tp);
			MOV_DetectCollision(tp);

			if (++toyMove->timer > 30) {
				toyMove->timer = 0;
				toyMove->smode = 0;
				toyMove->mode = MD_STATIC;
			}
			break;

		case MD_STATIC:
			if (!toyMove->smode) {
				CCL_Enable(tp, 0);
				CCL_Enable(tp, 1);

				toyMove->smode++;
			}

			//if not picked up
			if (tp->twp->flag >= 0)
			{
				colliwk* v8 = tp->twp->cwp;
				if (v8)
				{
					//if the object is touched, start running collision
					if ((v8->flag & 0x10) != 0)
					{
						toyMove->timer = 0;
						toyMove->smode = 0;
						toyMove->mode = MD_DYNAMIC;
					}
				}
			}
			else
			{
				//if it is picked up, run pickup
				toyMove->timer = 0;
				toyMove->smode = 0;
				toyMove->mode = MD_HOLDP;
			}
			break;

		case MD_DYNAMIC:
			if (!toyMove->smode) {
				CCL_Enable(tp, 0);
				CCL_Enable(tp, 1);

				toyMove->smode++;
			}

			//water handler
			sub_54B230(tp, toyMove->floatVal);

			if (tp->twp->flag & 1) {
				NJS_VECTOR veloVec;
				veloVec.x = move->Velo.x;
				veloVec.y = 0.0;
				veloVec.z = move->Velo.z;

				//if it has velocity, reset timer to 0
				if (njScalor(&veloVec) >= 0.01f) {
					toyMove->timer = 0;
				}
				else if (++toyMove->timer > 30) {
					//if it doesnt have velocity start running timer
					//if the the timer reaches 30, become static again

					move->Velo.x = 0.0;
					move->Velo.y = 0.0;
					move->Velo.z = 0.0;
					move->Acc.x = 0.0;
					move->Acc.y = 0.0;
					move->Acc.z = 0.0;

					toyMove->smode = 0;
					toyMove->timer = 0;
					toyMove->mode = MD_STATIC;
				}
			}

			//if picked up
			if (tp->twp->flag < 0) {
				toyMove->smode = 0;
				toyMove->timer = 0;
				toyMove->mode = MD_HOLDP;
			}

			MOV_Control(tp);
			MOV_DetectCollision(tp);
			break;

		case MD_HOLDP:
			if (!toyMove->smode) {
				CCL_Disable(tp, 0);
				CCL_Disable(tp, 1);

				toyMove->smode++;
			}

			ALW_CommunicationOff(tp);
			tp->twp->ang.y = 0x4000 - playertwp[0]->ang.y;

			//if it gets put down, go back to dynamic
			if (tp->twp->flag >= 0) {
				toyMove->timer = 0;
				toyMove->smode = 0;
				toyMove->mode = MD_DYNAMIC;
			}

			break;
	}

	move->PrePos = tp->twp->pos;
}

static MOVE_WORK* MOV_Init_ToyHack(task* obj) {
	MOVE_WORK* move; // esi

	move = (MOVE_WORK*)AllocateArray(0x26C + sizeof(TOY_MOVE_WK), 1, (char*)"..\\..\\src\\move.c", 64);
	obj->mwp = (motionwk*)move;                   // different offset than SADX

	move->Top = 3.0f;
	move->RotSpd.y = 256;                      // different offset than SADX
	move->Side = 2.2f;	
	move->Bottom = -3.0f;
	move->CliffHeight = 40.0f;
	move->BoundSide = 0.80000001f;
	move->BoundFloor = 0.89999998f;
	move->BoundCeiling = 0.2f;
	move->BoundFriction = 0.80000001f;
	move->Offset.y = 3.0f;

	return move;
}

void AL_Toy_Move_Init(task* tp, const CCL_INFO* pInfo, size_t count) {
	MOVE_WORK* mov = MOV_Init_ToyHack(tp);

	// this pointer's purpose is explained at the declaration
	pLastToyTask = tp;

	mov->Gravity = -0.05f;
	mov->Offset.y = 3.0f;
	mov->BoundFloor = 0.7f;
	mov->BoundFriction = 0.7f;
	mov->unk = 3;
	mov->Flag |= 8;

	GetToyMove(tp)->floatVal = 1.0f;

	CCL_INFO info[16] = { 0 };

	assert(count + 1 <= _countof(info));
	
	info[0] = pickupableColli;
	for(size_t i = 0; i < count; ++i) {
		info[1 + i] = pInfo[i];
	}

	CCL_Init(tp, info, count + 1, 5);

	if (tp->mwp) {
		ObjectMovableInitialize(tp->twp, 10);
	}
}

void __cdecl AddToColliListToy(task* a1)
{
	AL_Toy_Move_Update(a1);

	//if (a1->twp->Action == 0)
	//	a1->twp->Status |= 0x240u;
	//else
	//	a1->twp->Status &= ~0x240u;

	CCL_Entry(a1);
}

static void __declspec(naked) AddToCollisionListHook()
{
	__asm
	{
		push esi // a1

		// Call your __cdecl function here:
		call AddToColliListToy

		pop esi // a1
		retn
	}
}

CCL_INFO ALO_Horse_collision = { '\0', '\0', 'w', '\f', 0u, {  0,  1.2f,  0 }, 2, 0, 0, 0, 0, 0, 0 };
CCL_INFO pickupableColli = { 0, 1, 0x70, 0, 9216u, {  0.0,  1.5,  0.0 }, 15.0, 3.0, 0.0, 0.0, 0, 0, 0 };
CCL_INFO RadioCol = { '\0', '\0', 'w', '\f', 32768u, {  0.0,  0.0,  0.0 }, 2.0, 0.0, 0.0, 0.0, 0, 0, 0 };
CCL_INFO stru_8A5C10 = { '\0', '\0', 'w', '\f', 32768u, {  0.0,  1.0,  0.0 }, 2.0, 0.0, 0.0, 0.0, 0, 0, 0 };
CCL_INFO ALO_BoxExecutor_collision = { '\0', '\0', 'w', '\f', 32768u, {  0,  1.6f,  0 }, 1.6f, 0, 0, 0, 0, 0, 0 };

void __cdecl AL_TV_Init(task* a1)
{
	AL_Toy_Move_Init(a1, &stru_8A5C10, 1);
	GET_MOVE_WORK(a1)->Offset.y = 1.75f;
}

static void __declspec(naked) AL_TV_Init_Hook()
{
	__asm
	{
		push eax // obj

		// Call your __cdecl function here:
		call AL_TV_Init

		add esp, 4 // obj<eax> is also used for return value
		retn
	}
}

void __cdecl AL_Toy_Update(task* a1)
{
	AL_Toy_Move_Update(a1);

	if (!ALW_IsAttention(a1))
		a1->twp->flag |= 0x240u;
	else
		a1->twp->flag &= ~0x240u;


	CCL_Entry(a1);
}

static void __declspec(naked) AL_Toy_UpdateHook()
{
	__asm
	{
		push esi // a1

		// Call your __cdecl function here:
		call AL_Toy_Update

		pop esi // a1
		retn
	}
}

void __cdecl AL_Box_Init(task* a1)
{
	AL_Toy_Move_Init(a1, &ALO_BoxExecutor_collision, 1);
	//a1->EntityData2->field_AC = 1.75f;
}

static void __declspec(naked) AL_Box_Init_Hook()
{
	__asm
	{
		push eax // obj

		// Call your __cdecl function here:
		call AL_Box_Init

		add esp, 4 // obj<eax> is also used for return value
		retn
	}
}

void __cdecl AL_Radio_Init(task* a1)
{
	AL_Toy_Move_Init(a1, &RadioCol, 1);
	GET_MOVE_WORK(a1)->Offset.y = 1.6f;
}

static void __declspec(naked) AL_Radio_Init_Hook()
{
	__asm
	{
		push eax // obj

		// Call your __cdecl function here:
		call AL_Radio_Init

		add esp, 4 // obj<eax> is also used for return value
		retn
	}
}

void __cdecl AL_Horse_Init(task* a1)
{
	AL_Toy_Move_Init(a1, &ALO_Horse_collision, 1);
	//a1->EntityData2->field_AC = 1.75f;
}

static void __declspec(naked) AL_Horse_Init_Hook()
{
	__asm
	{
		push eax // obj

		// Call your __cdecl function here:
		call AL_Horse_Init

		add esp, 4 // obj<eax> is also used for return value
		retn
	}
}

void __cdecl ALO_BoxExecutor_Main_(task* a1)
{
	a1->exec = ALO_BoxExecutor;
	AL_Toy_Move_Register(a1, ALW_KIND_BOX);
}

void __cdecl ALO_TVExecutor_Display_(task *a1)
{
	a1->disp = ALO_TVDisplayer;
	AL_Toy_Move_Register(a1, ALW_KIND_TV);
	ALO_TVDisplayer(a1);
}

FunctionPointer(void, ALO_HorseDisplayer, (task*), 0x00580CA0);
void __cdecl ALO_HorseExecutor_Display_(task* a1)
{
	a1->disp = ALO_HorseDisplayer;
	AL_Toy_Move_Register(a1, ALW_KIND_HORSE);
	ALO_HorseDisplayer(a1);
}

void __cdecl ALO_RadicaseExecutor_Display_(task* a1)
{
	a1->disp = ALO_RadicaseDisplayer;
	AL_Toy_Move_Register(a1, ALW_KIND_RADICASE); 
	ALO_RadicaseDisplayer(a1);
}

template<task_exec func, int index>
void __cdecl ALO_ToyDisplayHook(task* tp) {
	tp->disp = func;
	AL_Toy_Move_Register(tp, index);
	func(tp);
}

void __cdecl ALO_Ball_Main2_(task* a1)
{
	//a1->MainSub = ALO_Ball_Main2;
	//a1->DisplaySub = ALO_Ball_Display;
	AL_Toy_Move_Register(a1, ALW_KIND_BALL);
}

void ALO_Ball_Hook() {
	__asm {
		push ebx
		call ALO_Ball_Main2_
		add esp,4
	}
}

DataArray(int, dword_1DC0F80, 0x1DC0F80, 1);
const int sub_530470Ptr = 0x530470;
ALW_ENTRY_WORK* sub_530470(int a1, int a2)
{
	ALW_ENTRY_WORK* result;
	__asm
	{
		mov edx, a1
		mov ebx, a2
		call sub_530470Ptr
		mov result, eax
	}
	return result;
}

void SaveToyPos() {
	ITEM_SAVE_INFO* v5;
	task* v6;
	ALW_ENTRY_WORK* v4 = 0;
	if (AL_IsGarden())
	{
		int v2 = dword_1DC0F80[6];
		if (v2)
		{
			while (1)
			{
				v4 = sub_530470(6, --v2);
				if (v4)
				{
					v5 = (ITEM_SAVE_INFO*)v4->pSaveInfo;
					v6 = v4->tp;
					if (v5)
					{
						if (v6)
						{
							break;
						}
					}
				}
			LABEL_25:
				if (!v2)
				{
					return;
				}
			}

			v5->pos = v6->twp->pos;
			v5->nbVisit = v6->twp->ang.y;
			if (AL_IsGarden())
			{
				v5->place = AL_GetStageNumber();
			}

			if (v6->twp->flag >= 0)
			{
				if (v5->pos.y >= -100.0)
				{
					CCL_HIT_INFO* v9 = CCL_IsHitKindEx(v6, CI_KIND_AL_STAGE_CHANGER);
					if (v9 && v9->hit_tp)
					{
						v5->place = 0;
					}
				}
				else 
					v5->place = 0;
			}
			else if (!AL_IsGarden())
			{
				v5->place = 0;
				___OutputDebugString( "trying to save item in invalid area");
			}
			else 
				v5->place = 0;
			goto LABEL_25;
		}
	}
}

//FunctionHook<void, NJS_POINT3*> ALO_BoxExecutor_Load_t(0x00580890);
//FunctionHook<void, NJS_POINT3*> ALO_TVExecutor_Load_t(0x0055CB90);
//FunctionHook<void, NJS_POINT3*> ALO_RadicaseExecutor_Load_t(0x0057CCA0);
UsercallFuncVoid(ALO_Horse_Load_t, (NJS_POINT3* pPos), (pPos), 0x00580F30, rEBX);

template <int Type, int Addr>
static void HookToyLoad() {
	static FunctionHook<void, NJS_POINT3*> ALO_Load_t(Addr);
	void( __cdecl* lambda_hook_func)(NJS_POINT3*) = [](NJS_POINT3* pPos) {
		ALO_Load_t.Original(pPos);
		AL_Toy_Move_Register(pLastToyTask, Type);
	};
	ALO_Load_t.Hook(lambda_hook_func);
}

void AL_Toy_Moveable_Init()
{
	// description of what we do here
	// 1. the nullsub_1 writecalls kill the ALW_Entry2 call, so we can then call it in the trampolines (in the future)
	// we nullsub it because we handle the ALW_Entry2 specially inside AL_Toy_Move_Register, and call it ourselves in 2.
	// 2. the WriteData's will be replaced with the trampolines, but for now, they replace the displaysub/mainsub to delay the AL_Toy_Move_Register call
	// 3. the "_Init_Hook" functions handle the actual "physics" init part of the toys, and we WriteCall the InitCollision calls
	// the horse is an edgecase, scroll down to see the explanation at the horse section
	// 4. the "_UpdateHook" functions hook the AddToCollisionList calls so we can run the physics update right before the function ends
	 
	// Ball
	// the ball is a special case, it already has moving so we only have to handle ALW_Entry stuff to save its position, and we don't even need to delay it
	// because they already do it?? (some objects in SA2 do this, where the mainsub they set is actually another "sub-init" thing)
	WriteCall((void*)0x0055D693, ALO_Ball_Hook);

	// Box
	
	WriteCall((void*)0x005808F4, nullsub_1); 
	//HookToyLoad<ALW_KIND_BOX>(ALO_BoxExecutor_Load_t);
	//HookToyLoad<ALW_KIND_BOX, 0x00580890>();
	WriteData((int*)0x058089E, (int)ALO_BoxExecutor_Main_); //box doesnt check cameradist, no fix needed, so we delay the mainsub not the displaysub
	WriteCall((void*)0x005808E4, AL_Box_Init_Hook); 
	WriteCall((void*)0x00580462, AL_Toy_UpdateHook);
	WriteCall((void*)0x58047F, AL_Toy_UpdateHook);
	WriteCall((void*)0x5804D5, AL_Toy_UpdateHook);
	WriteCall((void*)0x005804E4, AL_Toy_UpdateHook);

	// TV
	WriteCall((void*)0x0055CBF3, nullsub_1);
	//HookToyLoad<ALW_KIND_TV, 0x0055CB90>();
	//HookToyLoad<ALW_KIND_TV>(ALO_TVExecutor_Load_t);
	WriteData((int*)(0x0055CC3C - 4), (int)ALO_TVExecutor_Display_);
	WriteCall((void*)0x0055CBE2, AL_TV_Init_Hook);
	WriteCall((void*)0x0055C719, AL_Toy_UpdateHook);
	WriteCall((void*)0x55C99F, AL_Toy_UpdateHook);

	// radio
	WriteCall((void*)0x0057CD00, nullsub_1);
	//HookToyLoad<ALW_KIND_RADICASE, 0x0057CCA0>();
	//HookToyLoad<ALW_KIND_RADICASE>(ALO_RadicaseExecutor_Load_t);
	WriteData((int*)(0x0057CD2B - 4), (int)ALO_ToyDisplayHook<(task_exec)0x57CA80, 3>);
	WriteCall((void*)0x0057CCF2, AL_Radio_Init_Hook);
	WriteCall((void*)0x0057C9F5, AL_Toy_UpdateHook);
	WriteCall((void*)0x0057CA58, AL_Toy_UpdateHook);

	// horse
	WriteCall((void*)0x00580F19, nullsub_1);
	WriteData((int*)(0x00580F09 - 4), (int)ALO_HorseExecutor_Display_);
	WriteData<7>((char*)0x00580EDF, (char)0x90);
	WriteCall((void*)0x00580ECC, AL_Horse_Init);
	WriteCall((void*)0x00580EEF, nullsub_1);
	WriteCall((void*)0x00580C8F, AddToCollisionListHook);
}