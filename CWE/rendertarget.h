#pragma once
#include <d3d9.h>

IDirect3DTexture9* GetRenderTarget();
void StartRenderTarget();
void StopRenderTarget();
void InitRenderTarget();