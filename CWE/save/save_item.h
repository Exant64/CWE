#pragma once
#include <rapidjson.h>
#include <prettywriter.h>
#include <document.h>
#include <filewritestream.h>
#include <chao.h>
#include <api/api_metadata.h>

static constexpr const char* CategoryStrings[] = {
	"chao", // not gonna be saving that in json anytime soon lol
	"egg",
	"minimal",
	"fruit",
	"tree",
	"growtree",
	"toy",
	"seed",
	"accessory",//"sound",
	"hat",
	"special"
};

struct ItemSaveInfoBase {
	// we store here both index and string ID for efficiency
	// index gets resolved on load, ID field is used for quick saving back and for storing invalid accessory
	int IndexID;
	char ID[20];
	int Garden;
	NJS_POINT3 Position;
	Angle Angle;

	virtual ChaoItemCategory GetCategory() const = NULL;

	virtual void Save(rapidjson::PrettyWriter<rapidjson::FileWriteStream>& writer) const {
		writer.Key("category");
		writer.String(CategoryStrings[GetCategory()]);

		writer.Key("id");
		writer.String(ID);

		writer.Key("garden");
		writer.Int(Garden);

		writer.Key("angle");
		writer.Int(Angle);

		writer.Key("position");
		writer.StartArray();
		writer.Double(Position.x);
		writer.Double(Position.y);
		writer.Double(Position.z);
		writer.EndArray();
	}

	static void Load(ItemSaveInfoBase& item, const rapidjson::Value& value) {
		strcpy_s(item.ID, value["id"].GetString());

		item.IndexID = int(ItemMetadata::Get()->GetIndex(ChaoItemCategory_Accessory, item.ID));

		item.Garden = value["garden"].GetInt();
		item.Angle = value["angle"].GetInt();

		const auto& pos = value["position"].GetArray();
		item.Position.x = pos[0].GetFloat();
		item.Position.y = pos[1].GetFloat();
		item.Position.z = pos[2].GetFloat();
	}

	virtual void Clear() {
		IndexID = -1;
		ID[0] = 0;
	}
};

struct AccessorySaveInfo : ItemSaveInfoBase {
	ChaoItemCategory GetCategory() const {
		return ChaoItemCategory_Accessory;
	}

	void Save(rapidjson::PrettyWriter<rapidjson::FileWriteStream>& writer) const {
		ItemSaveInfoBase::Save(writer);
	}

	static void Load(AccessorySaveInfo& item, const rapidjson::Value& value) {
		ItemSaveInfoBase::Load(item, value);
	}
};

static const size_t AccessoryItemMax = 128;
extern std::array<AccessorySaveInfo, AccessoryItemMax> AccessoryItemList;

void ClearAllItems();
void SaveAllItems(rapidjson::PrettyWriter<rapidjson::FileWriteStream>& writer);
void LoadAllItems(const rapidjson::GenericArray<false, rapidjson::Value>& array);

ItemSaveInfoBase* CWE_GetNewItemSaveInfo(const ChaoItemCategory category);