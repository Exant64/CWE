#include "stdafx.h"
#include <rapidjson.h>
#include <memory>
#include <document.h>
#include <filereadstream.h>
#include <optional>
#include <chrono>
#include <filesystem>
#include <filewritestream.h>
#include <prettywriter.h>

// promise ill clean this up, was in a hurry

using namespace rapidjson;

static const char* GlobalSavePath = "./resource/gd_PC/SAVEDATA/CWE_GLOBAL.json";
// automated system todo
static const char* CWE_Version = "9.6";

// stolen from modloader's CrashDump.cpp

static std::wstring GetCurrentDateString()
{
	auto now = std::chrono::system_clock::now();
	std::time_t now_c = std::chrono::system_clock::to_time_t(now);
	std::tm* ptm = std::localtime(&now_c);

	// Format date with '_' separators
	std::wostringstream ss;
	ss << std::setfill(L'0') << std::setw(2) << ptm->tm_mday << L"_"
		<< std::setfill(L'0') << std::setw(2) << (ptm->tm_mon + 1) << L"_"
		<< (ptm->tm_year + 1900);

	return ss.str();
}

static std::wstring GetCurrentTimeString()
{
	auto now = std::chrono::system_clock::now();
	std::time_t now_c = std::chrono::system_clock::to_time_t(now);
	std::tm* ptm = std::localtime(&now_c);

	// Format time with '_' separators
	std::wostringstream ss;
	ss << std::setfill(L'0') << std::setw(2) << ptm->tm_hour << L"_"
		<< std::setfill(L'0') << std::setw(2) << ptm->tm_min << L"_"
		<< std::setfill(L'0') << std::setw(2) << ptm->tm_sec;

	return ss.str();
}

static std::wstring GenerateBackupDirectoryName() {
	const auto dateString = GetCurrentDateString();
	const auto timeString = GetCurrentTimeString();

	return L"Backup " + timeString + L" " + dateString;
}

static void BackupSaves() {
	const auto backupName = GenerateBackupDirectoryName();

	wchar_t message[MAX_PATH + 200];
	swprintf_s(message, L"A new CWE version has been installed, for safety reasons a backup of your save files will be created in the folder \"resource/gd_PC/SAVEDATA/%s\".", backupName.c_str());

	MessageBoxW(0, message, L"CWE", MB_OK);

	const std::filesystem::path saveDirectory = "./resource/gd_PC/SAVEDATA/";
	const std::filesystem::path backupDirectory = saveDirectory / backupName;

	std::filesystem::create_directory(backupDirectory);

	for (const auto& entry : std::filesystem::directory_iterator(saveDirectory)) {
		if (entry.is_directory()) {
			continue;
		}

		std::filesystem::copy_file(entry, backupDirectory / entry.path().filename());
	}
}

static std::optional<Document> GlobalSave_Load() {
	FILE* fp = fopen(GlobalSavePath, "rb");
	if (!fp) {
		return std::nullopt;
	}

	fseek(fp, 0L, SEEK_END);
	size_t sz = ftell(fp);

	// rapidjson expects a buffer with size of atleast 4
	sz = max(sz, 4);
	std::unique_ptr<char[]> readBuffer(new char[sz]);

	fseek(fp, 0, SEEK_SET);

	FileReadStream is(fp, readBuffer.get(), sz);

	Document d;
	d.ParseStream(is);

	fclose(fp);

	return d;
}

void GlobalSave_Init() {
	const auto loadResult = GlobalSave_Load();
	
	if (loadResult) {
		if (!strcmp((*loadResult)["version"].GetString(), CWE_Version)) {
			return;
		}
	}
	
	BackupSaves();	

	static char writeBuffer[1024];
	FILE* fp = fopen(GlobalSavePath, "wb");

	if (fp) {
		FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));
		PrettyWriter<FileWriteStream> writer(os);

		writer.StartObject();
		writer.Key("version");
		writer.String(CWE_Version);

		writer.EndObject();

		fclose(fp);
	}
}