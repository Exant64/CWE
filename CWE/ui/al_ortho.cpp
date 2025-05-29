#include "stdafx.h"
#include "../SA2ModLoader.h"
#include "d3dx9.h"
#include "../ninja_functions.h"
#include "../al_modelcontainer.h"

const int GXSetProjectionPtr = 0x0041FE70;
void GXSetProjection(int a1)
{
	__asm {
		mov eax, a1
		call GXSetProjectionPtr
	}
}

D3DXMATRIX ortho;
ThiscallFunctionPointer(void, Renderer_SetMatrix, (int th, int a1, int a2, int a3), 0x00866DE0);

float gOrthoZoom;
extern "C" {
	__declspec(dllexport) void OrthoInit(float orthoZoom = 100) {
		gOrthoZoom = orthoZoom;
		float aspect = HorizontalResolution / VerticalResolution;
		D3DXMatrixOrthoRH(&ortho,
			aspect * gOrthoZoom,
			gOrthoZoom,
			0.01f,
			100);
	}

	__declspec(dllexport) void OrthoDrawBegin() {
		Renderer_SetMatrix(*(int*)0x1A557C0, 0, 8, (int)&ortho);
		njControl3DBackup = njControl3D;
		njControl3D &= ~0x100;
	}

	__declspec(dllexport) void OrthoDrawEnd() {
		njControl3D = njControl3DBackup;
		GXSetProjection(0x025EFF00);
	}

	__declspec(dllexport) void OrthoDraw(NJS_OBJECT* obj, DrawObjectFunc draw = njCnkDrawObject) {
		OrthoDrawBegin();
		draw(obj);
		OrthoDrawEnd();
	}

	__declspec(dllexport) void njOrthoCnkDrawObject(NJS_OBJECT* obj) {
		OrthoDraw(obj, njCnkDrawObject);
	}

	__declspec(dllexport) void chOrthoCnkDrawObject(NJS_OBJECT* obj) {
		OrthoDraw(obj, chCnkDrawObject);
	}

	__declspec(dllexport) void OrthoTranslate(float x, float y, float mulScale = 1) {


		njUnitMatrix(0);
		njTranslate(NULL, x * gOrthoZoom, y * gOrthoZoom, 2);
		njScale(NULL, 0.025f * gOrthoZoom * mulScale, 0.025f * gOrthoZoom * mulScale, -1);
	}

	__declspec(dllexport) void OrthoScreenTranslate(float x, float y, float mulScale = 1) {

		float m_offX = ((x - 320.0f) / 640.0f) * (4.0f / 3.0f);
		float m_offY = -((y - 240) / 480.0f);
		njUnitMatrix(0);
		njTranslate(NULL, m_offX * gOrthoZoom, m_offY * gOrthoZoom, -40);
		njScale(NULL, 0.025f * gOrthoZoom * mulScale, 0.025f * gOrthoZoom * mulScale, 0.025f * gOrthoZoom * mulScale);
	}

}