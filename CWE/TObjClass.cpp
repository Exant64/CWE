#include "stdafx.h"
#include "TObjClass.h"

void callTObjClass_MainSub(ObjectMaster* _object)
{
	TObjClass* c = (TObjClass*)_object->Data2.Undefined;
	c->MainSub();
}

void callTObjClass_DisplaySub(ObjectMaster* _object)
{
	TObjClass* c = (TObjClass*)_object->Data2.Undefined;
	c->DisplaySub();
}

TObjClass::TObjClass(int list, const char* name)
{
	_object = AllocateObjectMaster(callTObjClass_MainSub, list, name);
	_object->Data2.Undefined = this;
	_object->DisplaySub = callTObjClass_DisplaySub;
}

TObjClass::~TObjClass()
{
	DeleteObject_(_object);
}
