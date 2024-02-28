#pragma once
#include "../SA2ModLoader.h"
#include "../al_modelcontainer.h"

extern "C" {
	void OrthoInit(float orthoZoom = 100);
	void OrthoDraw(NJS_OBJECT* obj, DrawObjectFunc draw = njCnkDrawObject);
	void OrthoDrawBegin();
	void OrthoDrawEnd();
	void OrthoTranslate(float x, float y, float mulScale = 1);
	void OrthoScreenTranslate(float x, float y, float mulScale = 1);
}