#pragma once
#include "SA2ModLoader.h"
#include "structs.h"
//#include <d3d9.h>

enum ShaderFlags
{
	None = 0x0,
	UseShadow = 0x1,
	UseFog = 0x2,
	UsePallette = 0x4,
};

enum ChunkMaterialFlagsEnum
{
	DontUseTexture = 0x1,
	HasSecondTexture = 0x2,
	SecondTextureEnvironmentMap = 0x4,
	UseChunkObjectColor = 0x8,
};

DataPointer(WeirdChunkTexIndexThing, WeirdChunkTexIndex, 0x01A275A8);
DataPointer(int, ShaderFlags, 0x01AF1960);
DataPointer(int, ChunkMatEnable, 0x01AED2CC);
DataPointer(int, ChunkMatFlag, 0x01A27594);
DataPointer(int, ChunkObjectColor, 0x01A275A4);
DataPointer(int, dword_1A55828, 0x1A55828);
DataPointer(int, dword_1A5582C, 0x1A5582C);
DataPointer(int, ShaderBackup, 0x01A5579C);
DataPointer(struc_36asd*, struc_36Instance, 0x01A557BC);
DataPointer(Renderer*, dword_1A557C0, 0x1A557C0);
DataPointer(NJS_TEXLIST, AL_BODY, 0x013669FC);

DataArray(int, dword_1298448, 0x01298448, 20);
DataArray(int, dword_1298414, 0x1298414, 12);
DataArray(int, dword_12983DC, 0x12983DC, 12);
DataArray(int, dword_12983A8, 0x12983A8, 12);

FunctionPointer(int, __cdecl sub_56E1E0, (int a1), 0x56E1E0);
FunctionPointer(void, sub_496310, (), 0x496310);
FunctionPointer(int, __cdecl SetPixelShaderFloatAlmostAlwaysTEVMode, (int a1, int a2), 0x0041FCA0);
FunctionPointer(void, sub_56DF50, (NJS_OBJECT* a1), 0x56DF50);
StdcallFunctionPointer(void, SetAndLoadShader, (int a1), 0x00424050);

ThiscallFunctionPointer(void, __thiscall LoadShaders, (struc_36asd *a1), 0x004248B0);
ThiscallFunctionPointer(void, Renderer__SetTexture, (Renderer *a1, int a2, signed int a3, TextureData *pointerToTexture, SamplerData *a5), 0x00867540);

FastcallFunctionPointer(void, __fastcall SetPaletteShader, (int a1, int a2), 0x004243F0);

VoidFunc(GameLoop_BeginScene, 0x00424740);

DataPointer(TextureData**, dword_1A55790, 0x1A55790);

DataArray(int, dword_1A557A8, 0x1A557A8, 4);
DataArray(int, dword_174BC40, 0x1A557A8, 4);
DataArray(int, dword_174BC44, 0x1A557A8, 4);
DataArray(int, dword_174BC58, 0x1A557A8, 4);
DataArray(int, dword_174BC5C, 0x1A557A8, 4);
DataPointer(int, ProbablySpriteFlags, 0x01A557A4);

IDirect3DPixelShader9 *chaoPixelSimpleShader;
IDirect3DDevice9* device = NULL;

bool dontFixTEV = true;