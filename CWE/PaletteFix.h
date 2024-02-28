#pragma once
#include <d3d9.h>
#include <vector>

extern std::vector<IDirect3DTexture9*> TexPalMap;
void TexturePaletteInsertInCache(Uint32* pSurface, Uint32 bank);
void PaletteFix_Init(IDirect3DDevice9* device); 