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

// HACK: so for the whole toy registering ordeal, the original version delayed the displaysub (because the displaysub needed the entrywork)
// and we couldn't hook the actual ALW_Entry function and do all our business there (because the position and angle set in the load functions  
// runs **after* the ALW_Entry call), so this is an ugly workaround
// we set this pointer when AL_Toy_Move_Init gets called, and then inside our FunctionHook/UsercallHooks, we use it to get the currently being spawned toy's ObjectMaster ptr
// and call ALW_Entry there
static ObjectMaster* pLastToyTask = NULL;

const int MOV_ControlPtr = 0x00796780;
void MOV_Control(ObjectMaster* eax0)
{
	__asm
	{
		mov eax, eax0
		call MOV_ControlPtr
	}
}

const int sub_54B230Ptr = 0x54B230;
void sub_54B230(ObjectMaster* eax0, float a2)
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
void ObjectMovableInitialize(EntityData1* a1, int a2)
{
	__asm
	{
		mov eax, a1
		mov edx, a2
		call sub_47D9E0Ptr
	}
}

const int MoveFunc2Ptr = 0x00798300;
void MoveFunc2(ObjectMaster* a1)
{
	__asm
	{
		mov esi, a1
		call MoveFunc2Ptr
	}
}
static const void* const AddToGlobalChaoThingMaybePtr_ = (void*)0x530750;
static inline signed int AddToGlobalChaoThingMaybe_(unsigned __int16 a1, ObjectMaster* obj, __int16 a3, ChaoData* data)
{
	signed int result;
	__asm
	{
		push dword ptr [data]
		push dword ptr [a3]
		mov ebx, [obj]
		mov cx, [a1]
		call AddToGlobalChaoThingMaybePtr_
		add esp, 8
		mov result, eax
	}
	return result;
}
void AL_Toy_Move_Register(ObjectMaster* obj, __int16 a3)
{
	ITEM_SAVE_INFO* info = NULL;
	
	if(AL_IsGarden()) {
		int saveIndex = a3;
		saveIndex *= 3;
		saveIndex += (AL_GetStageNumber() - 1);

		info = &cweSaveFile.cweToyInfo[saveIndex];
		if (info->Garden == 0) {
			info->Garden = AL_GetStageNumber();
			info->position = obj->Data1.Entity->Position;
			info->Age = obj->Data1.Entity->Rotation.y;
		}
	}
	else {
		PrintDebug("AL_Toy_Move_Register: invalid toy area");
	}

	AddToGlobalChaoThingMaybe_(6, obj, a3, (ChaoData*)info);
	if (info) {
		obj->Data1.Entity->Position = info->position;
		obj->Data1.Entity->Rotation.y = info->Age;
	}
}

AL_TOY_MOVE* GetToyMove(ObjectMaster* a1)
{
	return (AL_TOY_MOVE*)((int)a1->EntityData2 + 0x26C);
}

void AL_Toy_Move_Update(ObjectMaster *a1)
{
	AL_TOY_MOVE* toyMove = GetToyMove(a1);
	//todo: "nextaction" for collision changes?
	switch (toyMove->state)
	{
	case TOY_MOVE_IDLE:

		if (toyMove->flag == 0)
		{
			CCL_Disable(a1, 0);
			CCL_Disable(a1, 1);
			CCL_Disable(a1, 2);
			toyMove->flag++;
		}

		//run collision for 30 frames, after that become static
		MOV_Control(a1);
		MoveFunc2(a1);
		toyMove->timer++;
		if (toyMove->timer > 30)
		{
			toyMove->timer = 0;
			toyMove->flag = 0;
			toyMove->state = TOY_MOVE_STATIC;
		}
		break;
	case TOY_MOVE_STATIC:

		if (toyMove->flag == 0)
		{
			CCL_Enable(a1, 0);
			CCL_Enable(a1, 1);
			CCL_Disable(a1, 2);
			toyMove->flag++;
		}

		//if not picked up
		if (a1->Data1.Entity->Status >= 0)
		{
			CollisionInfo* v8 = a1->Data1.Entity->Collision;
			if (v8)
			{
				//if the object is touched, start running collision
				if ((v8->word4 & 0x10) != 0)
				{
					toyMove->timer = 0;
					toyMove->flag = 0;
					toyMove->state = TOY_MOVE_DYNAMIC;
				}
			}
		}
		else
		{
			//if it is picked up, run pickup
			toyMove->timer = 0;
			toyMove->flag = 0;
			toyMove->state = TOY_MOVE_HOLDP;
		}
		break;
	case TOY_MOVE_DYNAMIC:
		if (toyMove->flag == 0)
		{
			CCL_Enable(a1, 0);
			CCL_Enable(a1, 1);
			CCL_Disable(a1, 2);
			toyMove->flag++;
		}
		//water handler
		sub_54B230(a1, toyMove->floatVal);

		if (a1->Data1.Entity->Status & 1)
		{
			NJS_VECTOR veloVec;
			veloVec.x = a1->EntityData2->velocity.x;
			veloVec.y = 0.0;
			veloVec.z = a1->EntityData2->velocity.z;
			//if it has velocity, reset timer to 0
			if (njScalor(&veloVec) >= 0.01f)
			{
				toyMove->timer = 0;
			}
			else
			{
				//if it doesnt have velocity start running timer
				//if the the timer reaches 30, become static again
				toyMove->timer++;
				if (toyMove->timer > 30)
				{
					UnknownData2* v8 = a1->EntityData2;
					v8->velocity.x = 0.0;
					v8->velocity.y = 0.0;
					v8->velocity.z = 0.0;
					v8->speed.x = 0.0;
					v8->speed.y = 0.0;
					v8->speed.z = 0.0;
					toyMove->flag = 0;
					toyMove->timer = 0;
					toyMove->state = TOY_MOVE_STATIC;
				}
			}
		}
		//if picked up
		if (a1->Data1.Entity->Status < 0)
		{
			toyMove->flag = 0;
			toyMove->timer = 0;
			toyMove->state = TOY_MOVE_HOLDP;
		}
		MOV_Control(a1);
		MoveFunc2(a1);
		break;
	case TOY_MOVE_HOLDP:

		if (toyMove->flag == 0)
		{
			CCL_Disable(a1, 0);
			CCL_Disable(a1, 1);
			CCL_Disable(a1, 2);
			toyMove->flag++;
		}
		ALW_CommunicationOff(a1);
		a1->Data1.Entity->Rotation.y = 0x4000 - MainCharObj1[0]->Rotation.y;
		//if it gets put down, go back to dynamic
		if (a1->Data1.Entity->Status >= 0)
		{
			toyMove->timer = 0;
			toyMove->flag = 0;
			toyMove->state = TOY_MOVE_DYNAMIC;
		}
		break;
	}

	a1->EntityData2->some_vector = a1->Data1.Entity->Position;
	
}
UnknownData2* __cdecl AllocateUnknownData2New(ObjectMaster* obj)
{
	UnknownData2* data2; // esi

	data2 = (UnknownData2*)AllocateArray(0x26C + sizeof(AL_TOY_MOVE), 1, (char*)"..\\..\\src\\move.c", 64);
	obj->EntityData2 = data2;                   // different offset than SADX

	data2->field_B4 = 3.0f;
	data2->field_28 = 256;                      // different offset than SADX
	data2->field_B8 = 2.2f;	
	data2->field_BC = -3.0f;
	data2->field_C0 = 40.0f;
	data2->field_C4 = 0.80000001f;
	data2->field_C8 = 0.89999998f;
	data2->field_CC = 0.2f;
	data2->field_D0 = 0.80000001f;
	data2->field_AC = 3.0f;
	return data2;
}

void AL_Toy_Move_Init(ObjectMaster* p, CCL_INFO* col)
{
	UnknownData2* mov = AllocateUnknownData2New(p);

	// this pointer's purpose is explained at the declaration
	pLastToyTask = p;

	mov->gravity = -0.05f;
	mov->field_AC = 3.0f;
	mov->field_C8 = 0.7f;
	mov->field_D0 = 0.7f;
	mov->field_30 = 3;
	mov->field_40 |= 8;

	GetToyMove(p)->floatVal = 1.0f;

	CCL_INFO collisions[3] = { 0 };
	collisions[0] = pickupableColli;
	collisions[1] = *(CCL_INFO*)0x008A76F8;
	collisions[2] = *col;

	collisions[1].push = 0x77;
	collisions[1].attr = 0x8000;

	collisions[1].form = collisions[2].form;
	collisions[1].center = collisions[2].center;
	collisions[1].a = collisions[2].a;
	collisions[1].b = collisions[2].b;
	collisions[1].c = collisions[2].c;
	collisions[1].d = collisions[2].d;

	InitCollision(p, (CollisionData*)collisions, 3, 5);
	if (p->EntityData2)
	{
		ObjectMovableInitialize(p->Data1.Entity, 10);
	}
}

void __cdecl AddToColliListToy(ObjectMaster* a1)
{
	AL_Toy_Move_Update(a1);

	//if (a1->Data1.Entity->Action == 0)
	//	a1->Data1.Entity->Status |= 0x240u;
	//else
	//	a1->Data1.Entity->Status &= ~0x240u;

	AddToCollisionList(a1);
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

void __cdecl AL_TV_Init(ObjectMaster* a1)
{
	AL_Toy_Move_Init(a1, &stru_8A5C10);
	a1->EntityData2->field_AC = 1.75f;
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

void __cdecl AL_Toy_Update(ObjectMaster* a1)
{
	AL_Toy_Move_Update(a1);

	if (!ALW_IsAttention(a1))
		a1->Data1.Entity->Status |= 0x240u;
	else
		a1->Data1.Entity->Status &= ~0x240u;


	AddToCollisionList(a1);
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

void __cdecl AL_Box_Init(ObjectMaster* a1)
{
	AL_Toy_Move_Init(a1, &ALO_BoxExecutor_collision);
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

void __cdecl AL_Radio_Init(ObjectMaster* a1)
{
	AL_Toy_Move_Init(a1, &RadioCol);
	a1->EntityData2->field_AC = 1.6f;
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

void __cdecl AL_Horse_Init(ObjectMaster* a1)
{
	AL_Toy_Move_Init(a1, &ALO_Horse_collision);
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

void __cdecl ALO_BoxExecutor_Main_(ObjectMaster* a1)
{
	a1->MainSub = ALO_BoxExecutor_Main;
	AL_Toy_Move_Register(a1, ALW_KIND_BOX);
}

void __cdecl ALO_TVExecutor_Display_(ObjectMaster *a1)
{
	a1->DisplaySub = ALO_TVExecutor_Display;
	AL_Toy_Move_Register(a1, ALW_KIND_TV);
	ALO_TVExecutor_Display(a1);
}

ObjectFunc(ALO_HorseExecutor_Display, 0x00580CA0);
void __cdecl ALO_HorseExecutor_Display_(ObjectMaster* a1)
{
	a1->DisplaySub = ALO_HorseExecutor_Display;
	AL_Toy_Move_Register(a1, ALW_KIND_HORSE);
	ALO_HorseExecutor_Display(a1);
}

void __cdecl ALO_RadicaseExecutor_Display_(ObjectMaster* a1)
{
	a1->DisplaySub = ALO_RadicaseExecutor_Display;
	AL_Toy_Move_Register(a1, ALW_KIND_RADICASE); 
	ALO_RadicaseExecutor_Display(a1);
}

template<ObjectFuncPtr func, int index>
void __cdecl ALO_ToyDisplayHook(ObjectMaster* tp) {
	tp->DisplaySub = func;
	AL_Toy_Move_Register(tp, index);
	func(tp);
}

void __cdecl ALO_Ball_Main2_(ObjectMaster* a1)
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
al_entry_work* sub_530470(int a1, int a2)
{
	al_entry_work* result;
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
	ObjectMaster* v6;
	al_entry_work* v4 = 0;
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

			v5->position = v6->Data1.Entity->Position;
			v5->Age = v6->Data1.Entity->Rotation.y;
			if (AL_IsGarden())
			{
				v5->Garden = AL_GetStageNumber();
			}

			if (v6->Data1.Entity->Status >= 0)
			{
				if (v5->position.y >= -100.0)
				{
					c_colli_hit_info* v9 = CCL_IsHitKindEx(v6, CI_KIND_AL_STAGE_CHANGER);
					if (v9 && v9->hit_twp)
					{
						v5->Garden = 0;
					}
				}
				else 
					v5->Garden = 0;
			}
			else if (!AL_IsGarden())
			{
				v5->Garden = 0;
				PrintDebug( "trying to save item in invalid area");
			}
			else 
				v5->Garden = 0;
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
	WriteData((int*)(0x0057CD2B - 4), (int)ALO_ToyDisplayHook<(ObjectFuncPtr)0x57CA80, 3>);
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