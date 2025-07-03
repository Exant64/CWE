#include "stdafx.h"
#include "ninja_functions.h"
#include <math.h>
#include <usercall.h>

static const void* const OnControl3DPtr = (void*)0x446D20;
void OnControl3D(int flag)
{
	__asm
	{
		mov eax, [flag]
		call OnControl3DPtr
	}
}

static const void* const OffControl3DPtr = (void*)0x00446D30;
void OffControl3D(int flag)
{
	__asm
	{
		mov eax, [flag]
		call OffControl3DPtr
	}
}

static const void* const OnConstantAttrPtr = (void*)0x446CF0;
void OnConstantAttr(int soc_and, int soc_or)
{
	__asm
	{
		mov eax, [soc_and]
		mov ecx, [soc_or]
		call OnConstantAttrPtr
	}
}

void OffConstantAttr(int soc_and, int soc_or) {
	*(int*)0x025F02D4 &= ~soc_and;
	*(int*)0x025F0268 &= ~soc_or;
}

const int njColorBlendingModePtr = 0x00426420;
void njColorBlendingMode(int a1, int a2) {
	__asm {
		mov eax, a2
		push a1
		call njColorBlendingModePtr
		add esp, 4
	}
}
void  njCalcVector(NJS_VECTOR* a1, NJS_VECTOR* a2, NJS_MATRIX_PTR a3)
{
	Float v3; // ST00_4
	Float v4; // ST04_4

	v3 = a3[M10] * a1->x + a3[M11] * a1->y + a3[M12] * a1->z;
	v4 = a3[M20] * a1->x + a3[M21] * a1->y + a3[M22] * a1->z;
	a2->x = a3[M01] * a1->y + *a3 * a1->x + a3[M02] * a1->z;
	a2->y = v3;
	a2->z = v4;
}
void sub_426CC0(NJS_MATRIX_PTR result, NJS_VECTOR* a2, NJS_VECTOR* a3, char a4)
{
	NJS_VECTOR v4; // [esp+0h] [ebp-Ch]

	v4.x = result[1] * a3->y + *result * a3->x + result[2] * a3->z;
	v4.y = result[4] * a3->x + result[5] * a3->y + result[6] * a3->z;
	v4.z = result[8] * a3->x + result[9] * a3->y + result[10] * a3->z;
	if (!a4)
	{
		v4.x = result[3] + v4.x;
		v4.y = result[7] + v4.y;
		v4.z = result[11] + v4.z;
	}
	a2->x = v4.x;
	a2->y = v4.y;
	a2->z = v4.z;
}

static const void* const SetShadersPtr = (void*)0x41B1F0;
void SetShaders(int id)
{
	__asm
	{
		mov eax, id
		call SetShadersPtr
	}
}

void njSetTexture(NJS_TEXLIST* texlist) {
	_nj_curr_ctx_->texlist = texlist;
}

void njUnitMatrix(NJS_MATRIX_PTR matrix) {
	if (!matrix) matrix = _nj_current_matrix_ptr_;
	if (matrix)
	{
		memset(matrix, 0, 0x30u);
		*matrix = 1.0;
		matrix[5] = 1.0;
		matrix[10] = 1.0;
	}
}
float njUnitVector(NJS_VECTOR* a1)
{
	float v1; // st7
	float v2; // ST04_4
	float result; // st7
	float v4; // ST04_4

	v1 = a1->y * a1->y + a1->x * a1->x + a1->z * a1->z;
	if (v1 <= 0.0)
	{
		v1 = 0.0;
	}
	v2 = sqrtf(v1);
	result = v2;
	if (v2 > 0.0)
	{
		v4 = 1.0f / result;
		a1->x = a1->x * v4;
		a1->y = v4 * a1->y;
		a1->z = v4 * a1->z;
	}
	return result;
}

const void* const njRotateXPtr = (void*)0x42ADB0;
void njRotateX(NJS_MATRIX_PTR m, Angle x)
{
	__asm
	{
		mov ecx, [m]
		mov eax, [x]
		call njRotateXPtr
	}
}

const void* const njRotateYPtr = (void*)0x42ADD0;
void njRotateY(NJS_MATRIX_PTR m, Angle y)
{
	__asm
	{
		mov ecx, [m]
		mov eax, [y]
		call njRotateYPtr
	}
}

const void* const njRotateZPtr = (void*)0x42ADF0;
void njRotateZ(NJS_MATRIX_PTR m, Angle z)
{
	__asm
	{
		mov ecx, [m]
		mov eax, [z]
		call njRotateZPtr
	}
}

const int njTranslatePtr = 0x0077FD90;
void  njTranslate(float* a1, float a2, float a3, float a4)
{
	__asm
	{
		push a4
		push a3
		push a2
		mov eax, a1
		call njTranslatePtr
		add esp, 12
	}
}

const int njTranslateVPtr = 0x00428A30;
void  njTranslateEx(NJS_VECTOR* a1)
{
	__asm
	{
		mov eax, a1
		call njTranslateVPtr
	}
}

void chCnkDrawObject(NJS_OBJECT* a1)
{
	__asm
	{
		mov esi, a1
		call njCnkDrawObjectPtr
	}
}

const int DrawCnkModelPtr = 0x42D690;
void njCnkDrawModel(NJS_CNK_MODEL* a1)
{
	__asm
	{
		mov eax, a1
		call DrawCnkModelPtr
	}
}
const int njSetTextureNumPtr = 0x0042D6D0;
void njSetTextureNum(int a1, int a2, int a3, int a4)
{
	__asm
	{
		mov eax, a1
		mov ecx, a2
		mov ebx, a3
		push a4
		call njSetTextureNumPtr
		add esp, 4
	}
}

const int njScalePtr = 0x007802B0;
void njScale(NJS_MATRIX_PTR a1, float a2, float a3, float a4)
{
	__asm
	{
		push a4
		push a3
		push a2
		mov eax, a1
		call njScalePtr
		add esp, 12
	}
}
const int sub_426CC0Ptr = 0x426CC0;
__declspec(naked) void njCalcPoint(NJS_MATRIX_PTR result, NJS_VECTOR* a2, NJS_VECTOR* a3, char a4)
{
	__asm
	{
		mov eax, result
		mov edx, a2
		mov ecx, a3
		push dword ptr[a4]
		call sub_426CC0Ptr
		add esp, 4
		retn
	}
}
const int DrawQuadTexturePtr = 0x00782000;
void DrawQuadTexture(int a1, float a2)
{
	__asm
	{
		mov esi, a1
		push a2
		call DrawQuadTexturePtr
		add esp, 4
	}
}

void njSetTextureNum(int texid) {
	NJS_TEXMANAGE* p_texman = (NJS_TEXMANAGE*)_nj_curr_ctx_->texlist->textures[texid].texaddr;
	NJS_TEXSYSTEM* p_texsys = p_texman->texsys;

	_nj_curr_ctx_->texsurface = &p_texsys->texsurface;
	_nj_curr_ctx_->gbix = p_texsys->globalIndex;
	_nj_curr_ctx_->texnum = texid;
}

auto DrawFVF_H_ = GenerateUsercallWrapper<void(*)(const NJS_TEXTURE_VTX * a1, signed int vertexCount)>(noret, 0x781370, rEAX, rECX);
auto sub_41FDE0_ = GenerateUsercallWrapper<void(*)(float* a1, int a2)>(noret, 0x41FDE0, rECX, stack4);

VoidFunc(sub_4293B0, 0x4293B0);
VoidFunc(sub_4292E0, 0x4292E0);

// all this is veeery tacky, point filtering should be handled by RF later on
static void SetSamplerState(int a1) {
	DataPointer(char*, rendererthing, 0x1A557C0);
	int device = *(int*)(rendererthing + 0x38);
	StdcallFunctionPointer(void, SetSamplerStateD3D, (int a1, int Sampler, int Type, int Value), (void*)*(int*)(*(int*)device + 0x114));
	SetSamplerStateD3D(device, 0, 5, a1);
	SetSamplerStateD3D(device, 0, 6, a1);
	SetSamplerStateD3D(device, 0, 7, a1);
}

void njDrawTexture3DExSetData(const NJS_TEXTURE_VTX* a1, int vertexCount, bool pointFiltered) {
	sub_41FDE0_(_nj_current_matrix_ptr_, 0);
	SetShaders(1);
	sub_4293B0();
	sub_4292E0();

	if (pointFiltered) {
		SetSamplerState(1);
	}
	DrawFVF_H_(a1, vertexCount);
	if (pointFiltered) {
		SetSamplerState(2);
	}
}

Float njInnerProduct(const NJS_VECTOR* const v1, const NJS_VECTOR* const v2) {
	return (v1->x * v2->x) + (v1->y * v2->y) + (v1->z * v2->z);
}
