#pragma once

enum {
	CWE_ALW_CATEGORY_ACCESSORY,
	NB_CWE_ALW_CATEGORY
};


#pragma pack(push, 8)
struct ITEM_SAVE_INFO
{
	__int16 Type;
	__int16 Garden;
	__int16 Size;
	__int16 Age;
	NJS_VECTOR position;
};
#pragma pack(pop)

enum 
{
	ALW_CMD_ERROR = 0xFFFFFFFF,
	ALW_CMD_NONE = 0x0,
	ALW_CMD_GO = 0x1,
	ALW_CMD_EAT = 0x2,
	ALW_CMD_CHANGE = 0x3,
	ALW_CMD_FINISH = 0x4,
	ALW_CMD_PLANTED = 0x5,
	ALW_CMD_BYE = 0x6,
};

Bool CWE_ALW_Entry(ChaoItemCategory category, task* tp, Uint16 kind, void* pSaveInfo);
Bool CWE_ALW_CancelEntry(task* tp);
task* CWE_ALW_GetTaskCount(ChaoItemCategory category, Uint16 count);
int CWE_ALW_CountEntry(ChaoItemCategory category);
void CWE_ALW_ClearEntry(void);

void ALW_ResetEntry(al_entry_work* pEntry);

task* GetChaoObject(int a1, int a2);
task* __cdecl AL_GetFoundToyTask(task* a1);

Bool ALW_SetHeldOffset(task* tp, float offset);
Bool ALW_SetHeldRadius(task* tp, float radius);

Bool ALW_AttentionOn(task* tp1, task* tp2);
al_entry_work* ALW_IsAttention(task* tp);

ThiscallFunctionPointer(float, ALW_CalcDistFromLockOn, (task* a1), 0x00530580);
signed int __cdecl ALW_LockOn(task* a1, task* a2);
al_entry_work* __cdecl ALW_IsCommunication(task* a1);
void ALW_CommunicationOff(task* a1);
__int16 __cdecl ALW_RecieveCommand(task* a1);
task* __cdecl ALW_GetLockOnTask(task* a1);
signed int __cdecl ALW_SendCommand(task* a1, __int16 a2);
int ALW_TurnToLockOn(task* a1, int a2);
void* __cdecl AL_GetItemSaveInfo(task* a1);
void __cdecl AL_ClearItemSaveInfoPtr(task* a1);
bool __cdecl ALW_IsHeld(task* a1);
void ALW_CommunicationOn(task* a1, task* a2);
al_entry_work* __cdecl ALW_IsCommunicating(task* a1);
int __cdecl ALW_CountEntry(unsigned __int16 index);
al_perception_link* __cdecl AL_GetFoundTree(task* a1);
bool __cdecl ALW_IsSheAttentionOtherOne(task* a1, task* a2);
al_entry_work* ALW_IsAttention(task* tp);
bool ALW_LockOnPickedUp(task* a1);