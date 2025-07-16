#include "stdafx.h"
#include <ChaoMain.h>
#include <filesystem>
#include <filereadstream.h>
#include "api_accessory.h"
#include "api_util.h"
#include "ModelInfo.h"
#include <error/en.h>

#define MEMBER(asd) "\"#asd\" "

static bool GenericParseID(const rapidjson::Document& document, const APIErrorUtil& error, char id[20]) {
	if (!document.HasMember("id")) {
		error.print("\"id\" not found!");
	}
	else if (!document["id"].IsString()) {
		error.print("\"id\" isn't a string!");
	}
	else if (document["id"].GetStringLength() > 20) {
		error.print("\"id\" must consist of max. 20 characters!");
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
		error.print("\"model\" not found!");
		return NULL;
	}

	if (!document["model"].IsString()) {
		error.print("\"model\" isn't a string!");
		return NULL;
	}
	
	ModelInfo* pModel = new ModelInfo(jsonPath.parent_path() / document["model"].GetString());
	auto pObj = pModel->getmodel();

	if (pModel->getformat() != ModelFormat_Chunk) {
		delete pModel;
		error.print("\"model\" is not a chunk model!");
		return NULL;
	}

	if (!pObj) {
		delete pModel;
		error.print("\"model\" does not point to a valid model!");
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

	// "id"
	if (!GenericParseID(document, error, accessoryData.ID)) return false;

	if (!AccessoryCheckID(accessoryData.ID)) {
		error.print("ID is already used by another accessory!");
		return false;
	}

	// "texture"
	if (!document.HasMember("texture")) {
		error.print("\"texture\" not found!");
		return false;
	}
	else if (!document["texture"].IsString()) {
		error.print("\"texture\" isn't a string!");
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
				error.print("One of " MEMBER(hide_parts) " elements' is not an integer!");
				return false;
			}

			const auto part = uint64_t(element.GetInt());
			if (part < 0 || part >= 40) {
				error.print("One of " MEMBER(hide_parts) " elements' is outside of the valid range ([0..39])!");
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
		error.print("\"slot\" not found!");
		return false;
	}
	else if (!document["slot"].IsString()) {
		error.print("\"slot\" isn't a string!");
		return false;
	}
	else if (!AccessorySlotMap.contains(document["slot"].GetString())) {
		error.print("\"slot\" isn't a valid slot name!");
		return false;
	}
	else {
		accessoryData.SlotType = AccessorySlotMap.at(document["slot"].GetString());
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