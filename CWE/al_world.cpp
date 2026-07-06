#include "stdafx.h"
#include "al_world.h"
#include <Chao.h>
#include <unordered_map>
#include <FunctionHook.h>

ALW_ENTRY_WORK* __cdecl ALW_IsCommunication(task* a1)
{
	ALW_ENTRY_WORK* v1; // ecx
	ALW_ENTRY_WORK* result; // eax

	v1 = (ALW_ENTRY_WORK*)a1->fwp;
	if (!v1 || (result = v1->pCommu) == 0 || result->pCommu != v1)// check if interactingPtr is > 0 and not set to itself
	{
		result = 0;
	}
	return result;
}

ALW_ENTRY_WORK* ALW_IsCommunicationEx(task* tp, Uint16 category) {
    ALW_ENTRY_WORK* pEntry = GET_ALW_ENTRY_WORK(tp);
    if (pEntry) {
        ALW_ENTRY_WORK* pCommu = pEntry->pCommu;
        if (pCommu) {
            if (pCommu->pCommu == pEntry && pCommu->category == category)
                return pCommu;
        }
    }

    return NULL;
}

__int16 __cdecl ALW_RecieveCommand(task* a1)
{
	ALW_ENTRY_WORK* v1; // ecx
	__int16 result; // ax

	v1 = (ALW_ENTRY_WORK*)a1->fwp;
	result = 0;
	if (v1)
	{
		result = v1->command;
		v1->command = 0;
	}
	return result;
}
task* __cdecl ALW_GetLockOnTask(task* a1)
{
	ALW_ENTRY_WORK* v1; // eax
	ALW_ENTRY_WORK* v2; // eax
	task* result; // eax

	v1 = (ALW_ENTRY_WORK*)a1->fwp;
	if (!v1 || (v2 = v1->pLockOn) == 0 || (result = v2->tp) == 0)
	{
		result = 0;
	}
	return result;
}
signed int __cdecl ALW_SendCommand(task* a1, __int16 a2)
{
	ALW_ENTRY_WORK* v2; // eax

	v2 = ALW_IsCommunication(a1);
	if (!v2)
	{
		return 0;
	}
	v2->command = a2;
	return 1;
}

const int ALW_CommunicationOffPtr = 0x00530690;
void ALW_CommunicationOff(task* a1)
{
	__asm
	{
		mov eax, a1
		call ALW_CommunicationOffPtr
	}
}

void* __cdecl AL_GetItemSaveInfo(task* a1)
{
	ALW_ENTRY_WORK* v1; // eax
	void* result; // eax

	v1 = (ALW_ENTRY_WORK*)a1->fwp;
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
void __cdecl AL_ClearItemSaveInfoPtr(task* a1)
{
	ALW_ENTRY_WORK* v1; // eax

	v1 = (ALW_ENTRY_WORK*)a1->fwp;
	if (v1)
	{
		v1->pSaveInfo = 0;
	}
}

bool __cdecl ALW_IsHeld(task* a1)
{
	ALW_ENTRY_WORK* v1; // eax

	v1 = (ALW_ENTRY_WORK*)a1->fwp;
	return v1 && v1->flag & 2;
}

signed int __cdecl ALW_LockOn(task* a1, task* a2)
{
	ALW_ENTRY_WORK* v2; // eax
	ALW_ENTRY_WORK* v3; // ecx

	v2 = (ALW_ENTRY_WORK*)a1->fwp;
	v3 = (ALW_ENTRY_WORK*)a2->fwp;
	if (!v2 || !v3)
	{
		return 0;
	}
	v2->pLockOn = v3;
	return 1;
}

const int ALW_TurnToLockOnPtr = 0x00530510;
int ALW_TurnToLockOn(task* a1, int a2)
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
void ALW_CommunicationOn(task* a1, task* a2)
{
	__asm
	{
		mov eax, a1
		mov ecx, a2
		call sub_530640Ptr
	}
}

Bool ALW_AttentionOn(task* tp1, task* tp2) {
	static int CommuID = 0;

    ALW_ENTRY_WORK* pEntry1 = GET_ALW_ENTRY_WORK(tp1);
    ALW_ENTRY_WORK* pEntry2 = GET_ALW_ENTRY_WORK(tp2);
    if (pEntry1 && pEntry2) {
        pEntry1->pCommu = pEntry2;
        pEntry1->CommuID = CommuID;
        CommuID++;
        if (CommuID >= 0x100)
            CommuID = 0;

        return TRUE;
    }

    return FALSE;
}

ALW_ENTRY_WORK* __cdecl ALW_IsCommunicating(task* a1)
{
	ALW_ENTRY_WORK* v1; // ecx
	ALW_ENTRY_WORK* result; // eax

	v1 = (ALW_ENTRY_WORK*)a1->fwp;
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
al_perception_link* __cdecl AL_GetFoundTree(task* a1)
{
	chaowk* v1; // ecx
	int v2; // edx
	AL_PERCEPTION_INFO* v3; // ecx
	al_perception_link* result; // eax
	__int16 v5; // dx

	v1 = GET_CHAOWK(a1);
	v2 = v1->Perception.Tree.InSightFlag;
	v3 = &v1->Perception.Tree;
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
bool __cdecl ALW_IsSheAttentionOtherOne(task* a1, task* a2)
{
	ALW_ENTRY_WORK* v2; // eax
	ALW_ENTRY_WORK* v3; // eax
	bool result; // eax

	v2 = (ALW_ENTRY_WORK*)a2->fwp;
	result = 0;
	if (v2)
	{
		v3 = v2->pCommu;
		if (v3)
		{
			if (v3 != (ALW_ENTRY_WORK*)a1->fwp)
			{
				result = 1;
			}
		}
	}
	return result;
}

ALW_ENTRY_WORK* ALW_IsAttention(task* tp) {
    ALW_ENTRY_WORK* pEntry = GET_ALW_ENTRY_WORK(tp);

    if (pEntry) {
        return pEntry->pCommu;
	}

    return NULL;
}

Bool ALW_SetHeldOffset(task* tp, float offset) {
	ALW_ENTRY_WORK* pEntry = GET_ALW_ENTRY_WORK(tp);

	if (pEntry) {
		pEntry->offset = offset;
		return TRUE;
	}

	return FALSE;
}

Bool ALW_SetHeldRadius(task* tp, float radius) {
	ALW_ENTRY_WORK* pEntry = GET_ALW_ENTRY_WORK(tp);

	if (pEntry) {
		pEntry->radius = radius;
		return TRUE;
	}

	return FALSE;
}

bool ALW_LockOnPickedUp(task* a1)
{
	return (ALW_GetLockOnTask(a1) && ALW_GetLockOnTask(a1)->twp->flag < 0);
}

al_perception_link* __cdecl GetToyObjects(task* a1)
{
	chaowk* v1; // ecx
	int v2; // edx
	AL_PERCEPTION_INFO* v3; // ecx
	al_perception_link* result; // eax
	__int16 v5; // dx

	v1 = GET_CHAOWK(a1);
	v2 = v1->Perception.Toy.InSightFlag;
	v3 = &v1->Perception.Toy;
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
task* __cdecl AL_GetFoundToyTask(task* a1)
{
	al_perception_link* v1; // eax
	ALW_ENTRY_WORK* v2; // eax
	task* result; // eax

	v1 = GetToyObjects(a1);
	if (!v1 || (v2 = v1->pEntry) == 0 || (result = v2->tp) == 0)
	{
		result = 0;
	}
	return result;
}

const int GetChaoObjectPtr = 0x00530410;
task* GetChaoObject(int a1, int a2)
{
	task* val;
	__asm
	{
		mov eax, a1
		mov edi, a2
		call GetChaoObjectPtr
		mov val, eax
	}
	return val;
}

#define NB_MAX_WORLD_ENTRY 64
int nbWorldEntry[NB_CWE_ALW_CATEGORY] = { 0 };
ALW_ENTRY_WORK WorldEntryList[NB_CWE_ALW_CATEGORY][NB_MAX_WORLD_ENTRY];

static const std::unordered_map<ChaoItemCategory, size_t> MapRealCategoryToCWEWorld = {
	{ChaoItemCategory_Accessory, CWE_ALW_CATEGORY_ACCESSORY}
};

Bool CWE_ALW_Entry(ChaoItemCategory category, task* tp, Uint16 kind, void* pSaveInfo) {
	if (!MapRealCategoryToCWEWorld.contains(category)) {
		return FALSE;
	}

	const auto categoryIndex = MapRealCategoryToCWEWorld.at(category);

	int i = 0;
	do {
		if (!WorldEntryList[categoryIndex][i].tp) {
			WorldEntryList[categoryIndex][i].tp = tp;
			WorldEntryList[categoryIndex][i].kind = kind;
			WorldEntryList[categoryIndex][i].pSaveInfo = pSaveInfo;
			WorldEntryList[categoryIndex][i].category = category;

			nbWorldEntry[categoryIndex]++;
			tp->fwp = (ObjUnknownA*) & WorldEntryList[categoryIndex][i];

			return TRUE;
		}

		i++;
	} while (i <= NB_MAX_WORLD_ENTRY - 1);

	return FALSE;
}

void ALW_ResetEntry(ALW_ENTRY_WORK* pEntry) {
	if (!pEntry)
		return;

	pEntry->kind = 0;
	pEntry->flag = 0;
	pEntry->command = 0;
	pEntry->command_value = -1;
	pEntry->state = 0;

	pEntry->tp = NULL;
	pEntry->pCommu = NULL;
	pEntry->CommuID = -1;

	pEntry->pSaveInfo = 0;
}

int CWE_ALW_CountEntry(ChaoItemCategory category) {
	if (!MapRealCategoryToCWEWorld.contains(category)) {
		return NULL;
	}

	const auto categoryIndex = MapRealCategoryToCWEWorld.at(category);

	return nbWorldEntry[categoryIndex];
}

task* CWE_ALW_GetTaskCount(ChaoItemCategory category, Uint16 count) {
	if (!MapRealCategoryToCWEWorld.contains(category)) {
		return NULL;
	}

	const auto categoryIndex = MapRealCategoryToCWEWorld.at(category);

	int i;
	int j = 0;

	if (count > NB_MAX_WORLD_ENTRY - 1)
		return NULL;

	for (i = 0; i < nbWorldEntry[categoryIndex]; i++) {
		if (WorldEntryList[categoryIndex][i].tp) {
			if (count == j)
				return WorldEntryList[categoryIndex][i].tp;

			j++;
		}
	}

	return NULL;
}

Bool CWE_ALW_CancelEntry(task* tp) {
	auto* pEntry = GET_ALW_ENTRY_WORK(tp);

	if (!pEntry) return FALSE;

	// this is a hack to prevent the real ALW_CancelEntry code to try and remove it
	// incase any code was tacked on or will be tacked onto it later on or something
	task* pBackup = pEntry->tp;
	pEntry->tp = NULL;

	ALW_CancelEntry(tp);

	pEntry->tp = pBackup;

	if (pEntry->tp == tp) {
		ALW_ResetEntry(pEntry);
		nbWorldEntry[pEntry->category]--;
	}

	return TRUE;
}

void CWE_ALW_ClearEntry(void) {
	int i;

	for (i = 0; i < NB_CWE_ALW_CATEGORY; i++) {
		int j;

		nbWorldEntry[i] = 0;

		for (j = 0; j < NB_MAX_WORLD_ENTRY; j++) {
			ALW_ENTRY_WORK* pEntry = &WorldEntryList[i][j];
			pEntry->category = i;
			pEntry->num = j;
			ALW_ResetEntry(pEntry);
		}
	}
}

static void CWE_ALW_Create();
static FunctionHook<void> ALW_Create_Hook(0x530B80, CWE_ALW_Create);
static void CWE_ALW_Create() {
	ALW_Create_Hook.Original();

	CWE_ALW_ClearEntry();
}