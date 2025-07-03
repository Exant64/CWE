#pragma once
#include "stdafx.h"

/****** Texture Surface *************************************************************/
typedef struct
{
	Uint32      Type;            /* surface type                                     */
	Uint32      BitDepth;        /* bit depth                                        */
	Uint32      PixelFormat;     /* pixel format                                     */
	Uint32      nWidth;          /* width                                (in pixels) */
	Uint32      nHeight;         /* height                               (in pixels) */
	Uint32      TextureSize;     /* total size                            (in bytes) */
	Uint32      fSurfaceFlags;   /* surface flags                                    */
	Uint32*		pSurface;        /* pointer to surface data                          */
}
NJS2_TEXSURFACE;

/****** Texture System **************************************************************/
typedef struct
{
	Uint32          globalIndex; /* global index value                              */
	NJS2_TEXSURFACE  texsurface;  /* texture surface data                            */
	Int             count;       /* reference count                                 */
}
NJS_TEXSYSTEM;

/****** Texture Manage **************************************************************/
typedef struct
{
	Uint32          tspparam;   /* internal tsp param                               */
	Uint32          texparam;   /* internal tex param                               */
	Uint32          bank;       /* palette texture bank number         (-1 == none) */
	NJS_TEXSYSTEM* texsys;     /* texture system struct pointer                    */
	Int             count;      /* reference count                                  */
	Uint32          texflag;    /* texture flags                           (unused) */
}
NJS_TEXMANAGE;


typedef struct
{
	Uint32          drawmode;   /* drawing flags                                    */
	Uint32          cullmode;   /* culling flags                                    */
	NJS2_TEXSURFACE* texsurface; /* current texture surface                          */
	Uint32          texmode;    /* texture flags                                    */
	Uint32          colormode;  /* color mode flags                                 */
	Uint32          gbix;       /* current global texture index                     */
	Uint32          bank;       /* palette bank                                     */
	Uint32          modmode;    /* modifier flags                                   */
	NJS_TEXLIST*	texlist;    /* current texlist                                  */
	int unk;
	Uint32          texnum;     /* current texture number                           */
}
NJS_CTX;

DataPointer(NJS_CTX*, _nj_curr_ctx_, 0x02670544);

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
	NJS_VECTOR lightColor;
	NJS_VECTOR ambientReg;
	int SomeFlag;
	int field_25;
	int field_29;
};
void sub_426CC0(NJS_MATRIX_PTR result, NJS_VECTOR* a2, NJS_VECTOR* a3, char a4);
VoidFunc(sub_42D340, 0x42D340);
void njColorBlendingMode(int a1, int a2);
FastcallFunctionPointer(float,  njCos, (signed int a1), 0x042AC30);
FastcallFunctionPointer(signed int,  njPushMatrix, (NJS_MATRIX_PTR a1), 0x0077FE10);
FastcallFunctionPointer(float,  njSin, (signed int a1), 0x0042AAB0);
Float njInnerProduct(const NJS_VECTOR* const v1, const NJS_VECTOR* const v2);
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

void njSetTextureNum(int texid);
void njDrawTexture3DExSetData(const NJS_TEXTURE_VTX* a1, int vertexCount, bool pointFiltered = false);

VoidFunc(SaveControl3D, 0x446D00);
VoidFunc(LoadControl3D, 0x446D10);
VoidFunc(SaveConstantAttr, 0x446CB0);
VoidFunc(LoadConstantAttr, 0x446CD0);

void OnControl3D(int flag);
void OffControl3D(int flag);
void OnConstantAttr(int soc_and, int soc_or);
void OffConstantAttr(int soc_and, int soc_or);

FunctionPointer(int, njReleaseTexture, (NJS_TEXLIST* arg0), 0x0077F9F0);
FunctionPointer(void, sub_781CB0, (int a1), 0x781CB0);

DataArray(Light, Lights, 0x01DE4280, 12);
DataArray(LightGC, LightsGC, 0x01DE4420, 12);

#define RotateX(a) if(a) njRotateX(NULL, a)
#define RotateY(a) if(a) njRotateY(NULL, a)
#define RotateZ(a) if(a) njRotateZ(NULL, a)