#pragma once
#include "SA2ModLoader.h"
class TObjClass
{
public:
	TObjClass(int list, const char* name);
	~TObjClass();
	virtual void MainSub() = 0;
	virtual void DisplaySub() = 0;
	virtual void DeleteSub() = 0;
private:
	ObjectMaster* _object;

};

