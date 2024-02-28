#include "stdafx.h"
#include "document.h"
#include "../al_world.h"
#include <array>
#include "../al_save.h"
#include "filewritestream.h"
#include "prettywriter.h"
using namespace rapidjson;

void Parse(NJS_VECTOR& a, Value& d) {
	if (!d.IsArray()) return;
	if (d.Size() != 3) return;

	a.x = d[0].GetFloat();
	a.y = d[1].GetFloat();
	a.z = d[2].GetFloat();
}

void Parse(SAlItem& a, Value& d) {
	if (!d.HasMember("mCategory") ||
		!d.HasMember("mType")) {
		PrintDebug("SAlItem doesnt have correct members");
		return;
	}
	a.mCategory = d["mCategory"].GetInt();
	a.mType = d["mType"].GetInt();
}

void Parse(ITEM_SAVE_INFO& a, Value& d) {
	//how do i do this without it being disgusting
	if (!d.HasMember("Age") || 
		!d.HasMember("Garden") || 
		!d.HasMember("Size") || 
		!d.HasMember("Type") || 
		!d.HasMember("position"))
		return;
	a.Age = d["Age"].GetInt();
	a.Garden = d["Garden"].GetInt();
	Parse(a.position, d["Position"]);
	a.Size = d["Size"].GetInt();
	a.Type = d["Type"].GetInt();
}

void Write(PrettyWriter<FileWriteStream>& d, SAlItem& a) {
	d.Key("mCategory");
	d.Int(a.mCategory);
	d.Key("mType");
	d.Int(a.mType);
}
