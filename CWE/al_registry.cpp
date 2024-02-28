#include "stdafx.h"
#include "al_registry.h"

int ItemRegistry::GetID(const char* str)
{
	return map[str];
}

void ItemRegistry::Register(const char* str, int ID)
{
	map.emplace(str, ID);
}

ItemRegistry::ItemRegistry()
{
}
