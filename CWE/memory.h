#pragma once
extern void* (*syMalloc)(int size, const char* a2, int line);
extern void  (*syFree)(void* p, const char* a2, int line);

#define ALLOC(a) (a*)syMalloc(sizeof(a),__FILE__, __LINE__)
#define FREE(a)  syFree(a,(char*)__FILE__, __LINE__)