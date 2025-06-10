#pragma once

#include "SA2ModLoader.h"
#include <d3d9.h>
//#include <d3dx9.h>
#include <string>
#include "structs.h"

#include "variables.h"
#include <iostream>
#include <fstream>
#include <string>
#include "ninja_functions.h"
#include "../include/brightfixapi.h"
#include <ninja_functions.h>
#include <ALifeSDK_Functions.h>

const int sub_41FA60Ptr = 0x41FA60;
void  sub_41FA60(WeirdChunkTexIndexThing* a1, signed int a2)
{
	__asm
	{
		mov edi, a1
		push a2
		call sub_41FA60Ptr
		add esp, 4
	}
}

void __cdecl SetChunkTexIndexNull(int a1)
{
	WeirdChunkTexIndex.texturePointer = 0;
	WeirdChunkTexIndex.bank = -1;

	WeirdChunkTexIndex.tes3 = 1;
	WeirdChunkTexIndex.tes4 = 1;
	WeirdChunkTexIndex.a2 = 1;
	WeirdChunkTexIndex.a3 = 1;
	WeirdChunkTexIndex.tes5 = 0;

	sub_41FA60(&WeirdChunkTexIndex, a1);
}
extern "C"
{
	__declspec(noinline) void SetFlag(int on)
	{
	}

	void SetPixelShaderFloat(int reg, float val)
	{
		float vector[4] = { val,0,0,0 };
		device->SetPixelShaderConstantF(reg, vector, 1);
	}

	void SetChunkTexIndexSecondary(int index, int a2, int a3)
	{
		int v3; // esi
		int index_; // ecx
		Uint32 v5; // eax
		int v6; // edx
		int v7; // ecx

		v3 = Has_texlist_batadvPlayerChara_in_it[8];
		index_ = 12 * index;
		v5 = *(int*)(*(int*)(*(int*)v3 + 12 * index + 8) + 12) + 4;
		v6 = *(int*)(v5 + 24) >> 31;
		if (*(int*)(v5 + 24) & 0x8000)
		{
			v7 = *(int*)(*(int*)(*(int*)v3 + index_ + 8) + 8);
			WeirdChunkTexIndex.texturePointer = *(TextureData**)(v5 + 28);
			WeirdChunkTexIndex.bank = v7;
		}
		else
		{
			WeirdChunkTexIndex.texturePointer = *(TextureData**)(v5 + 28);
			WeirdChunkTexIndex.bank = -1;
		}
		WeirdChunkTexIndex.tes3 = 1;
		WeirdChunkTexIndex.tes4 = 1;
		WeirdChunkTexIndex.a2 = a2;
		WeirdChunkTexIndex.a3 = a3;
		WeirdChunkTexIndex.tes5 = v6 != 0;
		//sub_41FA60(&WeirdChunkTexIndex, 0);

		sub_41FA60(&WeirdChunkTexIndex, 2);
		DWORD addressU;
		DWORD addressV;
		device->GetSamplerState(0, D3DSAMPLERSTATETYPE::D3DSAMP_ADDRESSU, &addressU); //texture clipped off in very specific parts, this works around that
		device->GetSamplerState(0, D3DSAMPLERSTATETYPE::D3DSAMP_ADDRESSV, &addressV);
		device->SetSamplerState(2, D3DSAMPLERSTATETYPE::D3DSAMP_ADDRESSU, addressU);
		device->SetSamplerState(2, D3DSAMPLERSTATETYPE::D3DSAMP_ADDRESSV, addressV);
	}

	API __declspec(noinline) void LoadNewShaders()
	{
		SetShaders(1);
		device->SetVertexShader(chaoVertexSimpleShader);
		device->SetPixelShader(chaoPixelSimpleShader);
		SetFlag(0);
	}
	API __declspec(noinline) void CancelNewShaders()
	{
		SetFlag(1);//idk how pixelshaderconstantf works, is it specific to current shader?
		device->SetVertexShader((IDirect3DVertexShader9*)struc_36Instance->Shaders[1]->VertexShader->shaderData);
		device->SetPixelShader((IDirect3DPixelShader9*)struc_36Instance->Shaders[1]->PixelShader->shaderData);
		SetChunkTexIndexNull(2);
		//SetShaders(1);
		//SetShaders(ShaderBackup);
	}
	const int DoLightingPtr = 0x00487060;
	void DoLighting(int a1)
	{
		__asm
		{
			mov eax, a1
			call DoLightingPtr
		}
	}

	void EggStartShaderHook()
	{
		LoadNewShaders();
		DoLighting(LightIndex);
	}
	void EggEndHook()
	{
		CancelNewShaders();
		DoLighting(LightIndexBackupMaybe);
		SetChunkTexIndexNull(2);
	}
	static void __declspec(naked) SetChunkTexIndex()
	{
		__asm
		{
			push[esp + 04h] // a3
			push ebx // a2
			push eax // index

			// Call your __cdecl function here:
			call SetChunkTexIndexSecondary

			pop eax // index
			pop ebx // a2
			add esp, 4 // a3
			retn
		}
	}
	const int GetChunkTexIndexPtr = 0x0056D1F0;
	unsigned int GetChunkTexIndex(NJS_CNK_MODEL* a1)
	{
		int ret;
		__asm
		{
			mov eax, a1
			call GetChunkTexIndexPtr
			mov ret, eax
		}
		return ret;
	}

	const int SetChunkTexIndexOrigPtr = 0x0056E3D0;
	void  SetChunkTexIndexPrimary(int index, int a2, int a3)
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



	void BrightFixPlus_ShinyCheck(int shiny);

	void  ChaoColoringPatch(int texture, int color, int shiny, int highlights, int model)
	{
		signed int v5; // esi
		unsigned __int16 v6; // ax
		int v7; // eax
		bool v8; // zf
		unsigned __int16 v9; // ax

#ifdef SHINY_JEWEL_MONOTONE
		if (shiny == 2 && texture && highlights) // lil extra I added, maybe will resurrect later
		{
			int flag = SecondTextureEnvironmentMap | HasSecondTexture | DontUseTexture;
			ChunkMatEnable = 1;
			SetChunkTexIndexPrimary(17 + texture, 1, 1);
			SetChunkTexIndexSecondary(34, 0, 1);
			if (color)
			{
				flag |= UseChunkObjectColor;
				ChunkObjectColor = dword_1298448[color]; //dword_1298448 = chaoColors
			}
			ChunkMatFlag = flag;
#ifdef BRIGHTFIX_PLUS
			BrightFixPlus_ShinyCheck(shiny);
#endif		
			return;
		}
#endif
#ifdef BRIGHTFIX_PLUS
		BrightFixPlus_ShinyCheck(shiny);
#endif		
		v5 = 0;
		if (!model)
		{
			return;
		}
		if (shiny)
		{
			v5 = 6;
			v6 = GetChunkTexIndex((NJS_CNK_MODEL*)model);
			SetChunkTexIndexPrimary(v6, 1, 1);
			v7 = 34;
		LABEL_6:
			if (highlights && !texture) //HACK: !texture is to fix a problem regarding Shiny jewel monotones
				SetChunkTexIndexPrimary(v7, 1, 1);
			else
				SetChunkTexIndexSecondary(v7, 1, 1);
			//if(highlights && !texture) //HACK: !texture is to fix a problem regarding Shiny jewel monotones
				//SetChunkTexIndexTempDisableFlag(v7, 1, 1);
			//else SetChunkTexIndexReplace(texture + 17, 0, 1);
			v8 = texture == 0;
			goto LABEL_7;
		}
		v8 = texture == 0;
		if (texture > 0)
		{
			ChunkObjectColor = -1;
			v5 = 12;
			v7 = texture + 17;
			SetChunkTexIndexPrimary(v7, 1, 1);
			//SetChunkTexIndexNullZero();
			v8 = texture == 0;
			goto LABEL_7;
		}
	LABEL_7:
		if (v8)
		{
			if (color > 0)
			{
				v5 |= 8u;
				ChunkObjectColor = dword_1298448[color];
			}
			if (highlights)
			{
				v5 |= 1u;
			}
			else
			{
				v9 = GetChunkTexIndex((NJS_CNK_MODEL*)model);
				SetChunkTexIndexPrimary(v9, 1, 1);
			}
		}
		if (v5)
		{
			ChunkMatEnable = 1;
			ChunkMatFlag = v5;
		}
		else
		{
			ChunkMatEnable = 0;
			ChunkMatFlag = 0;
		}
		//float test[4] = { 1,1,1,1 };
		//device->SetPixelShaderConstantF(74, test, 1);
	}

	const int SetShadersPtr = 0x0041B1F0;
	void OverwriteSetShaders()
	{
		__asm
		{
			call SetShadersPtr
			push 2
			call SetChunkTexIndexNull
			add esp, 4
		}
	}

	static void __declspec(naked) ChaoColoring()
	{
		__asm
		{
			push[esp + 10h] // model
			push[esp + 10h] // highlights
			push[esp + 10h] // shiny
			push[esp + 10h] // color
			push edi // texture

			// Call your __cdecl function here:
			call ChaoColoringPatch

			pop edi // texture
			add esp, 4 // color
			add esp, 4 // shiny
			add esp, 4 // highlights
			add esp, 4 // model
			retn
		}
	}
	void PatchEyelidEyeIssue()
	{
		SetChunkTexIndexNull(2);
		ChunkMatEnable = 0;
	}

	void ToyFix(NJS_OBJECT* obj)
	{
		SetFlag(1);
		njCnkDrawObject(obj);
		SetFlag(0);
	}

	void MedalWrap(NJS_OBJECT* object, int(__cdecl* callback)(NJS_CNK_MODEL*))
	{
		SetFlag(1);
		ProcessChunkModelsWithCallback(object, ProcessChunkModel);
		SetFlag(0);
	}

	void  EggColoring(int result, int a2)
	{
		int v2; // edx
		unsigned __int16 v3; // ax
		unsigned __int16 v4; // ax
		int v5; // ecx
		int v6; // ecx

		switch (a2)
		{
		case SA2BEggColour_Normal:
			ChunkMatEnable = 0;
			return;
		case SA2BEggColour_Yellow_MonoTone:
		case SA2BEggColour_White_MonoTone:
		case SA2BEggColour_Brown_MonoTone:
		case SA2BEggColour_SkyBlue_MonoTone:
		case SA2BEggColour_Pink_MonoTone:
		case SA2BEggColour_Blue_MonoTone:
		case SA2BEggColour_Grey_MonoTone:
		case SA2BEggColour_Green_MonoTone:
		case SA2BEggColour_Red_MonoTone:
		case SA2BEggColour_LimeGreen_MonoTone:
		case SA2BEggColour_Purple_MonoTone:
		case SA2BEggColour_Orange_MonoTone:
		case SA2BEggColour_Black_MonoTone:
			v2 = dword_1298448[a2];
			ChunkMatEnable = 1;
			ChunkMatFlag = 9;
			ChunkObjectColor = v2;
			return;
		case SA2BEggColour_Yellow_TwoTone:
		case SA2BEggColour_White_TwoTone:
		case SA2BEggColour_Brown_TwoTone:
		case SA2BEggColour_SkyBlue_TwoTone:
		case SA2BEggColour_Pink_TwoTone:
		case SA2BEggColour_Blue_TwoTone:
		case SA2BEggColour_Grey_TwoTone:
		case SA2BEggColour_Green_TwoTone:
		case SA2BEggColour_Red_TwoTone:
		case SA2BEggColour_LimeGreen_TwoTone:
		case SA2BEggColour_Purple_TwoTone:
		case SA2BEggColour_Orange_TwoTone:
		case SA2BEggColour_Black_TwoTone:
			ChunkMatEnable = 1;
			ChunkMatFlag = UseChunkObjectColor;
			v3 = GetChunkTexIndex((NJS_CNK_MODEL*)result);
			SetChunkTexIndexPrimary(v3, 1, 1);
			ChunkObjectColor = dword_1298414[a2];
			return;
		case SA2BEggColour_NormalShiny:
			ChunkMatFlag = SecondTextureEnvironmentMap | HasSecondTexture;
			njSetTexture(&AL_BODY);
			goto LABEL_6;
		case SA2BEggColour_YellowShiny_MonoTone:
		case SA2BEggColour_WhiteShiny_MonoTone:
		case SA2BEggColour_BrownShiny_MonoTone:
		case SA2BEggColour_SkyBlueShiny_MonoTone:
		case SA2BEggColour_PinkShiny_MonoTone:
		case SA2BEggColour_BlueShiny_MonoTone:
		case SA2BEggColour_GreyShiny_MonoTone:
		case SA2BEggColour_GreenShiny_MonoTone:
		case SA2BEggColour_RedShiny_MonoTone:
		case SA2BEggColour_LimeGreenShiny_MonoTone:
		case SA2BEggColour_PurpleShiny_MonoTone:
		case SA2BEggColour_OrangeShiny_MonoTone:
		case SA2BEggColour_BlackShiny_MonoTone:
			Has_texlist_batadvPlayerChara_in_it[8] = (int)&AL_BODY;
			v5 = dword_12983DC[a2];
			ChunkMatFlag = UseChunkObjectColor | SecondTextureEnvironmentMap | HasSecondTexture | DontUseTexture;
			ChunkObjectColor = v5;
			ChunkMatEnable = 1;
			v4 = GetChunkTexIndex((NJS_CNK_MODEL*)result);
			SetChunkTexIndexPrimary(34, 1, 1);
			break;
		case SA2BEggColour_YellowShiny_TwoTone:
		case SA2BEggColour_WhiteShiny_TwoTone:
		case SA2BEggColour_BrownShiny_TwoTone:
		case SA2BEggColour_SkyBlueShiny_TwoTone:
		case SA2BEggColour_PinkShiny_TwoTone:
		case SA2BEggColour_BlueShiny_TwoTone:
		case SA2BEggColour_GreyShiny_TwoTone:
		case SA2BEggColour_GreenShiny_TwoTone:
		case SA2BEggColour_RedShiny_TwoTone:
		case SA2BEggColour_LimeGreenShiny_TwoTone:
		case SA2BEggColour_PurpleShiny_TwoTone:
		case SA2BEggColour_OrangeShiny_TwoTone:
		case SA2BEggColour_BlackShiny_TwoTone:
			Has_texlist_batadvPlayerChara_in_it[8] = (int)&AL_BODY;
			v6 = dword_12983A8[a2];
			ChunkMatFlag = UseChunkObjectColor | SecondTextureEnvironmentMap | HasSecondTexture;
			ChunkObjectColor = v6;
		LABEL_6:
			ChunkMatEnable = 1;
			v4 = GetChunkTexIndex((NJS_CNK_MODEL*)result);
			SetChunkTexIndexPrimary(v4, 1, 1);
			SetChunkTexIndexSecondary(34, 1, 1);
			break;
		case SA2BEggColour_GlitchyNormal:										//nice IDA thing lol
		case SA2BEggColour_GlitchyNormal | SA2BEggColour_Yellow_MonoTone:
		case SA2BEggColour_GreenShiny_TwoTone | SA2BEggColour_Green_MonoTone:
		case SA2BEggColour_RedShiny_TwoTone | SA2BEggColour_Green_MonoTone:
		case SA2BEggColour_LimeGreenShiny_TwoTone | SA2BEggColour_Green_MonoTone:
		case SA2BEggColour_PurpleShiny_TwoTone | SA2BEggColour_Green_MonoTone:
		case SA2BEggColour_OrangeShiny_TwoTone | SA2BEggColour_Green_MonoTone:
		case SA2BEggColour_BlackShiny_TwoTone | SA2BEggColour_Green_MonoTone:
		case SA2BEggColour_GlitchyNormal | SA2BEggColour_Green_MonoTone:
		case SA2BEggColour_GlitchyNormal | SA2BEggColour_Red_MonoTone:
		case 0x40:
		case 0x41:
		case 0x42:
		case SA2BEggColour_Brown_MonoTone | 0x40:
		case 0x44:
			ChunkObjectColor = -1;
			ChunkMatEnable = 1;
			ChunkMatFlag = 12; //also fixed the egg problem , i made a patch for this before but now it's "baked" into this.
			Has_texlist_batadvPlayerChara_in_it[8] = (int)&AL_BODY;
			SetChunkTexIndexPrimary(a2 - 36, 1, 1);
			break;
		default:
			return;
		}
	}
	static void __declspec(naked) EggColoringHook()
	{

		__asm
		{
			push esi // a2
			push eax // result

			// Call your __cdecl function here:
			call EggColoring

			pop eax // result
			pop esi // a2
			retn
		}
	}

	void BrightFixPlus_ShinyCheck(int shiny)
	{
		if (shiny == 2)
		{
			dontFixTEV = false;
		}
		else dontFixTEV = true;
	}
	const int GXLoadTexMtxImmPtr = 0x00424480;
	__declspec(naked) void GXLoadTexMtxImm(NJS_MATRIX_PTR a1, int index, signed int a3)
	{
		__asm
		{
			mov edx, a1
			mov ebx, index
			push a3
			jmp GXLoadTexMtxImmPtr

		}
	}

	void BrightFixPlus_UV()
	{
		//setting second index to 0 will restore env maps which will make shinies work
		//since 1 is never used afaik because no multi tex support
		if (!dontFixTEV)
		{
			GXLoadTexMtxImm((NJS_MATRIX_PTR)0x01AF1620, 0, 1);
		}
	}
	void BrightFixPlus_TEVModeCheck()
	{
		if (dontFixTEV)
		{
			//bright chao
			SetPixelShaderFloatAlmostAlwaysTEVMode(0, 7);
		}
		else
			SetPixelShaderFloatAlmostAlwaysTEVMode(0, 8);
	}

	DataPointer(int, dword_1A27590, 0x1A27590);
	DataPointer(int, dword_1A27598, 0x1A27590);
	DataPointer(float, dword_1AED2D0, 0x1AED2D0);

	DataPointer(NJS_ARGB, GlobalColorThing, 0x025EFFD0);

	FunctionPointer(void, __cdecl sub_42E730, (NJS_OBJECT* a1), 0x42E730);
	void HatHookTest(NJS_OBJECT* obj)
	{
		SetFlag(1);
		sub_42E730(obj);
		SetFlag(0);
	}

	FunctionPointer(int, sub_5A57C0, (int a1, __int16 a2, int a3), 0x5A57C0);
	DataPointer(int, struct36Instance, 0x01A557BC);
	void __cdecl sub_5A57C0Hook(int a1, __int16 a2, int a3)
	{
		//on shiny twotone, copy secondary unused env map matrix into main matrix
		//shader has an untouched UV slot so i just use that when i see that its in shiny twotone mode
		if (!dontFixTEV)
		{
			float texGenSrc = 1;
			device->SetVertexShaderConstantF(160, (float*)(struct36Instance + 0x3E8 + 64), 4);
			device->SetVertexShaderConstantF(140, &texGenSrc, 1);
		}

		sub_5A57C0(a1, a2, a3);
	}

	unsigned char shaderExport[508] = {
	0x00, 0x03, 0xFF, 0xFF, 0x51, 0x00, 0x00, 0x05, 0x01, 0x00, 0x0F, 0xA0,
	0x00, 0x00, 0x80, 0xBF, 0x00, 0x00, 0x80, 0xC0, 0x00, 0x00, 0xC0, 0x40,
	0x2F, 0xBA, 0x68, 0x3F, 0x1F, 0x00, 0x00, 0x02, 0x05, 0x00, 0x03, 0x80,
	0x00, 0x00, 0x2F, 0x90, 0x1F, 0x00, 0x00, 0x02, 0x05, 0x00, 0x02, 0x80,
	0x02, 0x00, 0x03, 0x90, 0x1F, 0x00, 0x00, 0x02, 0x05, 0x00, 0x07, 0x80,
	0x01, 0x00, 0x03, 0x90, 0x1F, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x90,
	0x00, 0x08, 0x0F, 0xA0, 0x1F, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x90,
	0x01, 0x08, 0x0F, 0xA0, 0x1F, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x90,
	0x02, 0x08, 0x0F, 0xA0, 0x01, 0x00, 0x00, 0x02, 0x00, 0x00, 0x07, 0x80,
	0x01, 0x00, 0xE4, 0xA0, 0x01, 0x00, 0x00, 0x02, 0x06, 0x00, 0x01, 0x80,
	0x4A, 0x00, 0xE4, 0xA0, 0x01, 0x00, 0x00, 0x02, 0x08, 0x00, 0x03, 0x80,
	0x01, 0x00, 0x54, 0x90, 0x12, 0x00, 0x00, 0x04, 0x07, 0x00, 0x03, 0x80,
	0x06, 0x00, 0x00, 0x80, 0x08, 0x00, 0xE4, 0x80, 0x02, 0x00, 0x54, 0x90,
	0x02, 0x00, 0x00, 0x03, 0x00, 0x00, 0x04, 0x80, 0x00, 0x00, 0xAA, 0x80,
	0x00, 0x00, 0x00, 0xA1, 0x42, 0x00, 0x00, 0x03, 0x01, 0x00, 0x0F, 0x80,
	0x07, 0x00, 0xE4, 0x80, 0x00, 0x08, 0xE4, 0xA0, 0x42, 0x00, 0x00, 0x03,
	0x03, 0x00, 0x0F, 0x80, 0x08, 0x00, 0xE4, 0x80, 0x02, 0x08, 0xE4, 0xA0,
	0x02, 0x00, 0x00, 0x03, 0x02, 0x00, 0x3F, 0x80, 0x01, 0x00, 0xE4, 0x80,
	0x00, 0x00, 0xE4, 0x90, 0x01, 0x00, 0x00, 0x02, 0x02, 0x00, 0x08, 0x80,
	0x00, 0x00, 0xFF, 0x90, 0x58, 0x00, 0x00, 0x04, 0x02, 0x00, 0x2F, 0x80,
	0x00, 0x00, 0xAA, 0x80, 0x00, 0x00, 0xE4, 0x90, 0x02, 0x00, 0xE4, 0x80,
	0x02, 0x00, 0x00, 0x03, 0x00, 0x00, 0x03, 0x80, 0x00, 0x00, 0xE4, 0x80,
	0x00, 0x00, 0x00, 0xA0, 0x58, 0x00, 0x00, 0x04, 0x02, 0x00, 0x2F, 0x80,
	0x00, 0x00, 0x55, 0x80, 0x02, 0x00, 0xE4, 0x80, 0x01, 0x00, 0xE4, 0x80,
	0x05, 0x00, 0x00, 0x03, 0x01, 0x00, 0x2F, 0x80, 0x01, 0x00, 0xE4, 0x80,
	0x00, 0x00, 0xE4, 0x90, 0x58, 0x00, 0x00, 0x04, 0x00, 0x00, 0x2F, 0x80,
	0x00, 0x00, 0x00, 0x80, 0x02, 0x00, 0xE4, 0x80, 0x01, 0x00, 0xE4, 0x80,
	0x0F, 0x00, 0x00, 0x02, 0x01, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x80,
	0x0F, 0x00, 0x00, 0x02, 0x01, 0x00, 0x02, 0x80, 0x00, 0x00, 0x55, 0x80,
	0x0F, 0x00, 0x00, 0x02, 0x01, 0x00, 0x04, 0x80, 0x00, 0x00, 0xAA, 0x80,
	0x01, 0x00, 0x00, 0x02, 0x00, 0x08, 0x28, 0x80, 0x00, 0x00, 0xFF, 0x80,
	0x05, 0x00, 0x00, 0x03, 0x00, 0x00, 0x07, 0x80, 0x01, 0x00, 0xE4, 0x80,
	0x01, 0x00, 0xFF, 0xA0, 0x0E, 0x00, 0x00, 0x02, 0x01, 0x00, 0x21, 0x80,
	0x00, 0x00, 0x00, 0x80, 0x0E, 0x00, 0x00, 0x02, 0x01, 0x00, 0x22, 0x80,
	0x00, 0x00, 0x55, 0x80, 0x0E, 0x00, 0x00, 0x02, 0x01, 0x00, 0x24, 0x80,
	0x00, 0x00, 0xAA, 0x80, 0x05, 0x00, 0x00, 0x03, 0x03, 0x00, 0x07, 0x80,
	0x03, 0x00, 0xE4, 0x80, 0x01, 0x00, 0xFF, 0xA0, 0x02, 0x00, 0x00, 0x03,
	0x03, 0x00, 0x21, 0x80, 0x03, 0x00, 0x00, 0x80, 0x01, 0x00, 0x00, 0x80,
	0x02, 0x00, 0x00, 0x03, 0x03, 0x00, 0x22, 0x80, 0x03, 0x00, 0x55, 0x80,
	0x01, 0x00, 0x55, 0x80, 0x02, 0x00, 0x00, 0x03, 0x03, 0x00, 0x24, 0x80,
	0x03, 0x00, 0xAA, 0x80, 0x01, 0x00, 0xAA, 0x80, 0x05, 0x00, 0x00, 0x03,
	0x00, 0x08, 0x27, 0x80, 0x03, 0x00, 0xE4, 0x80, 0x46, 0x00, 0xE4, 0xA0,
	0xFF, 0xFF, 0x00, 0x00
	};

	IDirect3DVertexShader9* vertShader;

#ifdef BRIGHTFIX_PLUS
	API void __cdecl BrightFix_Init(const char* path, BYTE* vertexShader, BYTE* shaderData)
#else
	__declspec(dllexport) void __cdecl Init(const char* path)
#endif
	{ 
		device = dword_1A557C0->pointerToDevice;

		if ((device->CreateVertexShader((DWORD*)vertexShader, &chaoVertexSimpleShader)) != D3D_OK)//creates shaders
		{
			MessageBoxA(0, "BrightFix", "Cant load shader!", 0);
		}

		if ((device->CreatePixelShader((DWORD*)shaderData, &chaoPixelSimpleShader)) != D3D_OK)//creates shaders
		{
			MessageBoxA(0, "BrightFix", "Cant load shader!", 0);
		}
		//
		//Ambient test
		//WriteJump((void*)0x0056E3C0, AmbientHook);

		//PaletteFix_Init();
		//ShadowHook_Init();
#ifdef BRIGHTFIX_PLUS
		WriteCall((void*)0x0056DC1A, BrightFixPlus_TEVModeCheck);
#endif

		WriteCall((void*)0x0053FE8A, ToyFix); //toy fix - same as the model wrap below 
		WriteCall((void*)0x005401FD, MedalWrap); //medal fix - because of the flag and the medals drawing in the same place as chao, they looked blank so I had to make this hack to fix them
		WriteCall((void*)0x00540E4B, MedalWrap); //left hand weird toy draw shit fix
		WriteCall((void*)0x00540EE7, MedalWrap); //right hand weird toy draw shit fix

		WriteCall((void*)0x057B6C5, EggStartShaderHook); //egg
		WriteJump((void*)0x0057B979, EggEndHook);

		WriteJump((void*)0x0056D2A0, EggColoringHook); //hooks eggcoloring

		WriteCall((void*)0x0053FF13, HatHookTest); //fix hook environment map
		WriteCall((void*)0x0053FF7B, HatHookTest);
		WriteCall((void*)0x0053FF91, HatHookTest);
		WriteCall((void*)0x0053FFA7, HatHookTest);

		WriteCall((void*)0x054FFA1, LoadNewShaders); //chao setshaders hook
		WriteCall((void*)0x0055017E, CancelNewShaders);
		WriteCall((void*)0x0056D54D, LoadNewShaders); //eggcoloring general hook
		WriteJump((void*)0x0056D58A, CancelNewShaders);

		//this doesnt do anything on brightfixplus since we hook it anyways for coexistence
		WriteData((char*)0x56DC17, (char)0); //hack tevmode so brightness doesnt happen

		//WriteData((char*)0x56DC71, (char)0); //reenable environment map
		//WriteCall((void*)0x0056DC77, BrightFixPlus_UV);
		WriteCall((void*)0x0056DC8A, sub_5A57C0Hook);

		WriteJump((void*)0x0056D470, ChaoColoring); //chaocoloring hook
		WriteCall((void*)0x00540402, PatchEyelidEyeIssue); //fixes the "Everything is shiny" glitch
	}
#ifndef BRIGHTFIX_PLUS
	__declspec(dllexport) ModInfo SA2ModInfo = { ModLoaderVer };
#endif
}

