#include "stdafx.h"
#include "al_save.h"
#include <cstdio>
#include "Chao.h"
#include <random>
#include "ChaoMain.h"
#include <vector>
#include "asmutil.h"
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

			outItem.mId = ItemMetadata::Get()->GetIndex(outItem.mCategory, id);
			if (outItem.mId == Uint16(-1)) {
				return false;
			}
		}
		else {
			outItem.mId = Uint16(value["type"].GetInt());
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
		bool foundID = ItemMetadata::Get()->GetID(item.mCategory, item.mId, id);

		if (foundID) {
			writer.Key("id");
			writer.String(id);
		}
		else {
			writer.Key("type");
			writer.Int(item.mId);
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
			sz = NJM_MAX(sz, size_t(4));
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
			fread(ModAPI_SaveAPI[i].pointer, ModAPI_SaveAPI[i].fileSize, 1, f);
			fclose(f);
		}
		else
			memset(ModAPI_SaveAPI[i].pointer, 0, ModAPI_SaveAPI[i].fileSize);
	}
}

static ASM_FUNC int ReadSaveFileThing(char *path, void *buffer, size_t _size) {
    // arguments
    ASM_PUSH(      ASM_ESP(3+0+0) ); // size
    ASM_MOVE( edx, ASM_ESP(2+1+0) ); // buffer
    ASM_MOVE( ecx, ASM_ESP(1+1+0) ); // path

    // call
    ASM_CALL_R( eax, 0x426860 );

    // end arguments
    ASM_ESP_ADD( 1 );

    // return
    ASM_RET( 0 );
}

int __cdecl ReadCWESaveFile(char* path, void* buffer, size_t size) {
	int ret = ReadSaveFileThing(path, buffer, size);
	ReadCWESaveFiles();
	return ret;
}

static void ASM_FUNC ReadCWESaveFileHook() {
	ASM_PUSH(ASM_ESP(1)); // size
	ASM_PUSH(edx); // buffer
	ASM_PUSH(ecx); // path

	// Call your __cdecl function here:
	ASM_CALL (ReadCWESaveFile);

	ASM_POP(ecx); // path
	ASM_POP(edx); // buffer
	ASM_ESP_ADD( 1 ); // size
	ASM_RET(0);
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

static void ASM_FUNC SaveCWESaveFileHook() {
	ASM_PUSH(ASM_ESP(1)); // size
	ASM_PUSH(edx); // a2
	ASM_PUSH(ecx); // path

	// Call your __cdecl function here:
	ASM_CALL (SaveCWESaveFile);

	ASM_POP(ecx); // path
	ASM_POP(edx); // a2
	ASM_ESP_ADD( 1 ); // count
	ASM_RET(0);
}

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
	// no mem leftover (just to be safe)
	memset(&cweSaveFile, 0, sizeof(CWESaveFile));
	for (int i = 0; i < 30; i++)
		cweSaveFile.specialItems[i].kind = -1;
	WriteCall((void*)0x0052DF4C, (void*)ReadCWESaveFileHook);

	//i hook every single save file load, which also means the second memory card load for karate/transporter which fails on PC
	//however it causes the cwe savefile to reload which can delete items obtained before saving
	//hence this being commented out
	//lot of explanation for something commented out
	//WriteCall((void*)0x00532653, (void*)ReadCWESaveFileHook);

	WriteCall((void*)0x52E2AC, (void*)SaveCWESaveFileHook);
	WriteCall((void*)0x52FECB, (void*)SaveCWESaveFileHook);
	WriteCall((void*)0x532483, (void*)SaveCWESaveFileHook);
}