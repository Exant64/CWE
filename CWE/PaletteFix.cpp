#include "stdafx.h"
#include "SA2ModLoader.h"
#include <d3d9.h>
#include <map>
#include <vector>
#include <algorithm>
#include "ALifeSDK_Functions.h"

struct TextureData
{
	int* dword0;
	int dword4;
	int dword8;
	IDirect3DTexture9* D3DTexture;
	int dword10;
	char gap14[4];
	char byte18;
	char gap19[15];
	int dword28;
	int dword2C;
	char gap30[4];
	char byte34;
	char gap35[15];
	int dword44;
	int dword48;
	int dword4C;
	int dword50;
	int dword54;
	int dword58;
	int dword5C;
	int dword60;
	int dword64;
	int dword68;
	int dword6C;
	int byte70;
};

struct  WeirdChunkTexIndexThing
{
	TextureData* texturePointer;
	int a3;
	int a2;
	int tes3;
	int tes4;
	int tes5;
	int bank;
};

static IDirect3DTexture9* PaletteSource[3]; //used for source in d3d device UpdateTexture
//static IDirect3DTexture9* PaletteDest;  //same but dest
enum
{
	None = 0x0,
	UseShadow = 0x1,
	UseFog = 0x2,
	UsePallette = 0x4,
};
DataPointer(int, ShaderBackup, 0x01A5579C);
DataPointer(int, ShaderFlags, 0x01AF1960);
static IDirect3DDevice9* pDevice;

Uint16 PaletteBuffer[256][16];

std::vector<IDirect3DTexture9*> TexPalMap;
void TexturePaletteInsertInCache(Uint32* pSurface, Uint32 bank) {
	TextureData* texData = (TextureData*)pSurface;
	IDirect3DTexture9* tex = (IDirect3DTexture9*)texData->D3DTexture;

	D3DLOCKED_RECT tex1Rect;
	D3DLOCKED_RECT resultRect;

	int index = TexPalMap.size();
	if (index >= 3) {
		throw std::exception("more than 3 palletized textures per chao??? report this");
	}

	TexPalMap.push_back(tex);

	IDirect3DTexture9* palettizedTex = PaletteSource[index];

	palettizedTex->LockRect(0, &resultRect, 0, D3DLOCK_DISCARD);
	tex->LockRect(0, &tex1Rect, 0, D3DLOCK_READONLY);

	const unsigned char* tex1Bits = (unsigned char*)(tex1Rect.pBits); //format of the grayscale image is A8
	const unsigned short* palBits = PaletteBuffer[bank]; //all these are R5G6B5, hence its unsigned short
	unsigned short* resultBits = (unsigned short*)(resultRect.pBits);

	for (int i = 0; i < 32 * 32; i++) {
		resultBits[i] = palBits[tex1Bits[i] & 15];
	}

	tex->UnlockRect(0);
	palettizedTex->UnlockRect(0);

}

static void PaletteFix_SetPaletteShader(WeirdChunkTexIndexThing* a1, int bank)
{
	int v5; // ecx
	int v6; // eax
	//if (bank)
	{

		TextureData* texData = a1->texturePointer;
		if (texData) {
			IDirect3DTexture9* tex = (IDirect3DTexture9*)texData->D3DTexture;
			auto findResult = std::find(TexPalMap.begin(), TexPalMap.end(), tex);

			if (findResult == TexPalMap.end()) {
				//throw std::exception("TEXTURE WASN'T CACHED");
				return;
			}

			int index = std::distance(TexPalMap.begin(), findResult);
			pDevice->SetTexture(0, PaletteSource[index]);
		}
		return;
	}

	v5 = ShaderFlags;
	v6 = ShaderFlags & ~UsePallette;
	ShaderFlags = v6;
	if (v5 != v6)
	{
		SetShaders(1);
		SetShaders(ShaderBackup);
	}
}
static void __declspec(naked) PaletteFix_SetPaletteShaderHook()
{
	__asm
	{
		push edx // a2
		push edi // a1

		// Call your __cdecl function here:
		call PaletteFix_SetPaletteShader

		add esp, 8
		retn
	}
}



void __stdcall PaletteFix_SetPalette(int index) {
	memcpy(PaletteBuffer[index], *(void**)0x01DBED80, sizeof(PaletteBuffer[index]));
}

void PaletteFix_Init(IDirect3DDevice9* device)
{
	WriteData((char*)0x0041FB79, (char)0x14); //patch so we can send the bank index to SetPaletteShader

	pDevice = device;

	for (int i = 0; i < 3; i++) {
		pDevice->CreateTexture(32, 32, 1, 0, D3DFMT_R5G6B5, D3DPOOL_MANAGED, &PaletteSource[i], 0);
	}
	
	WriteJump((void*)0x41AB40, PaletteFix_SetPalette);
	WriteJump((void*)0x004243F0, PaletteFix_SetPaletteShaderHook);
}