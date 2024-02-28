#pragma once


#pragma pack(push, 8)
struct __declspec(align(4)) ChaoHudThingB
{
	int adjust;
	float wd;
	float ht;
	float s0;
	float t0;
	float s1;
	float t1;
	NJS_TEXLIST* pTexlist;
	int TexNum;
};
#pragma pack(pop)
FunctionPointer(void, SetChaoHUDThingBColor, (float a1, float a2, float a3, float a4), 0x00559FA0);
void DrawChaoHudThingB(ChaoHudThingB* a1, float a2, float a3, float a4, float a5, float a6, int a7, int a8);

void WcConvFromCStr(int result, int a2, signed int a3);

struct ChaoHudThingOld
{
	NJS_POINT2I position;
	NJS_POINT2I scale;
	NJS_POINT2I c;
	NJS_POINT2I d;
};

struct ChaoHudThing
{
	__int16 x0;
	__int16 x1;
	__int16 y0;
	__int16 y1;
	__int16 u0;
	__int16 u1;
	__int16 v0;
	__int16 v1;
};

void DrawChaoHudThing(ChaoHudThing* a1, int a2);
void __cdecl AlgKinderOrthoQuadDrawArray(ChaoHudThing* q, int len, Uint32 color);
void __cdecl AlgKinderOrthoQuadDrawArrayTile(ChaoHudThing* a3, int count, Uint32 color, int nx, int ny);
void __cdecl AlgKinderOrthoQuadDrawArrayOffset(ChaoHudThing const* a3, int count, unsigned int color, __int16 oX, __int16 oY);
void __cdecl AlgKinderOrthoQuadDrawArrayTileOffset(ChaoHudThing const* a4, int a5, unsigned int a6, int a7, int a8, int a9, __int16 a10);