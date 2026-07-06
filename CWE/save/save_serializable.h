#pragma once
#include "document.h"
#include <array>
#include "filewritestream.h"
#include "prettywriter.h"
using namespace rapidjson;

void Parse(NJS_VECTOR& a, Value& d);
void Parse(SAlItemCwe& a, Value& d);
void Parse(ITEM_SAVE_INFO& a, Value& d);

void Write(PrettyWriter<FileWriteStream>& d, SAlItemCwe& a);

template <typename T, int max>
void LoadMember(Document& d, std::array<T, max>& result, const char* jsonName) {
	if (!d.HasMember(jsonName)) {
		___OutputDebugString("%s doesnt exist", jsonName);
		return;
	}

	auto& arr = d[jsonName];
	if (!arr.IsArray()) {
		___OutputDebugString("%s isnt an array", jsonName);
		return;
	}
	___OutputDebugString("%d", arr.Size());
	for (size_t i = 0; i < arr.Size(); i++) {
		if (i >= max) {
			___OutputDebugString("%s has more items than the specified max", jsonName);
			break;
		}
		
		Parse(result[i], arr[i]);
	}
}

template <typename T, int max>
void SaveMember(PrettyWriter<FileWriteStream>& d, std::array<T, max>& result, const char* jsonName) {
	d.Key(jsonName);
	d.StartArray();

	for (int i = 0; i < max; i++) {
		d.StartObject();
		Write(d,result[i]);
		d.EndObject();
	}
	d.EndArray();
}

