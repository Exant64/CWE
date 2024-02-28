#pragma once
#include "stdafx.h"


struct Light
{
	NJS_VECTOR direction;
	float intensity;
	float ambient;
	NJS_VECTOR color;
};

struct LightGC
{
	NJS_VECTOR direction;
	float floatC;
	float float10;
	NJS_ARGB float14;
	char SomeFlag;
	int field_25;
	int field_29;
	char field_2B;
	char field_2C;
	char field_2D;
	char field_2E;
	char field_2F;
};
void sub_426CC0(NJS_MATRIX_PTR result, NJS_VECTOR* a2, NJS_VECTOR* a3, char a4);
VoidFunc(sub_42D340, 0x42D340);
void njColorBlendingMode(int a1, int a2);
FastcallFunctionPointer(float,  njCos, (signed int a1), 0x042AC30);
FastcallFunctionPointer(signed int,  njPushMatrix, (NJS_MATRIX_PTR a1), 0x0077FE10);
FastcallFunctionPointer(float,  njSin, (signed int a1), 0x0042AAB0);
FunctionPointer(int, chCnkDrawModel, (NJS_CNK_MODEL *a1), 0x0056E1E0);
static void chCnkDrawModelWrapper(NJS_CNK_MODEL* a1) {
	chCnkDrawModel(a1);
}
FunctionPointer(signed int, njPushMatrixEx, (), 0x00429710);
FunctionPointer(signed int, njPopMatrixEx, (), 0x00429000);
//FunctionPointer(void, njScale, (float a1, float a2, float a3), 0x00427750);
FunctionPointer(void, njCnkDrawObjectOrig, (NJS_OBJECT *a1), 0x42E730);
static void njCnkDrawObject(NJS_OBJECT* a1) {
	njCnkDrawObjectOrig(a1);
}
DataPointer(NJS_MATRIX, flt_25F02A0, 0x25F02A0);
FunctionPointer(int, njPushUnitMatrix, (), 0x44B210);

DataPointer(int, njControl3D, 0x025F02D8);
DataPointer(int, njControl3DBackup, 0x01DEB6A4);
DataPointer(float*, _nj_current_matrix_ptr_, 0x01A557FC);
void  njCalcVector(NJS_VECTOR *a1, NJS_VECTOR *a2, NJS_MATRIX_PTR a3);
void  njSetTexture(NJS_TEXLIST* texlist);
float njUnitVector(NJS_VECTOR *a1);

void njCalcPoint(NJS_MATRIX_PTR result, NJS_VECTOR* a2, NJS_VECTOR* a3, char a4);

DataPointer(NJS_ARGB, _nj_constant_material_, 0x025EFFD0);
FunctionPointer(void, SetMaterial, (float a, float r, float g, float b), 0x44B2E0);

void  njTranslate(float *a1, float a2, float a3, float a4);

void  njTranslateEx(NJS_VECTOR* a1);

const int njCnkDrawObjectPtr = 0x0056E210;
void chCnkDrawObject(NJS_OBJECT* a1);

void njCnkDrawModel(NJS_CNK_MODEL* a1);

void njRotateX(NJS_MATRIX_PTR m, Angle x);
void njRotateY(NJS_MATRIX_PTR m, Angle y);
void njRotateZ(NJS_MATRIX_PTR m, Angle z);

void njSetTextureNum(int a1, int a2, int a3, int a4);

void njScale(NJS_MATRIX_PTR a1, float a2, float a3, float a4);

void njUnitMatrix(NJS_MATRIX_PTR matrix);
void DrawQuadTexture(int a1, float a2);
FunctionPointer(void, sub_781CB0, (int a1), 0x781CB0);
DataArray(Light, Lights, 0x01DE4280, 12);

#define RotateX(a) if(a) njRotateX(NULL, a)
#define RotateY(a) if(a) njRotateY(NULL, a)
#define RotateZ(a) if(a) njRotateZ(NULL, a)