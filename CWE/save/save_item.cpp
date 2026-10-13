#include "stdafx.h"
#include <array>
#include "save_item.h"
#include <filewritestream.h>

std::array<AccessorySaveInfo, AccessoryItemMax> AccessoryItemList;
static size_t AccessoryItemCount = 0;

void SaveAllItems(rapidjson::PrettyWriter<rapidjson::FileWriteStream>& writer) {
	writer.StartArray();
	for (const auto& item : AccessoryItemList) {
		if (item.IndexID == -1) continue;

		writer.StartObject();
		item.Save(writer);
		writer.EndObject();
	}
	writer.EndArray();
}

static void LoadItem(const Sint8 category, const rapidjson::Value& value) {
	AccessorySaveInfo saveInfo;

	switch (category) {
	case ALW_CATEGORY_ACCESSORY:
		if (AccessoryItemCount >= AccessoryItemMax) return;

		AccessorySaveInfo::Load(saveInfo, value);
		AccessoryItemList[AccessoryItemCount++] = saveInfo;
	}
}

void ClearAllItemSave() {
	AccessoryItemCount = 0;
	for (auto& item : AccessoryItemList) {
		item.IndexID = -1;
		item.ID[0] = 0;
	}
}

void LoadAllItems(const rapidjson::GenericArray<false, rapidjson::Value>& array) {
	for (const auto& item : array) {
		const auto categoryStr = item["category"].GetString();
		int category = -1;

		for (size_t i = 0; i < _countof(CategoryStrings); ++i) {
			if (!strcmp(categoryStr, CategoryStrings[i])) {
				category = int(i);
				break;
			}
		}

		if (category == -1) continue;

		LoadItem(category, item);
	}
}

ItemSaveInfoBase* CWE_GetNewItemSaveInfo(const Sint8 category) {
	switch (category) {
		case ALW_CATEGORY_ACCESSORY:
			for (auto& item : AccessoryItemList) {
				if (item.IndexID == -1) {
					return &item;
				}
			}
			break;
	}

	return NULL;
}

// returns if it uses the new class-based save system or the old one
bool CWE_IsCustomItemSaveInfoCategory(const Sint8 category) {
	switch (category) {
		case ALW_CATEGORY_ACCESSORY:
			return true;
	}

	return false;
}