#pragma once
#include <rapidjson.h>
#include <prettywriter.h>
#include <document.h>
#include <filewritestream.h>
#include <chao.h>
#include <api/api_metadata.h>
#include <unordered_map>

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

static const std::unordered_map<int, std::string> IndexToIDGardenMap = {
	{1, "neut"},
	{2, "hero"},
	{3, "dark"}
};

static const std::unordered_map<std::string, int> IDToIndexGardenMap = {
	{"neut", 1},
	{"hero", 2},
	{"dark", 3}
};

struct ItemSaveInfoBase {
	// we store here both index and string ID for efficiency
	// index gets resolved on load, ID field is used for quick saving back and for storing invalid accessory
	int IndexID;
	char ID[METADATA_ID_SIZE];
	int Garden;
	NJS_POINT3 Position;
	Angle Angle;

	virtual ChaoItemCategory GetCategory() const = NULL;

	virtual void Save(rapidjson::PrettyWriter<rapidjson::FileWriteStream>& writer) const {
		if (IndexID == -1) return;

		writer.Key("category");
		writer.String(CategoryStrings[GetCategory()]);

		writer.Key("id");
		writer.String(ID);

		writer.Key("garden");
		writer.String(IndexToIDGardenMap.at(Garden).c_str());

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

		item.Garden = IDToIndexGardenMap.at(value["garden"].GetString());
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
	Uint32 Colors[8];
	Uint32 UsedColors;

	ChaoItemCategory GetCategory() const {
		return ChaoItemCategory_Accessory;
	}

	void Clear() override {
		ItemSaveInfoBase::Clear();
		UsedColors = 0;
	}

	void Save(rapidjson::PrettyWriter<rapidjson::FileWriteStream>& writer) const {
		ItemSaveInfoBase::Save(writer);

		writer.Key("colors");
		writer.StartArray();

		for (size_t i = 0; i < 8; ++i) {
			const NJS_COLOR* pColor = (NJS_COLOR*)&Colors[i];

			char color[32];
			sprintf_s(color, "#%02x%02x%02x", pColor->argb.r, pColor->argb.g, pColor->argb.b);
			writer.String(color);
		}

		writer.EndArray();

		writer.Key("used_colors");
		writer.Uint(UsedColors);
	}

	static void Load(AccessorySaveInfo& item, const rapidjson::Value& value) {
		ItemSaveInfoBase::Load(item, value);

		if (value.HasMember("colors")) {
			const auto& colorsArray = value["colors"].GetArray();
			for (size_t i = 0; i < 8; ++i) {
				if (colorsArray[i].IsString()) {
					NJS_COLOR* pColor = (NJS_COLOR*)&item.Colors[i];
					const auto& colorString = colorsArray[i].GetString();

					int r, g, b;
					sscanf(colorString + 1, "%02x%02x%02x", &r, &g, &b);

					pColor->argb.a = 255;
					pColor->argb.r = r;
					pColor->argb.g = g;
					pColor->argb.b = b;
				}
			}
		}

		if (value.HasMember("used_colors")) {
			item.UsedColors = value["used_colors"].GetUint();
		}
		else {
			item.UsedColors = 0;
		}
	}
};

static const size_t AccessoryItemMax = 128;
extern std::array<AccessorySaveInfo, AccessoryItemMax> AccessoryItemList;

void ClearAllItems();
void SaveAllItems(rapidjson::PrettyWriter<rapidjson::FileWriteStream>& writer);
void LoadAllItems(const rapidjson::GenericArray<false, rapidjson::Value>& array);

bool CWE_IsCustomItemSaveInfoCategory(const ChaoItemCategory category);
ItemSaveInfoBase* CWE_GetNewItemSaveInfo(const ChaoItemCategory category);