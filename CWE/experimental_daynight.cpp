#include "stdafx.h"
#include <util.h>

static NJS_ARGB VertColor = { 1,1,1,1 };
static int Phase = 0;
static int Timer = 0;

const static NJS_ARGB ColorLerp[] = {
	{1,1,1,1},
	//{1,223/255.0f, 212/255.0f, 177/255.0f},
	{1,1,200 / 255.0f, 152 / 255.0f},
	{1,0.566252f,0.702886f,0.84448f}
};

static void LerpColor(NJS_ARGB& out, NJS_ARGB& a, NJS_ARGB& b, float t) {
	out.r = lerp(a.r, b.r, t);
	out.g = lerp(a.g, b.g, t);
	out.b = lerp(a.b, b.b, t);
}

static void DrawLandtableHook() {
	VoidFunc(DrawLandtable, 0x0047C2E0);
	*(char*)0x93BEB3 = 0;
	DrawLandtable();
	*(char*)0x93BEB3 = 1;
}
VoidFunc(GXEnd, 0x0041C070);
static void GXEndHook() {


	DataPointer(int*, VertDeclInfo, 0x0174F7E8);
	DataPointer(int*, PosUVColor, 0x0174F7FC);

	if (VertDeclInfo == PosUVColor) {

		int strideSize = VertDeclInfo[9] / 4;

		int count = *(int*)0x01933F04;
		int* pVertex = (int*)(*(int*)0x01933EF8 + *(int*)0x01933F08);
		for (int i = 0; i < count; i++) {
			NJS_COLOR color = { 0 };
			color.color = pVertex[i * strideSize + 3];
#if 0
			color.argb.r *= 0.366252f;
			color.argb.g *= 0.502886;
			color.argb.b *= 0.64448;
#else
			color.argb.r = (Uint8)(color.argb.r * VertColor.r);
			color.argb.g = (Uint8)(color.argb.g * VertColor.g);
			color.argb.b = (Uint8)(color.argb.b * VertColor.b);
#endif
			pVertex[i * strideSize + 3] = color.color;
		}
	}
	GXEnd();
}
