#include "stdafx.h"
#include "al_save.h"
#include <cstdio>
#include "Chao.h"
#include <random>
#include "ChaoMain.h"
#include <vector>
//#include "usercall.h"
#include "rapidjson.h"
#include <document.h>
#include "save/save_serializable.h"
#include <filereadstream.h>
#include "prettywriter.h"
#include <filewritestream.h>
using namespace rapidjson;

std::vector< SaveFileAPIEntry> ModAPI_SaveAPI;

CWESaveFile cweSaveFile;

// also this is like the only place in the code that uses namespace for some reason
// (i really hate this part of the code lol)
namespace save {
	std::array<SAlItem, 10> CWE_PurchasedItems;

	char readBuffer[65536];
	char writeBuffer[65536];

	// this whole json version of the savefiles is extremely redundant
	// it should be replaced eventually (people won't be happy that a third savefile will show up for cwe
	// but it would be worth it)
	void LoadCWESave() {
		char filename[MAX_PATH];
		sprintf(filename, "%s_%s", (const char*)0x136604C, "CWEV1.json");

		FILE* fp = fopen(filename, "rb");
		if (fp) {
			FileReadStream is(fp, readBuffer, sizeof(readBuffer));

			Document d;
			d.ParseStream(is);

			LoadMember<SAlItem, 10>(d, CWE_PurchasedItems, "PurchasedItems");

			fclose(fp);
		}
	}

	void SaveCWESave() {
		char filename[MAX_PATH];
		sprintf(filename, "%s_%s", (const char*)0x136604C, "CWEV1.json");

		FILE* fp = fopen(filename, "wb");
		
		if (fp) {
			FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));
			PrettyWriter<FileWriteStream> writer(os);

			writer.StartObject();

			SaveMember<SAlItem, 10>(writer, CWE_PurchasedItems, "PurchasedItems");

			writer.EndObject();

			fclose(fp);
		}
	}
}

int GetTimeOfDay() 
{
	return cweSaveFile.TimeOfDay;
}

void SetTimeOfDay(int time)
{
	cweSaveFile.TimeOfDay = time;
}

int GetWeather()
{
	return cweSaveFile.Weather;
}

void SetWeather(int weather)
{
	cweSaveFile.Weather = weather;
}

SAlItem* GetMarketInventory(int category)
{
	return cweSaveFile.marketInventory[category];
}

int GetMarketInvSize(int category)
{
	return cweSaveFile.marketInventoryCount[category];
}

void ReadCWESaveFiles() {
	char strBuffer[MAX_PATH];

	//test json api
	save::LoadCWESave();

	for (size_t i = 0; i < ModAPI_SaveAPI.size(); i++)
	{
		sprintf(strBuffer, "%s_%s", (const char*)0x136604C, ModAPI_SaveAPI[i].suffix);
		FILE* f;
		fopen_s(&f, strBuffer, "rb");
		if (f)
		{
			fread_s(ModAPI_SaveAPI[i].pointer, ModAPI_SaveAPI[i].fileSize, ModAPI_SaveAPI[i].fileSize, 1, f);
			fclose(f);
		}
		else
			memset(ModAPI_SaveAPI[i].pointer, 0, ModAPI_SaveAPI[i].fileSize);
	}
}

int __cdecl ReadCWESaveFile(char* path, void* buffer, size_t size) {
	int ret = ReadSaveFileThing(path, buffer, size);
	ReadCWESaveFiles();
	return ret;
}
static void __declspec(naked) ReadCWESaveFileHook()
{
	__asm
	{
		push[esp + 04h] // size
		push edx // buffer
		push ecx // path

		// Call your __cdecl function here:
		call ReadCWESaveFile

		pop ecx // path
		pop edx // buffer
		add esp, 4 // size
		retn
	}
}

#include "usercall.h"
#include "al_stage.h"
auto WriteSaveFile = GenerateUsercallWrapper<int(*)(char* path, void* a2, int count)>(rEAX, 0x426760, rECX, rEDX, stack4);

void SaveCWESaveFiles() {
	char strBuffer[MAX_PATH];
	save::SaveCWESave();

	for (size_t i = 0; i < ModAPI_SaveAPI.size(); i++)
	{
		sprintf(strBuffer, "%s_%s", (const char*)0x136604C, ModAPI_SaveAPI[i].suffix);
		FILE* f;
		fopen_s(&f, strBuffer, "wb");
		if (f)
		{
			fwrite(ModAPI_SaveAPI[i].pointer, ModAPI_SaveAPI[i].fileSize, 1, f);
			fclose(f);
		}
	}
}

int __cdecl SaveCWESaveFile(char* path, void* a2, int count) {
	int ret = WriteSaveFile(path, a2, count);
	SaveCWESaveFiles();	
	return ret;	
}

static void __declspec(naked) SaveCWESaveFileHook()
{
	__asm
	{
		push[esp + 04h] // count
		push edx // a2
		push ecx // path

		// Call your __cdecl function here:
		call SaveCWESaveFile

		pop ecx // path
		pop edx // a2
		add esp, 4 // count
		retn
	}
}

ChaoData* GetFreeCWESlot()
{
	ChaoData* dataPtr = 0;
	for (int i = 0; i < 24; i++)
	{
		if (cweSaveFile.chaoParam[i].data.Type == 0)
		{
			dataPtr = &cweSaveFile.chaoParam[i];
			break;
		}
	}
	return dataPtr;
}

char* GetFreeChaoSlot()
{
	signed int v0; // esi
	
	v0 = 0;
	ChaoData* dataPtr = 0;
	for(int i = 0; i < 24; i++)
	{
		if (ChaoSlots[i].data.Type == 0)
		{
			dataPtr = &ChaoSlots[i];
			break;
		}
	}
	dataPtr = GetFreeCWESlot();
	if (dataPtr != 0)
	{
		memset(dataPtr, 0, sizeof(ChaoData));
		return (char*)dataPtr;
	}
	else 
		return 0;
}
int GetFreeChaoCWE(int a1)
{
	int count = 0;
	for (int i = 0; i < 24; i++)
	{
		if (cweSaveFile.chaoParam[i].data.Type && cweSaveFile.chaoParam[i].data.Garden == a1)
			count++;
	}
	return count;
}

int GetFreeChaoCWE2()
{
	int count = 0;
	for (int i = 0; i < 24; i++)
	{
		if (cweSaveFile.chaoParam[i].data.Type)
			count++;
	}
	return count;
}

int __cdecl sub_5319F0_(int a1)
{
	char* v1; // eax
	int v2; // edx
	int v3; // ecx

	v1 = &ChaoSlots[0].data.Garden;
	v2 = 0;
	v3 = 24;
	do
	{
		if (*(v1 - 1) && (unsigned __int8)* v1 == a1)
		{
			++v2;
		}
		v1 += 2048;
		--v3;
	}
	while (v3);

	return v2 + GetFreeChaoCWE(a1);
}
static void __declspec(naked) sub_5319F0()
{
	__asm
	{
		push esi // a1

		// Call your __cdecl function here:
		call sub_5319F0_

		pop esi // a1
		retn
	}
}
DataArray(BlackMarketItem, PurchasedInventory, 0x01DBEDA0, 6);
int sub_531A20()
{
	char* v0; // eax
	int v1; // edx
	int v2; // ecx
	int i; // eax
	
	v0 = (char*)&ChaoSlots[0].data.Type;
	v1 = 0;
	v2 = 24;
	do
	{
		if (*v0)
		{
			++v1;
		}
		v0 += 2048;
		--v2;
	} while (v2);
	v1 += GetFreeChaoCWE2();
	for (i = 0; i < *(int*)0x01DBEDAC; ++i)
	{
		if (PurchasedInventory[i].Category == 1)
		{
			++v1;
		}
	}
	if (!(ChaoGardensUnlocked & 0x10))
	{
		v1 += 7;
	}
	if (!(ChaoGardensUnlocked & 0x40))
	{
		v1 += 7;
	}
	return v1;
}
void SpawnCWEChao()
{
	SpawnAllChaoInGarden();
	if (AL_IsGarden())
	{
		for (int i = 0; i < 24; i++)
		{
			ChaoData* chao = &cweSaveFile.chaoParam[i];
			if (chao->data.Garden == AL_GetStageNumber()
				&& chao != *(ChaoData**)0x01A5CA5C)
				//&& chao != (ChaoData*)GBAManager_GetChaoDataPointer())
			{
				NJS_VECTOR* spawn = &ProbablyChaoSpawnPoints[16 * AL_GetStageNumber()
					+ (int)(njRandom() * 15.9f) - 16];

				if (chao->data.Type)
				{
					if (chao->data.Type == 1)
					{
						CreateChao(chao, 0, 0, spawn, NJM_DEG_ANG(njRandom() * 360.f));
					}
					else if (*(char*)&chao->data.InKindergarten == -1)
					{
						ObjectMaster* task = CreateChao(
							chao,
							0,
							0,
							spawn,
							NJM_DEG_ANG(njRandom() * 360.f));
						if (task)
						{
							if (AL_EmotionGetValue(task, EM_ST_SLEEP_DEPTH))
							{
								AL_SetBehavior(task, (BHV_FUNC)0x54EF10);
							}
						}
					}
				}
			}
		}
	}
}

void* AL_GetSpecialItemSave()
{
	for (int i = 0; i < 30; i++)
	{
		if (cweSaveFile.specialItems[i].Type == -1)
			return &cweSaveFile.specialItems[i];
	}
	return 0;
}


void AL_SaveInit()
{
#ifdef CHAO_48
	WriteJump((void*)0x531AA0, GetFreeChaoSlot);
	WriteCall((void*)0x0054C9C2, SpawnCWEChao);

	//has enough space to create egg
	WriteJump((void*)0x5319F0, sub_5319F0);

	//black market limit
	WriteJump((void*)0x531A20, sub_531A20);
	WriteData((char*)0x0058AF4B, (char)48);
#endif
	// no mem leftover (just to be safe)
	memset(&cweSaveFile, 0, sizeof(CWESaveFile));
	for (int i = 0; i < 30; i++)
		cweSaveFile.specialItems[i].Type = -1;
	WriteCall((void*)0x0052DF4C, ReadCWESaveFileHook);

	//i hook every single save file load, which also means the second memory card load for karate/transporter which fails on PC
	//however it causes the cwe savefile to reload which can delete items obtained before saving
	//hence this being commented out
	//lot of explanation for something commented out
	//WriteCall((void*)0x00532653, ReadCWESaveFileHook);

	WriteCall((void*)0x52E2AC, SaveCWESaveFileHook);
	WriteCall((void*)0x52FECB, SaveCWESaveFileHook);
	WriteCall((void*)0x532483, SaveCWESaveFileHook);
}