#pragma once


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

enum {
	ALW_CATEGORY_CHAO = 0x0,
	ALW_CATEGORY_EGG = 0x1,
	ALW_CATEGORY_MINIMAL = 0x2,
	ALW_CATEGORY_FRUIT = 0x3,
	ALW_CATEGORY_TREE = 0x4,
	ALW_CATEGORY_GROWTREE = 0x5,
	ALW_CATEGORY_TOY = 0x6,
	ALW_CATEGORY_SEED = 0x7,
	ALW_CATEGORY_ACCESSORY = 0x8,
	ALW_CATEGORY_SOUND = 0x8, // unused
	ALW_CATEGORY_MASK = 0x9,
	ALW_CATEGORY_SPECIAL = 0xA,
	NB_CATEGORY = 0xB,
};

ObjectMaster* GetChaoObject(int a1, int a2);
ObjectMaster* __cdecl AL_GetFoundToyTask(ObjectMaster* a1);

ThiscallFunctionPointer(float, ALW_CalcDistFromLockOn, (ObjectMaster* a1), 0x00530580);
signed int __cdecl ALW_LockOn(ObjectMaster* a1, ObjectMaster* a2);
al_entry_work* __cdecl ALW_IsCommunication(ObjectMaster* a1);
void ALW_CommunicationOff(ObjectMaster* a1);
__int16 __cdecl ALW_RecieveCommand(ObjectMaster* a1);
ObjectMaster* __cdecl ALW_GetLockOnTask(ObjectMaster* a1);
signed int __cdecl ALW_SendCommand(ObjectMaster* a1, __int16 a2);
int ALW_TurnToLockOn(ObjectMaster* a1, int a2);
void* __cdecl AL_GetItemSaveInfo(ObjectMaster* a1);
void __cdecl AL_ClearItemSaveInfo(ITEM_SAVE_INFO* a1);
void __cdecl AL_ClearItemSaveInfoPtr(ObjectMaster* a1);
bool __cdecl ALW_IsHeld(ObjectMaster* a1);
void ALW_CommunicationOn(ObjectMaster* a1, ObjectMaster* a2);
al_entry_work* __cdecl ALW_IsCommunicating(ObjectMaster* a1);
int __cdecl ALW_CountEntry(unsigned __int16 index);
al_perception_link* __cdecl AL_GetFoundTree(ObjectMaster* a1);
bool __cdecl ALW_IsSheAttentionOtherOne(ObjectMaster* a1, ObjectMaster* a2);
al_entry_work* ALW_IsAttention(ObjectMaster* tp);
bool ALW_LockOnPickedUp(ObjectMaster* a1);