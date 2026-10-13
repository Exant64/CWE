#pragma once

enum {
	BBI_CMN_0 = 0x0,
	BBI_CMN_1 = 0x1,
	BBI_CMN_2 = 0x2,
	BBI_CMN_3 = 0x3,
	BBI_CMN_4 = 0x4,
	BBI_CMN_5 = 0x5,
	BBI_CMN_6 = 0x6,
	BBI_CMN_7 = 0x7,
	BBI_CMN_8 = 0x8,
	BBI_CMN_9 = 0x9,
	BBI_CMN_PLUS = 0xA,
	BBI_CMN_LV = 0xB,
	BBI_CMN_LEVELUP = 0xC,
	BBI_CMN_SWIM = 0xD,
	BBI_CMN_FLY = 0xE,
	BBI_CMN_RUN = 0xF,
	BBI_CMN_POWER = 0x10,
	BBI_CMN_STAMINA = 0x11,
	BBI_CMN_TOP = 0x12,
	BBI_CMN_TOP2 = 0x13,
	BBI_CMN_MIDDLE = 0x14,
	BBI_CMN_BOTTOM = 0x15,
	BBI_CMN_MARU_L = 0x16,
	BBI_CMN_MARU_M = 0x17,
	BBI_CMN_MARU_S = 0x18,
};

#pragma pack(push, 8)
struct ALIGN(4) CHS_BILL_INFO
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

DataArray(CHS_BILL_INFO, ChaoTexCommonData, 0x13128B0, 25);

FunctionPointer(void, chSetBillboardColor, (float a1, float a2, float a3, float a4), 0x00559FA0);
void chDrawBillboardSR(CHS_BILL_INFO* a1, float a2, float a3, float a4, float a5, float a6, int a7, int a8);

void WcConvFromCStr(int result, int a2, signed int a3);

struct SAlgKinderOrthoQuad {
	__int16 x0;
	__int16 x1;
	__int16 y0;
	__int16 y1;
	__int16 u0;
	__int16 u1;
	__int16 v0;
	__int16 v1;
};

void AlgKinderOrthoQuadDraw(const SAlgKinderOrthoQuad* a1, int a2);
void __cdecl AlgKinderOrthoQuadDrawArray(const SAlgKinderOrthoQuad* q, int len, Uint32 color);
void __cdecl AlgKinderOrthoQuadDrawArrayTile(SAlgKinderOrthoQuad* a3, int count, Uint32 color, int nx, int ny);
void __cdecl AlgKinderOrthoQuadDrawArrayOffset(SAlgKinderOrthoQuad const* a3, int count, unsigned int color, __int16 oX, __int16 oY);
void __cdecl AlgKinderOrthoQuadDrawArrayTileOffset(SAlgKinderOrthoQuad const* a4, int a5, unsigned int a6, int a7, int a8, int a9, __int16 a10);