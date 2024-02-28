#pragma once
#include <map>
//generic registry minecraft-style thing
class ItemRegistry
{
private:
	std::map<const char*, int> map;
public:
	int GetID(const char* str);
	void Register(const char* str, int ID);
	ItemRegistry();
};