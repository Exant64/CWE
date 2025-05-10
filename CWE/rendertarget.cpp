#include "stdafx.h"
#include <al_save.h>
#include <alg_kinder_bl.h>
#include <d3dx9.h>
#include <ninja_functions.h>
#include <ChaoMain.h>

static IDirect3DTexture9* RenderTarget;
static IDirect3DSurface9* FStaticStencil;

static IDirect3DSurface9* BackupTarget, *BackupDepthStencil;

static DWORD BackupColorWriteEnable;

IDirect3DTexture9* GetRenderTarget() {
    return RenderTarget;
}

void StartRenderTarget() {
    cwe_device->GetRenderState(D3DRS_COLORWRITEENABLE, &BackupColorWriteEnable);
    cwe_device->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA);

    cwe_device->GetRenderTarget(0, &BackupTarget);
    cwe_device->GetDepthStencilSurface(&BackupDepthStencil);

    IDirect3DSurface9* surf;
    RenderTarget->GetSurfaceLevel(0, &surf);
    cwe_device->SetRenderTarget(0, surf);
    cwe_device->SetDepthStencilSurface(FStaticStencil);
    cwe_device->Clear(0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL,
        0x00000000, 1.0f, 0L);
}

void StopRenderTarget() {
    cwe_device->SetRenderState(D3DRS_COLORWRITEENABLE, BackupColorWriteEnable);

    cwe_device->SetRenderTarget(0, BackupTarget);
    cwe_device->SetDepthStencilSurface(BackupDepthStencil);
}

void InitRenderTarget() {
    UINT hres = UINT(512.f * HorizontalResolution / VerticalResolution);
    cwe_device->CreateTexture(hres, 512,
        1, D3DUSAGE_RENDERTARGET,
        D3DFMT_A8B8G8R8,
        D3DPOOL_DEFAULT,
        &RenderTarget,
        NULL
    );

    cwe_device->CreateDepthStencilSurface(
        hres,
        512,
        D3DFMT_D24S8,
        D3DMULTISAMPLE_NONE,
        0,
        TRUE,
        &FStaticStencil,
        NULL
    );
}