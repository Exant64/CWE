#include "stdafx.h"
#include "usercall.h"

void*(*syMalloc)(int size, const char* a2, int line) = GenerateUsercallWrapper<void*(*)(int size, const char* a2, int line)>(rEAX, 0x0077DF20, rEDX, rECX, rEAX);
void(*syFree)(void* p, const char* a2, int line) = GenerateUsercallWrapper<void(*)(void* p, const char* a2, int line)>(noret, 0x0077DF80, rEAX, rEDX, rECX);
