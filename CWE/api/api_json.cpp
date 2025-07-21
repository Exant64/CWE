#include "stdafx.h"
#include <ChaoMain.h>
#include <filesystem>
#include <filereadstream.h>
#include "api_accessory.h"
#include "api_util.h"
#include "ModelInfo.h"
#include <error/en.h>

#define MEMBER(member) "\"#member\" "

static bool GenericParseID(const rapidjson::Document& document, const APIErrorUtil& error, char id[20]) {
	if (!document.HasMember("id")) {
		error.print(MEMBER(id) "not found!");
	}
	else if (!document["id"].IsString()) {
		error.print(MEMBER(id) "isn't a string!");
	}
	else if (document["id"].GetStringLength() > 20) {
		error.print(MEMBER(id) "must consist of max. 20 characters!");
	}
	else {
		memcpy(id, document["id"].GetString(), 20);
		return true;
	}

	return false;
}

static NJS_OBJECT* GenericParseChunkObj(const rapidjson::Document& document, const APIErrorUtil& error, const char* pathToJson) {
	const std::filesystem::path jsonPath = pathToJson;

	if (!document.HasMember("model")) {
		error.print(MEMBER(model) "not found!");
		return NULL;
	}

	if (!document["model"].IsString()) {
		error.print(MEMBER(model) "isn't a string!");
		return NULL;
	}
	
	ModelInfo* pModel = new ModelInfo(jsonPath.parent_path() / document["model"].GetString());
	auto pObj = pModel->getmodel();

	if (pModel->getformat() != ModelFormat_Chunk) {
		delete pModel;
		error.print(MEMBER(model) "is not a chunk model!");
		return NULL;
	}

	if (!pObj) {
		delete pModel;
		error.print(MEMBER(model) "does not point to a valid model!");
	}
	return pObj;
}

static bool GenericMarketDataParse(const rapidjson::Document& document, const APIErrorUtil& error, const char*& pName, const char*& pDescription, BlackMarketItemAttributes& attrib) {
	if (!document.HasMember("market_data")) {
		error.print(MEMBER(market_data) "not found!");
		return false;
	}

	const auto& market_data = document["market_data"];

	if (!market_data.IsObject()) {
		error.print(MEMBER(market_data) "not an object!");
		return false;
	}

	if (!market_data.HasMember("price")) {
		error.print(MEMBER(price) "in" MEMBER(market_data) "not found!");
		return false;
	}
	else if (!market_data["price"].IsInt()) {
		error.print(MEMBER(price) "in " MEMBER(market_data) "is not an integer!");
		return false;
	}
	else {
		attrib.PurchasePrice = market_data["price"].GetInt();
	}

	if (!market_data.HasMember("sale")) {
		error.print(MEMBER(sale) "in" MEMBER(market_data) "not found!");
		return false;
	}
	else if (!market_data["sale"].IsInt()) {
		error.print(MEMBER(sale) "in " MEMBER(market_data) "is not an integer!");
		return false;
	}
	else {
		attrib.SalePrice = market_data["sale"].GetInt();
	}

	if (!market_data.HasMember("emblems")) {
		error.print(MEMBER(emblems) "in" MEMBER(market_data) "not found!");
		return false;
	}
	else if (!market_data["emblems"].IsInt()) {
		error.print(MEMBER(emblems) "in " MEMBER(market_data) "is not an integer!");
		return false;
	}
	else {
		attrib.RequiredEmblems = market_data["emblems"].GetInt();
	}

	if (!market_data.HasMember("name")) {
		error.print(MEMBER(name) "in" MEMBER(market_data) "not found!");
		return false;
	}
	else if (!market_data["name"].IsString()) {
		error.print(MEMBER(name) "in " MEMBER(market_data) "is not a string!");
		return false;
	}
	else {
		pName = market_data["name"].GetString();
	}

	if (!market_data.HasMember("description")) {
		error.print(MEMBER(description) "in" MEMBER(market_data) "not found!");
		return false;
	}
	else if (!market_data["description"].IsString()) {
		error.print(MEMBER(description) "in " MEMBER(market_data) "is not a string!");
		return false;
	}
	else {
		pDescription = market_data["description"].GetString();
	}

	// copy the strings if everything succeeded
	pName = _strdup(pName);
	pDescription = _strdup(pDescription);

	attrib.Name = -1;
	attrib.Descriptions = -1;

	return true;
}

static bool AccessoryParse(const char* path, const rapidjson::Document& document) {
	const static std::unordered_map<std::string, EAccessoryType> AccessorySlotMap = {
		{"head", EAccessoryType::Head},
		{"face", EAccessoryType::Face},
		{"generic1", EAccessoryType::Generic1},
		{"generic2", EAccessoryType::Generic2},
	};

	APIErrorUtil error("Error loading accessory %s: ", path);

	CWE_API_ACCESSORY_DATA accessoryData{};
	std::optional<CWE_API_ACCESSORY_BALD_DATA> optionBaldData;
	std::vector<CWE_API_ACCESSORY_COLOR_ENTRY> colorEntries;

	// "id"
	if (!GenericParseID(document, error, accessoryData.ID)) return false;

	if (!AccessoryCheckID(accessoryData.ID)) {
		error.print("ID is already used by another accessory!");
		return false;
	}

	// "texture"
	if (!document.HasMember("texture")) {
		error.print(MEMBER(texture) "not found!");
		return false;
	}
	else if (!document["texture"].IsString()) {
		error.print(MEMBER(texture) "isn't a string!");
		return false;
	}
	else {
		accessoryData.pTextureName = document["texture"].GetString();
		accessoryData.pTexlist = NULL;
	}

	// "hide_parts" (optional)
	if (document.HasMember("hide_parts")) {
		const auto& hide_parts = document["hide_parts"];
		if (!hide_parts.IsArray()) {
			error.print(MEMBER(hide_parts) "is not an array!");
			return false;
		}

		for (const auto& element : hide_parts.GetArray()) {
			if (!element.IsInt()) {
				error.print("One of " MEMBER(hide_parts) "elements' is not an integer!");
				return false;
			}

			const auto part = uint64_t(element.GetInt());
			if (part < 0 || part >= 40) {
				error.print("One of " MEMBER(hide_parts) "elements' is outside of the valid range ([0..39])!");
				return false;
			}

			accessoryData.HideNodes |= uint64_t(1) << part;
		}
	}

	// "disable_jiggle" (optional)
	if (document.HasMember("disable_jiggle")) {
		const auto& disable_jiggle = document["disable_jiggle"];
		if (!disable_jiggle.IsBool()) {
			error.print(MEMBER(disable_jiggle) "is not a boolean!");
			return false;
		}

		if (disable_jiggle.GetBool()) {
			accessoryData.Flags |= CWE_API_ACCESSORY_FLAGS_NO_JIGGLE;
		}
	}

	// "slot"
	if (!document.HasMember("slot")) {
		error.print(MEMBER(slot) "not found!");
		return false;
	}
	else if (!document["slot"].IsString()) {
		error.print(MEMBER(slot) "isn't a string!");
		return false;
	}
	else if (!AccessorySlotMap.contains(document["slot"].GetString())) {
		error.print(MEMBER(slot) "isn't a valid slot name!");
		return false;
	}
	else {
		accessoryData.SlotType = AccessorySlotMap.at(document["slot"].GetString());
	}

	if (document.HasMember("bald_dont_hide_parts")) {
		const auto& value = document["bald_dont_hide_parts"];
		if (!value.IsBool()) {
			error.print(MEMBER(bald_dont_hide_parts) "isn't a boolean!");
			return false;
		}

		if (value.GetBool()) {
			accessoryData.Flags |= CWE_API_ACCESSORY_FLAGS_BALD_KEEP_HEAD_PARTS;
		}
	}

	if (document.HasMember("bald_mode")) {
		const auto& bald_mode = document["bald_mode"];
		if (bald_mode.IsArray()) {
			const auto& array = bald_mode.GetArray();
			if (array.Size() != 3) {
				error.print(MEMBER(bald_mode) "array isn't a length of 3!");
				return false;
			}

			for (size_t i = 0; i < 3; ++i) {
				if (!array[i].IsBool()) {
					error.print(MEMBER(bald_mode) "array element isn't a boolean!");
					return false;
				}

				if (array[i].GetBool()) {
					switch (i) {
					case 0:
						accessoryData.Flags |= CWE_API_ACCESSORY_FLAGS_BALD_PRESET_X;
						break;
					case 1:
						accessoryData.Flags |= CWE_API_ACCESSORY_FLAGS_BALD_PRESET_Y;
						break;
					case 2:
						accessoryData.Flags |= CWE_API_ACCESSORY_FLAGS_BALD_PRESET_Z;
						break;
					}
				}
			}
		}
		else if (bald_mode.IsObject()) {
			const auto& baldModeData = bald_mode.GetObj();
			const auto parse_point3 = [&](const char* memberName, NJS_POINT3& point, const rapidjson::Value& value) {
				if (!value.IsArray()) {
					error.print("%s isn't an array!", memberName);
					return false;
				}

				const auto& array = value.GetArray();
				if (array.Size() != 3) {
					error.print("%s length isn't 3!", memberName);
					return false;
				}

				for (size_t i = 0; i < 3; ++i) {
					if (!array[i].IsFloat()) {
						error.print("%s members' aren't floats!");
						return false;
					}
				}

				point.x = array[0].GetFloat();
				point.y = array[1].GetFloat();
				point.z = array[2].GetFloat();

				return true;
			};

			CWE_API_ACCESSORY_BALD_DATA baldData;
			if (!parse_point3("center", baldData.Center, baldModeData["center"])) return false;
			if (!parse_point3("influence", baldData.Influence, baldModeData["influence"])) return false;

			if (!baldModeData.HasMember("clip_face")) {
				error.print(MEMBER(clip_face) "isn't there!");
				return false;
			}

			if (!baldModeData["clip_face"].IsBool()) {
				error.print(MEMBER(clip_face) "isn't a boolean!");
				return false;
			}

			baldData.ClipFace = baldModeData["clip_face"].GetBool();

			if (!baldModeData.HasMember("radius")) {
				error.print(MEMBER(radius) "isn't there!");
				return false;
			}

			if (!baldModeData["radius"].IsFloat()) {
				error.print(MEMBER(radius) "isn't a float!");
				return false;
			}

			baldData.Radius = baldModeData["radius"].GetFloat();

			optionBaldData = baldData;
		}
		else {
			error.print(MEMBER(bald_mode) "isn't an array or an object!");
			return false;
		}
	}

	if (document.HasMember("color_slots")) {
		const auto parseColor = [&](const auto& colorElement, NJS_ARGB& outColor) {
			if (colorElement.IsString()) {
				// #AABBCC formatting
				const auto& colorString = colorElement.GetString();
				constexpr size_t hexFormatStringLen = std::char_traits<char>::length("#AABBCC");

				if (strlen(colorString) != hexFormatStringLen) {
					error.print("color string isn't in a valid formatting! (expected #AABBCC formatting)");
					return false;
				}

				bool checkFormatting = true;
				checkFormatting &= colorString[0] == '#';

				//# 0 A 1 A 2 B 3 B 4 C 5 C 6
				for (size_t strIndex = 1; checkFormatting && strIndex <= 6; strIndex++) {
					bool validHex = (colorString[strIndex] >= '0' && colorString[strIndex] <= '9');
					validHex |= (colorString[strIndex] >= 'A' && colorString[strIndex] <= 'F');
					checkFormatting &= validHex;
				}

				if (!checkFormatting) {
					error.print("phase color string isn't in a valid formatting! (expected #AABBCC formatting)");
					return false;
				}

				int r, g, b;
				sscanf(colorString + 1, "%02x%02x%02x", &r, &g, &b);

				outColor.r = r / 255.f;
				outColor.g = g / 255.f;
				outColor.b = b / 255.f;
			}
			else {
				error.print("color isn't in a valid formatting!");
				return false;
			}

			return true;
		};

		const auto& color_slots = document["color_slots"];
		if (!color_slots.IsObject()) {
			error.print("color_slots is not an object!");
			return false;
		}

		if (!color_slots["colors"].IsArray()) {
			error.print("color_slots color is not an array!");
			return false;
		}

		const auto& color_slots_colors = color_slots["colors"].GetArray();
		if (color_slots_colors.Size() != 8) {
			error.print("color_slots colors length isn't 8!");
			return false;
		}

		for (size_t i = 0; i < 8; ++i) {
			NJS_ARGB color;
			NJS_COLOR finalColor;

			if (!parseColor(color_slots_colors[i], color)) {
				return false;
			}
			
			finalColor.argb.a = 255;
			finalColor.argb.r = Uint8(color.r * 255);
			finalColor.argb.g = Uint8(color.g * 255);
			finalColor.argb.b = Uint8(color.b * 255);

			accessoryData.DefaultColors[i] = finalColor.color;
		}

		if (!color_slots.HasMember("used")) {
			error.print("color_slots used is missing!");
			return false;
		}

		if (!color_slots["used"].IsArray()) {
			error.print("color_slots used is not an array!");
			return false;
		}

		const auto& used = color_slots["used"].GetArray();
		for (const auto& usedEntry : used) {
			if (!usedEntry.IsInt()) {
				error.print("color_slots used element isn't an integer!");
				return false;
			}

			accessoryData.UsedColorSlots |= (1 << usedEntry.GetInt());
		}

		if (!color_slots.HasMember("entries")) {
			error.print("color_slots entries is missing!");
			return false;
		}

		if (!color_slots["entries"].IsArray()) {
			error.print("color_slots entries is not an array!");
			return false;
		}

		const auto& entries = color_slots["entries"].GetArray();
		for (const auto& entry : entries) {
			if (!entry.IsObject()) {
				error.print("color_slots entries member isn't an object!");
				return false;
			}

			if (!entry.HasMember("material_index") || !entry.HasMember("node_index") || !entry.HasMember("slot_index")) {
				error.print("color_slots entries member missing crucial member!");
				return false;
			}

			const auto& material_index = entry["material_index"];
			const auto& node_index = entry["node_index"];
			const auto& slot_index = entry["slot_index"];

			if (!material_index.IsInt() || !node_index.IsInt() || !slot_index.IsInt()) {
				error.print("color_slots entries members aren't integer!");
				return false;
			}

			const CWE_API_ACCESSORY_COLOR_ENTRY colorEntry = {
				node_index.GetInt(),
				material_index.GetInt(),
				slot_index.GetInt()
			};

			colorEntries.push_back(colorEntry);
		}
	}
	
	BlackMarketItemAttributes attrib;
	if (!GenericMarketDataParse(document, error, accessoryData.pName, accessoryData.pDescription, attrib)) return false;

	// "model"
	accessoryData.pObject = GenericParseChunkObj(document, error, path);
	if (!accessoryData.pObject) {
		// let's try not to memleak it
		free((void*)accessoryData.pName);
		free((void*)accessoryData.pDescription);
		
		return false;
	}

	if (colorEntries.size()) {
		accessoryData.Flags |= CWE_API_ACCESSORY_FLAGS_FREE_COLORS;
		accessoryData.pColorEntries = (CWE_API_ACCESSORY_COLOR_ENTRY*)calloc(colorEntries.size(), sizeof(CWE_API_ACCESSORY_COLOR_ENTRY));
		memcpy(accessoryData.pColorEntries, colorEntries.data(), sizeof(CWE_API_ACCESSORY_COLOR_ENTRY)* colorEntries.size());
		accessoryData.ColorEntryCount = colorEntries.size();
	}

	if (optionBaldData) {
		accessoryData.pBaldData = (CWE_API_ACCESSORY_BALD_DATA*)malloc(sizeof(CWE_API_ACCESSORY_BALD_DATA));
		*accessoryData.pBaldData = *optionBaldData;
		accessoryData.Flags |= CWE_API_ACCESSORY_FLAGS_FREE_BALD;
	}

	accessoryData.pTextureName = _strdup(accessoryData.pTextureName);
	accessoryData.pMarketAttrib = &attrib;

	AddChaoAccessory(&accessoryData);

	return true;
}

struct JSONItemCollection {
	size_t ModIndex;
	std::vector<std::string> Paths;
};

struct JSONItemEntry {
	const char* const DirectoryToLookFor;
	std::vector<JSONItemCollection> ModEntries;
	bool (*Parse) (const char* path, const rapidjson::Document& document);
};

static JSONItemEntry ItemEntries[] = {
	{"Accessories", {}, AccessoryParse}
};

void LoadJSONData(size_t index) {
	const auto& entry = ItemEntries[index];
	for (const auto& modEntry : entry.ModEntries) {
		for (const auto& path : modEntry.Paths) {
			FILE* fp = fopen(path.c_str(), "rb");
			if (fp) {
				fseek(fp, 0L, SEEK_END);
				size_t sz = ftell(fp);

				// rapidjson expects a buffer with size of atleast 4
				sz = max(sz, 4);
				std::unique_ptr<char[]> readBuffer(new char[sz]);

				fseek(fp, 0, SEEK_SET);

				rapidjson::FileReadStream is(fp, readBuffer.get(), sz);

				rapidjson::Document d;
				d.ParseStream(is);

				APIErrorUtil error("Error reading API JSON %s: ", path.c_str());

				if (d.HasParseError()) {
					error.print("error parsing at %u: %s\n",
						(unsigned)d.GetErrorOffset(),
						GetParseError_En(d.GetParseError())
					);

					continue;
				}

				entry.Parse(path.c_str(), d);
			}
		}
	}
}

void ScanAllMods() {
	const auto mods = g_HelperFunctions->Mods;
	for (size_t i = 0; i < mods->size(); ++i) {
		const auto& mod = mods->data()[i];
		const auto &cweFolder = std::filesystem::path(mod.Folder) / "CWE";
		DWORD dwAttrib = GetFileAttributes(cweFolder.c_str());

		if (dwAttrib == INVALID_FILE_ATTRIBUTES || !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY)) {
			continue;
		}

		for (size_t j = 0; j < _countof(ItemEntries); ++j) {
			auto& item = ItemEntries[j];
			const auto& folder = cweFolder / item.DirectoryToLookFor;

			DWORD dwAttrib = GetFileAttributes(folder.c_str());

			if (dwAttrib == INVALID_FILE_ATTRIBUTES || !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY)) {
				continue;
			}

			JSONItemCollection collection = {
				.ModIndex = i
			};

			const auto findSearchPath = folder / "*.json";

			_WIN32_FIND_DATAA newdata;
			HANDLE newhfind = FindFirstFileA(findSearchPath.generic_string().c_str(), &newdata);
			if (newhfind != INVALID_HANDLE_VALUE)
			{
				do
				{
					collection.Paths.push_back(folder.generic_string() + "\\" + newdata.cFileName);
				} while (FindNextFileA(newhfind, &newdata) != 0);
				FindClose(newhfind);
			}

			item.ModEntries.push_back(collection);
		}
	}
}