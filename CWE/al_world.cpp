#include "stdafx.h"
#include "al_world.h"

al_entry_work* __cdecl ALW_IsCommunication(ObjectMaster* a1)
{
	al_entry_work* v1; // ecx
	al_entry_work* result; // eax

	v1 = (al_entry_work*)a1->UnknownA_ptr;
	if (!v1 || (result = v1->pCommu) == 0 || result->pCommu != v1)// check if interactingPtr is > 0 and not set to itself
	{
		result = 0;
	}
	return result;
}
__int16 __cdecl ALW_RecieveCommand(ObjectMaster* a1)
{
	al_entry_work* v1; // ecx
	__int16 result; // ax

	v1 = (al_entry_work*)a1->UnknownA_ptr;
	result = 0;
	if (v1)
	{
		result = v1->command;
		v1->command = 0;
	}
	return result;
}
ObjectMaster* __cdecl ALW_GetLockOnTask(ObjectMaster* a1)
{
	al_entry_work* v1; // eax
	al_entry_work* v2; // eax
	ObjectMaster* result; // eax

	v1 = (al_entry_work*)a1->UnknownA_ptr;
	if (!v1 || (v2 = v1->pLockOn) == 0 || (result = v2->tp) == 0)
	{
		result = 0;
	}
	return result;
}
signed int __cdecl ALW_SendCommand(ObjectMaster* a1, __int16 a2)
{
	al_entry_work* v2; // eax

	v2 = ALW_IsCommunication(a1);
	if (!v2)
	{
		return 0;
	}
	v2->command = a2;
	return 1;
}

const int ALW_CommunicationOffPtr = 0x00530690;
void ALW_CommunicationOff(ObjectMaster* a1)
{
	__asm
	{
		mov eax, a1
		call ALW_CommunicationOffPtr
	}
}

void* __cdecl AL_GetItemSaveInfo(ObjectMaster* a1)
{
	al_entry_work* v1; // eax
	void* result; // eax

	v1 = (al_entry_work*)a1->UnknownA_ptr;
	if (v1)
	{
		result = v1->pSaveInfo;
	}
	else
	{
		result = 0;
	}
	return result;
}
void __cdecl AL_ClearItemSaveInfoPtr(ObjectMaster* a1)
{
	al_entry_work* v1; // eax

	v1 = (al_entry_work*)a1->UnknownA_ptr;
	if (v1)
	{
		v1->pSaveInfo = 0;
	}
}
void __cdecl AL_ClearItemSaveInfo(ITEM_SAVE_INFO* a1)
{
	a1->Type = -1;
	a1->position.x = 0;
	a1->position.y = 0;
	a1->position.z = 0;
	a1->Garden = -1;
	a1->Age = 0;
	a1->Size = 0;
}
bool __cdecl ALW_IsHeld(ObjectMaster* a1)
{
	al_entry_work* v1; // eax

	v1 = (al_entry_work*)a1->UnknownA_ptr;
	return v1 && v1->flag & 2;
}

signed int __cdecl ALW_LockOn(ObjectMaster* a1, ObjectMaster* a2)
{
	al_entry_work* v2; // eax
	al_entry_work* v3; // ecx

	v2 = (al_entry_work*)a1->UnknownA_ptr;
	v3 = (al_entry_work*)a2->UnknownA_ptr;
	if (!v2 || !v3)
	{
		return 0;
	}
	v2->pLockOn = v3;
	return 1;
}

const int ALW_TurnToLockOnPtr = 0x00530510;
int ALW_TurnToLockOn(ObjectMaster* a1, int a2)
{
	int retur;
	__asm
	{
		mov eax, a1
		push a2
		call ALW_TurnToLockOnPtr
		add esp, 4
		mov retur, eax
	}
	return retur;
}

const int sub_530640Ptr = 0x530640;
void ALW_CommunicationOn(ObjectMaster* a1, ObjectMaster* a2)
{
	__asm
	{
		mov eax, a1
		mov ecx, a2
		call sub_530640Ptr
	}
}
al_entry_work* __cdecl ALW_IsCommunicating(ObjectMaster* a1)
{
	al_entry_work* v1; // ecx
	al_entry_work* result; // eax

	v1 = (al_entry_work*)a1->UnknownA_ptr;
	if (!v1 || (result = v1->pCommu) == 0 || result->pCommu != v1)// check if interactingPtr is > 0 and not set to itself
	{
		result = 0;
	}
	return result;
}
DataArray(int, NumberOfChaoGlobal, 0x1DC0F80,12);
int __cdecl ALW_CountEntry(unsigned __int16 index)
{
	return NumberOfChaoGlobal[index];
}
al_perception_link* __cdecl AL_GetFoundTree(ObjectMaster* a1)
{
	ChaoData1* v1; // ecx
	int v2; // edx
	AL_PERCEPTION_INFO* v3; // ecx
	al_perception_link* result; // eax
	__int16 v5; // dx

	v1 = a1->Data1.Chao;
	v2 = v1->TreeObjects.InSightFlag;
	v3 = &v1->TreeObjects;
	result = 0;
	if (v2)
	{
		v5 = v3->NearestNum;
		if (v5 >= 0)
		{
			result = &v3->field_18[v5];
		}
	}
	return result;
}
bool __cdecl ALW_IsSheAttentionOtherOne(ObjectMaster* a1, ObjectMaster* a2)
{
	al_entry_work* v2; // eax
	al_entry_work* v3; // eax
	bool result; // eax

	v2 = (al_entry_work*)a2->UnknownA_ptr;
	result = 0;
	if (v2)
	{
		v3 = v2->pCommu;
		if (v3)
		{
			if (v3 != (al_entry_work*)a1->UnknownA_ptr)
			{
				result = 1;
			}
		}
	}
	return result;
}
al_entry_work* ALW_IsAttention(ObjectMaster* tp)
{
	al_entry_work* v1; // r11
	al_entry_work* result; // r3

	v1 = (al_entry_work*)tp->UnknownA_ptr;
	if (v1)
		result = v1->pCommu;
	else
		result = 0;
	return result;
}

bool ALW_LockOnPickedUp(ObjectMaster* a1)
{
	return (ALW_GetLockOnTask(a1) && ALW_GetLockOnTask(a1)->Data1.Entity->Status < 0);
}

al_perception_link* __cdecl GetToyObjects(ObjectMaster* a1)
{
	ChaoData1* v1; // ecx
	int v2; // edx
	AL_PERCEPTION_INFO* v3; // ecx
	al_perception_link* result; // eax
	__int16 v5; // dx

	v1 = a1->Data1.Chao;
	v2 = v1->ToyObjects.InSightFlag;
	v3 = &v1->ToyObjects;
	result = 0;
	if (v2)
	{
		v5 = v3->NearestNum;
		if (v5 >= 0)
		{
			result = &v3->field_18[v5];
		}
	}
	return result;
}
ObjectMaster* __cdecl AL_GetFoundToyTask(ObjectMaster* a1)
{
	al_perception_link* v1; // eax
	al_entry_work* v2; // eax
	ObjectMaster* result; // eax

	v1 = GetToyObjects(a1);
	if (!v1 || (v2 = v1->pEntry) == 0 || (result = v2->tp) == 0)
	{
		result = 0;
	}
	return result;
}

const int GetChaoObjectPtr = 0x00530410;
ObjectMaster* GetChaoObject(int a1, int a2)
{
	ObjectMaster* val;
	__asm
	{
		mov eax, a1
		mov edi, a2
		call GetChaoObjectPtr
		mov val, eax
	}
	return val;
}
