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
#include "save/save_item.h"
#include <filereadstream.h>
#include "prettywriter.h"
#include <filewritestream.h>
#include <al_daynight.h>
#include <api/api_metadata.h>

using namespace rapidjson;

std::vector< SaveFileAPIEntry> ModAPI_SaveAPI;

CWESaveFile cweSaveFile;

static struct {
	int category;
	const char* key;
} CategoryKeyValuePairs[] = {
	{ALW_CATEGORY_EGG, "egg"},
	{ALW_CATEGORY_FRUIT, "fruit"},
	{ALW_CATEGORY_SEED, "seed"},
	{ALW_CATEGORY_MASK, "hat"},
	{ALW_CATEGORY_ACCESSORY, "accessory"},
	{ALW_CATEGORY_SPECIAL, "special"},
	{ALW_CATEGORY_THEME, "menu_theme"}
};

// also this is like the only place in the code that uses namespace for some reason
// (i really hate this part of the code lol)
namespace save {
	// todo: refactor to be static and create helper function that returns span
	std::array<SAlItemCwe, 10> CWE_PurchasedItems;

	// yes, i know that we already have an SAlItem with that terrible awful "save_serializable" stuff
	// for now i'll keep the new one here, i don't plan to support that system
	// with how infrequently we add stuff here, its enough to just do it the ugly way
	static bool ReadSAlItem(const rapidjson::Value& value, SAlItemCwe& outItem) {
		const char* categoryStr = value["category"].GetString();
		outItem.mCategory = -1;
		for (size_t i = 0; i < _countof(CategoryKeyValuePairs); i++) {
			const auto& pair = CategoryKeyValuePairs[i];
			if (!strcmp(categoryStr, pair.key)) {
				outItem.mCategory = pair.category;
				break;
			}
		}

		if (outItem.mCategory == -1) {
			return false;
		}

		if (value.HasMember("id")) {
			char id[METADATA_ID_SIZE];
			strncpy_s(id, value["id"].GetString(), sizeof(id));

			outItem.mType = ItemMetadata::Get()->GetIndex(outItem.mCategory, id);
			if (outItem.mType == -1) {
				return false;
			}
		}
		else {
			outItem.mType = Uint16(value["type"].GetInt());
		}

		return true;
	}

	template <typename T>
	static bool SaveSAlItem(rapidjson::PrettyWriter<T>& writer, const SAlItemCwe& item) {
		const char* categoryString = NULL;
		for (size_t i = 0; i < _countof(CategoryKeyValuePairs); ++i) {
			const auto& pair = CategoryKeyValuePairs[i];
			if (item.mCategory == pair.category) {
				categoryString = pair.key;
				break;
			}
		}

		if (!categoryString) {
			return false;
		}

		writer.StartObject();

		writer.Key("category");
		writer.String(categoryString);

		char id[METADATA_ID_SIZE];
		bool foundID = ItemMetadata::Get()->GetID(item.mCategory, item.mType, id);

		if (foundID) {
			writer.Key("id");
			writer.String(id);
		}
		else {
			writer.Key("type");
			writer.Int(item.mType);
		}

		writer.EndObject();

		return true;
	}

	// this whole json version of the savefiles is extremely redundant
	// it should be replaced eventually (people won't be happy that a third savefile will show up for cwe
	// but it would be worth it)
	void LoadCWESave() {
		ClearAllItemSave();
		
		char filename[MAX_PATH];
		sprintf(filename, "%s_%s", (const char*)0x136604C, "CWEV1.json");

		FILE* fp = fopen(filename, "rb");
		if (fp) {
			fseek(fp, 0L, SEEK_END);
			size_t sz = ftell(fp);

			// rapidjson expects a buffer with size of atleast 4
			sz = max(sz, 4);
			std::unique_ptr<char[]> readBuffer(new char[sz]);

			fseek(fp, 0, SEEK_SET);

			FileReadStream is(fp, readBuffer.get(), sz);

			Document d;
			d.ParseStream(is);

			LoadMember<SAlItemCwe, 10>(d, CWE_PurchasedItems, "PurchasedItems");

			if (d.HasMember("daynight")) {
				const auto& daynightMember = d["daynight"];

				AL_DayNightCycle_GetSaveCurrentDay() = daynightMember["day"].GetUint();
				AL_DayNightCycle_GetSaveTime() = daynightMember["time"].GetFloat();
				AL_DayNightCycle_GetSaveCurrentPhase() = daynightMember["currentPhase"].GetUint();
				AL_DayNightCycle_GetSaveNextDayCloudy() = daynightMember["nextDayCloudy"].GetBool();
			}

			if (d.HasMember("market_inventory")) {
				const auto& market = d["market_inventory"];

				for (size_t i = 0; i < _countof(CategoryKeyValuePairs); ++i) {
					const auto& pair = CategoryKeyValuePairs[i];
					auto& inv = cweSaveFile.marketInventory[pair.category];
					auto& count = cweSaveFile.marketInventoryCount[pair.category];

					if (market.HasMember(pair.key)) {
						const auto& list = market[pair.key].GetArray();
						const auto size = list.Size();

						count = 0;

						for (size_t j = 0; j < size; ++j) {
							if (count >= _countof(inv)) {
								break;
							}

							ReadSAlItem(list[j], inv[count++]);
						}
					}
				}
			}

			if (d.HasMember("purchased_items")) {
				const auto& items = d["purchased_items"].GetArray();

				cweSaveFile.purchasedItemCount = 0;

				for (const auto& item : items) {
					ReadSAlItem(item, CWE_PurchasedItems[cweSaveFile.purchasedItemCount++]);
				}
			}

			if (d.HasMember("items")) {
				const auto& items = d["items"].GetArray();
				LoadAllItems(items);
			}

			fclose(fp);
		}
	}

	void SaveCWESave() {
		// FileWriteStream will flush it if it fills up, so we got nothing to worry about regarding size
		// 1024 is just an arbitrary number i chose
		static char writeBuffer[1024];

		char filename[MAX_PATH];
		sprintf(filename, "%s_%s", (const char*)0x136604C, "CWEV1.json");

		FILE* fp = fopen(filename, "wb");
		
		if (fp) {
			FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));
			PrettyWriter<FileWriteStream> writer(os);

			writer.StartObject();

			SaveMember<SAlItemCwe, 10>(writer, CWE_PurchasedItems, "PurchasedItems");

			writer.Key("daynight");
			writer.StartObject();
			{
				writer.Key("day");
				writer.Uint(AL_DayNightCycle_GetSaveCurrentDay());
				writer.Key("time");
				writer.Double(AL_DayNightCycle_GetSaveTime());
				writer.Key("currentPhase");
				writer.Uint(AL_DayNightCycle_GetSaveCurrentPhase());
				writer.Key("nextDayCloudy");
				writer.Bool(AL_DayNightCycle_GetSaveNextDayCloudy());
			}
			writer.EndObject();

			writer.Key("market_inventory");
			writer.StartObject();
			for (size_t i = 0; i < _countof(CategoryKeyValuePairs); ++i) {
				const auto& pair = CategoryKeyValuePairs[i];
				const auto& inv = cweSaveFile.marketInventory[pair.category];
				const auto& count = cweSaveFile.marketInventoryCount[pair.category];

				writer.Key(pair.key);
				writer.StartArray();
				for (size_t j = 0; j < count; ++j) {
					SaveSAlItem(writer, inv[j]);
				}
				writer.EndArray();
			}
			writer.EndObject();

			writer.Key("purchased_items");
			writer.StartArray();
			for (size_t i = 0; i < cweSaveFile.purchasedItemCount; ++i) {
				SaveSAlItem(writer, CWE_PurchasedItems[i]);
			}
			writer.EndArray();

			writer.Key("items");
			SaveAllItems(writer);

			writer.EndObject();

			fclose(fp);
		}
	}
}

SAlItemCwe* GetMarketInventory(int category)
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

// int __usercall@<eax>(char *path@<ecx>, void *buffer@<edx>, size_t size)
static const void *const ReadSaveFileThingPtr = (void*)0x426860;
static inline int ReadSaveFileThing(char *path, void *buffer, size_t _size)
{
	int result;
	__asm
	{
		push[_size]
		mov edx, [buffer]
		mov ecx, [path]
		call ReadSaveFileThingPtr
		add esp, 4
		mov result, eax
	}
	return result;
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

#ifdef CHAO_48
CHAO_SAVE_INFO* GetFreeCWESlot()
{
	CHAO_SAVE_INFO* dataPtr = 0;
	for (int i = 0; i < 24; i++)
	{
		if (cweSaveFile.chaoParam[i].param.type == 0)
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
	CHAO_SAVE_INFO* dataPtr = 0;
	for(int i = 0; i < 24; i++)
	{
		if (ChaoSlots[i].param.type == 0)
		{
			dataPtr = &ChaoSlots[i];
			break;
		}
	}
	dataPtr = GetFreeCWESlot();
	if (dataPtr != 0)
	{
		memset(dataPtr, 0, sizeof(CHAO_SAVE_INFO));
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
		if (cweSaveFile.chaoParam[i].param.type && cweSaveFile.chaoParam[i].param.place == a1)
			count++;
	}
	return count;
}

int GetFreeChaoCWE2()
{
	int count = 0;
	for (int i = 0; i < 24; i++)
	{
		if (cweSaveFile.chaoParam[i].param.type)
			count++;
	}
	return count;
}

int __cdecl sub_5319F0_(int a1)
{
	char* v1; // eax
	int v2; // edx
	int v3; // ecx

	v1 = &ChaoSlots[0].param.place;
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
DataArray(SAlItem, PurchasedInventory, 0x01DBEDA0, 6);
int sub_531A20()
{
	char* v0; // eax
	int v1; // edx
	int v2; // ecx
	int i; // eax
	
	v0 = (char*)&ChaoSlots[0].param.type;
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
		if (PurchasedInventory[i].mCategory == 1)
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
	AL_SetChaoOnTheGarden();
	if (AL_IsGarden())
	{
		for (int i = 0; i < 24; i++)
		{
			CHAO_SAVE_INFO* chao = &cweSaveFile.chaoParam[i];
			if (chao->param.place == AL_GetStageNumber()
				&& chao != *(CHAO_SAVE_INFO**)0x01A5CA5C)
				//&& chao != (ChaoData*)GBAManager_GetChaoDataPointer())
			{
				NJS_VECTOR* spawn = &ProbablyChaoSpawnPoints[16 * AL_GetStageNumber()
					+ (int)(njRandom() * 15.9f) - 16];

				if (chao->param.type)
				{
					if (chao->param.type == 1)
					{
						CreateChaoExtra(chao, 0, 0, spawn, NJM_DEG_ANG(njRandom() * 360.f));
					}
					else if (chao->param.ClassNum == -1)
					{
						task* task = CreateChaoExtra(
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
#endif

ITEM_SAVE_INFO* AL_GetSpecialItemSave() {
	for (int i = 0; i < _countof(cweSaveFile.specialItems); i++) {
		if (cweSaveFile.specialItems[i].kind == -1) {
			return &cweSaveFile.specialItems[i];
		}
	}

	return NULL;
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
		cweSaveFile.specialItems[i].kind = -1;
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