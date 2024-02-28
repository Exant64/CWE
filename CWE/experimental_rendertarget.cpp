#include "stdafx.h"
#include <al_save.h>
#include <alg_kinder_bl.h>
#include <d3dx9.h>
#include <ninja_functions.h>

extern IDirect3DDevice9* cwe_device;

const int SetChunkTexIndexOrigPtr = 0x0056E3D0;
void  SetChunkTexIndexPrimaryOrig(int index, int a2, int a3)
{
	__asm
	{
		push a3
		mov ebx, a2
		mov eax, index
		call SetChunkTexIndexOrigPtr
		add esp, 4
	}
}

IDirect3DSurface9* RenderTarget;
IDirect3DSurface9* OutSurface;
IDirect3DSurface9* FStaticStencil;
IDirect3DTexture9* OutTexture;

void __cdecl SetChunkTexIndexPrimaryFix(int a1, int a2, int a3)
{
	/*
	NJS_TEXLIST* texlist = (NJS_TEXLIST*)Has_texlist_batadvPlayerChara_in_it[8];
	if (texlist->textures[a1].texaddr == 0)
	{
		SetDebugFontColor(0xFF00FF00);
		DisplayDebugStringFormatted(NJM_LOCATION(5, 5), "MISSING TEXTURE %00x %d", texlist, a1);
		SetChunkTexIndexNull(0);
	}
	else
		SetChunkTexIndexPrimaryOrig(a1, a2, a3);

	*/
#ifdef RENDERTARGET
	device->SetTexture(0, OutTexture);
#endif
}
static void __declspec(naked) SetChunkTexIndexPrimaryHook()
{
	__asm
	{
		push[esp + 04h] // a3
		push ebx // a2
		push eax // index

		// Call your __cdecl function here:
		call SetChunkTexIndexPrimaryFix

		pop eax // index
		pop ebx // a2
		add esp, 4 // a3
		retn
	}
}

const int sub_458C30Ptr = 0x458C30;
void sub_458C30(int a1)
{
	__asm {
		mov eax, a1
		call sub_458C30Ptr
	}
}

void BeginScene_Ex() {
	VoidFunc(BeginScene, 0x0041B2D0);


	//cwe_device->BeginScene();



	//cwe_device->EndScene();

	RECT rect = { 0,0,256,256 };
	//cwe_device->StretchRect(RenderTarget, NULL, OutSurface, NULL, D3DTEXTUREFILTERTYPE::D3DTEXF_LINEAR);

	//D3DXSaveTextureToFileA("F:\\test.png", D3DXIMAGE_FILEFORMAT::D3DXIFF_PNG, OutTexture, 0);
	//cwe_device->SetRenderTarget(0, 0);

	cwe_device->EndScene();

	cwe_device->SetRenderTarget(1, RenderTarget);
	cwe_device->SetDepthStencilSurface(FStaticStencil);
	cwe_device->BeginScene();
	cwe_device->Clear(0, 0, 1 | 2, 0, 1, 0);
	cwe_device->SetRenderState(D3DRENDERSTATETYPE::D3DRS_ZFUNC, D3DCMPFUNC::D3DCMP_ALWAYS);
	njPushMatrixEx();
	Rotation rot = { 0,0,0 };
	SAlItem item = { 3, 2 };
	DrawItem(0, 0, 1, rot, item);
	njPopMatrixEx();
	cwe_device->EndScene();

	cwe_device->StretchRect(RenderTarget, NULL, OutSurface, NULL, D3DTEXTUREFILTERTYPE::D3DTEXF_LINEAR);
	D3DXSaveTextureToFileA("F:\\test.png", D3DXIMAGE_FILEFORMAT::D3DXIFF_PNG, OutTexture, 0);

	BeginScene();
}