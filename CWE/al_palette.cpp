#include "stdafx.h"

#include "Chao.h"
#include "al_palette.h"
#include "al_parameter.h"

#include "Trampoline.h"
#include "PaletteFix.h"
#include <brightfixapi.h>
#include <al_draw.h>

char gPaletteNegative[PVP_COUNT][192];
char gPaletteRegular[PVP_COUNT][192];
//#define gPalette byte_1A13BF8
const char* gPaletteFilename[39] =
{
  "XL_NC00.GVP",
  "XL_NC01.GVP",
  "XL_HCZ.GVP",
  "XL_HCN.GVP",
  "XL_HCS.GVP",
  "XL_HCF.GVP",
  "XL_HCR.GVP",
  "XL_HCP.GVP",
  "XL_DC.GVP",
  "XL_HNZ.GVP",
  "XL_HNN.GVP",
  "XL_HNS.GVP",
  "XL_HNF.GVP",
  "XL_HNR.GVP",
  "XL_HNP.GVP",
  "XL_HSZ.GVP",
  "XL_HSN.GVP",
  "XL_HSS.GVP",
  "XL_HSF.GVP",
  "XL_HSR.GVP",
  "XL_HSP.GVP",
  "XL_HFZ.GVP",
  "XL_HFN.GVP",
  "XL_HFS.GVP",
  "XL_HFF.GVP",
  "XL_HFR.GVP",
  "XL_HFP.GVP",
  "XL_HRZ.GVP",
  "XL_HRN.GVP",
  "XL_HRS.GVP",
  "XL_HRF.GVP",
  "XL_HRR.GVP",
  "XL_HRP.GVP",
  "XL_HPZ.GVP",
  "XL_HPN.GVP",
  "XL_HPS.GVP",
  "XL_HPF.GVP",
  "XL_HPR.GVP",
  "XL_HPP.GVP"
};
const char* gPalette8Filename[] =
{
  "AL8_NC00.GVP",
  "AL8_NC01.GVP",
  "AL8_HCZ.GVP",
  "AL8_HCN.GVP",
  "AL8_HCS.GVP",
  "AL8_HCF.GVP",
  "AL8_HCR.GVP",
  "AL8_HCP.GVP",
  "AL8_DC.GVP",
};
const char* off_1365D78[39] =
{
  (const char* )20012932,
  (const char*)20014132,
  (const char*)20015660,
  (const char*)20015956,
  (const char*)20016772,
  (const char*)20017276,
  (const char*)20017780,
  (const char*)20018572,
  (const char*)20019076,
  (const char*)20019580,
  (const char*)20020084,
  (const char*)20020876,
  (const char*)20021380,
  (const char*)20021884,
  (const char*)20022388,
  (const char*)20023164,
  (const char*)20023628,
  (const char*)20023940,
  (const char*)20024404,
  (const char*)20024868,
  (const char*)20025332,
  (const char*)20025644,
  (const char*)20026108,
  (const char*)20026572,
  (const char*)20026884,
  (const char*)20028156,
  (const char*)20028620,
  (const char*)20028932,
  (const char*)20029396,
  (const char*)20029860,
  (const char*)20030324,
  (const char*)20030636,
  (const char*)20031100,
  (const char*)20031564,
  (const char*)20031876,
  (const char*)20033148,
  (const char*)20033612,
  (const char*)20033924,
  (const char*)20034388,
#ifdef NEUTPALETTE
  "AL_NNZ.GVP",
  "AL_NNN.GVP",
  "AL_NNS.GVP",
  "AL_NNF.GVP",
  "AL_NNR.GVP",
  "AL_NNP.GVP",
#endif
#ifdef DARKPALETTE
   "AL_DFZ.GVP",
  "AL_DFN.GVP",
  "AL_DFS.GVP",
  "AL_DFF.GVP",
  "AL_DFR.GVP",
  "AL_DFP.GVP",
   "AL_DNZ.GVP",
  "AL_DNN.GVP",
  "AL_DNS.GVP",
  "AL_DNF.GVP",
  "AL_DNR.GVP",
  "AL_DNP.GVP",
#endif
};

#if 0
const int sub_534E10Ptr = 0x534E10;
void alpalSetBank(ObjectMaster* a1, int a2)
{
	__asm
	{
		mov eax, a1
		mov ecx, a2
		call sub_534E10Ptr
	}
}
#endif

#pragma pack(push, 8)
struct __declspec(align(4)) NJS_TEXSURFACE_
{
	Uint32 Type;
	Uint32 BitDepth;
	Uint32 PixelFormat;
	Uint32 nWidth;
	Uint32 nHeight;
	Uint32 TextureSize;
	Uint32 fSurfaceFlags;
	Uint32* pSurface;
};
#pragma pack(pop)


struct NJS_TEXSYSTEM
{
	Uint32 globalIndex;
	NJS_TEXSURFACE_ texsurface;
	Int count;
};

struct NJS_TEXMANAGE
{
	Uint32 tspparam;
	Uint32 texparam;
	Uint32 bank;
	NJS_TEXSYSTEM* texsys;
	Int count;
	Uint32 texflag;
};

int dword_1365D30[] = 
{ 
	0, 1, 2, //child chao
	3, 4, 5, //hero normal
	6, 7, 8, //hero swim
	9, 10, 11, //hero fly
	12, 13, 14, //hero run
	15, 16, 17, //hero power
	0, 1, 2, //neutral normal
	46,47,48, //dark fly
	49,50,51, //dark normal
};

void __cdecl alpalSetBank(ObjectMaster* a1, int a2)
{
	int v2; // eax
	NJS_TEXLIST* currentTexlist;

	TexPalMap.clear();

	switch (a1->Data1.Chao->pParamGC->Type)
	{
	case 2:
		v2 = 0;
		break;
	case 6:
		v2 = 1;
		break;
	case 9:
		v2 = 2;
		break;
	case 12:
		v2 = 3;
		break;
	case 15:
		v2 = 4;
		break;
	case 18:
		v2 = 5;
		break;
#ifdef NEUTPALETTE
	case ChaoType_Neutral_Normal:
		v2 = 6;
		break;
#endif
#ifdef DARKPALETTE
	case ChaoType_Dark_Fly:
		v2 = 7;
		break;
	case ChaoType_Dark_Normal:
		v2 = 8;
		break;
#endif
	default:
		return;
	}
	currentTexlist = (NJS_TEXLIST*)(Has_texlist_batadvPlayerChara_in_it[8]);
	for (int i = 0; i < 3; i++) {
		int texIndex = dword_1365D30[v2 * 3 + i];
		NJS_TEXMANAGE* pTexManage = (NJS_TEXMANAGE*)(currentTexlist->textures[texIndex].texaddr);
		Uint32* pSurface = pTexManage->texsys->texsurface.pSurface;

		pTexManage->bank = 3 * a2 + i;
		TexturePaletteInsertInCache(pSurface, pTexManage->bank);
	}
}

static void __declspec(naked) alpalSetBankHook()
{
	__asm
	{
		push ecx // a2
		push eax // a1

		// Call your __cdecl function here:
		call alpalSetBank

		pop eax // a1
		pop ecx // a2
		retn
	}
}

//we removed the alpalSetBank calls from DrawChao, meaning now it only sets bank once before entering DrawChao
//however to apply it to the right texlist (negative/regular/whatever in the future) we also need to call SetBodyTexture before it
void __cdecl alpalAL_DrawHook(ObjectMaster* a1, int a2) {
	AL_SetBodyTexture(a1);
	alpalSetBank(a1, a2);
}

static void __declspec(naked) alpalSetBankDrawChaoHook()
{
	__asm
	{
		push ecx // a2
		push eax // a1

		// Call your __cdecl function here:
		call alpalAL_DrawHook

		pop eax // a1
		pop ecx // a2
		retn
	}
}


void LoadPaletteHook();
Trampoline LoadPalette_t(0x0534350, 0x053435A, LoadPaletteHook);
void LoadPaletteHook()
{
	VoidFunc(original, LoadPalette_t.Target());
	WriteData((int*)0x00534381, (int)gPaletteFilename);
	WriteData((int*)0x00534603, (int)&gPaletteFilename[LengthOfArray(gPaletteFilename)]);
	WriteData((int*)0x0053437C, (int)gPaletteNegative);
	original();
	WriteData((int*)0x00534381, (int)off_1365D78);
	WriteData((int*)0x00534603, (int)&off_1365D78[LengthOfArray(off_1365D78)]);
	WriteData((int*)0x0053437C, (int)gPaletteRegular);
	original();
}


__int16 sub_420F10(unsigned __int8 a1, unsigned __int8 a2, unsigned __int8 a3)
{
	return 225 * a3 & 31 | 32 * (((unsigned __int16)(31 * a2 / 255) << 6) | 193 * a1 & 0x3F);
}
StdcallFunctionPointer(void, sub_41AB40, (signed int a1), 0x41AB40);

const int UpdateChaoPalettePtr = 0x0534670;
void __cdecl UpdateChaoPalette(ChaoDataBase* a1, ObjectMaster* a2, int a3)
{
	__asm
	{
		mov eax, a1
		mov ecx, a2
		push a3
		call UpdateChaoPalettePtr
		add esp, 4
	}
}

void __cdecl AL_PaletteSetColorRatio(ChaoDataBase* chaoData, ObjectMaster* a1, int cno, char gPalette[][48 * 4])
{
	ChaoType v3; // dl
	signed int v4; // ecx
	float flyswim; // st7
	NJS_BGRA* AL_NC00; // edi
	NJS_BGRA* AL_NC01; // ebp
	NJS_BGRA* AL_HCP; // ebx
	float v9; // st6
	signed int v10; // esi
	unsigned __int8 v11; // bl
	signed int v12; // esi
	signed int v13; // ecx
	char* v14; // ebx
	char* v15; // edi
	char* v16; // ebp
	float v17; // st3
	signed int v18; // esi
	unsigned __int8 v19; // [esp+7h] [ebp-101h]
	float powerrun; // [esp+8h] [ebp-100h]
	float powerruna; // [esp+8h] [ebp-100h]
	unsigned __int8 v22; // [esp+Eh] [ebp-FAh]
	unsigned __int8 v23; // [esp+Fh] [ebp-F9h]
	float v24; // [esp+10h] [ebp-F8h]
	NJS_BGRA* AL_DC; // [esp+18h] [ebp-F0h]
	int AL_DCa; // [esp+18h] [ebp-F0h]
	char* AL_DCb; // [esp+18h] [ebp-F0h]
	unsigned __int16* v28; // [esp+1Ch] [ebp-ECh]
	float Magnitude; // [esp+1Ch] [ebp-ECh]
	NJS_BGRA* AL_HCN; // [esp+20h] [ebp-E8h]
	int AL_HCNa; // [esp+20h] [ebp-E8h]
	int AL_HCNb; // [esp+20h] [ebp-E8h]
	float v33; // [esp+24h] [ebp-E4h]
	int* v34; // [esp+28h] [ebp-E0h]
	float evolution; // [esp+2Ch] [ebp-DCh]
	float v36; // [esp+30h] [ebp-D8h]
	unsigned __int16* v37; // [esp+30h] [ebp-D8h]
	NJS_BGRA* AL_HCF; // [esp+34h] [ebp-D4h]
	NJS_BGRA* AL_HCZ; // [esp+38h] [ebp-D0h]
	float v40; // [esp+3Ch] [ebp-CCh]
	float v41; // [esp+3Ch] [ebp-CCh]
	float v42; // [esp+40h] [ebp-C8h]
	float v43; // [esp+40h] [ebp-C8h]
	float v44; // [esp+44h] [ebp-C4h]
	char v45[192]; // [esp+48h] [ebp-C0h]
	int v85;
	int v86;

	v34 = (int*)v45;
	v3 = (ChaoType)chaoData->Type;
	if (AL_IsHero(v3) && chaoData->LobbyTextureValue > 0) 
		v3 = (ChaoType)((chaoData->LobbyTextureValue + 1) * 3);
	//lobbytextureval = 1 -> v3 = 6
	//lobbytextureval = 2 -> v3 = 9
	//formula ^ for chaotype override

	switch ((unsigned __int8)v3)
	{
	case ChaoType_Child:
		v4 = PVP_NCZ;
		break;
	case ChaoType_Hero_Normal:
		v4 = PVP_HNZ;
		break;
	case ChaoType_Hero_Swim:
		v4 = PVP_HSZ;
		break;
	case ChaoType_Hero_Fly:
		v4 = PVP_HFZ;
		break;
	case ChaoType_Hero_Run:
		v4 = PVP_HRZ;
		break;
	case ChaoType_Hero_Power:
		v4 = PVP_HPZ;
		break;
#ifdef NEUTPALETTE
	case ChaoType_Neutral_Normal:
		v4 = PVP_NNZ;
		break;
#endif
#ifdef DARKPALETTE
	case ChaoType_Dark_Fly:
		v4 = PVP_DFZ;
		break;
	case ChaoType_Dark_Normal:
		v4 = PVP_DNZ;
		break;
#endif
	default:
		return;
	}
	

	flyswim = chaoData->FlySwim;
	if (v3 == ChaoType_Child)
	{
		evolution = chaoData->EvolutionProgress;
		powerrun = chaoData->PowerRun;
		AL_NC00 = (NJS_BGRA*)gPalette[PVP_NCZ];
		AL_NC01 = (NJS_BGRA*)gPalette[PVP_NCN];
		AL_HCZ = (NJS_BGRA*)gPalette[PVP_HCZ];
		AL_HCN = (NJS_BGRA*)gPalette[PVP_HCN];
		AL_HCP = (NJS_BGRA*)gPalette[PVP_HCP];
		AL_HCF = (NJS_BGRA*)gPalette[PVP_HCF];
		AL_DC = (NJS_BGRA*)gPalette[PVP_DCZ];
		v28 = (unsigned short*)a1->Data1.Chao->palette;
		v33 = chaoData->Alignment;
		if (evolution > 1)
		{
			evolution = 1;
		}
		if (powerrun == 0)
		{
			powerrun = 0.000001f;
		}
		if (flyswim == 0)
		{
			flyswim = 0.000001f;
		}
		if (powerrun > 1)
		{
			powerrun = 1;
			goto LABEL_21;
		}
		if (powerrun >= -1)
		{
			if (powerrun >= 0)
			{
				goto LABEL_21;
			}
		}
		else
		{
			powerrun = -1;
		}
		AL_HCP = (NJS_BGRA*)gPalette[PVP_HCR];
		powerrun = -powerrun;
	LABEL_21:
		if (flyswim > 1)
		{
			flyswim = 1;
			goto LABEL_27;
		}
		if (flyswim >= -1)
		{
			if (flyswim >= 0)
			{
				goto LABEL_27;
			}
		}
		else
		{
			flyswim = -1;
		}
		AL_HCF = (NJS_BGRA*)gPalette[PVP_HCS];
		flyswim = -flyswim;
	LABEL_27:
		v36 = 1 - evolution;
		v19 = 0;
		v9 = 1.0f / (powerrun + flyswim);
		if (v33 < 0)
		{
			//dark alignment
			if (v33 < -1)
			{
				v33 = -1;
			}
			v11 = v19;
			AL_HCNa = 48;
			do
			{
				v12 = 0;
				do
				{
					if (v12)
					{
						if (v12 == 1)
						{
							v23 = (unsigned char)(((double)AL_NC01->b * evolution + (double)AL_NC00->b * v36)
								* (1.0 - v33 * -1.0)
								+ (double)AL_DC->b * (v33 * -1.0));
						}
						else if (v12 == 2)
						{
							v19 = (unsigned char)(((double)AL_NC01->b * evolution + (double)AL_NC00->b * v36)
								* (1.0 - v33 * -1.0)
								+ (double)AL_DC->b * (v33 * -1.0));
						}
					}
					else
					{
						v11 = (unsigned char)(((double)AL_NC01->b * evolution + (double)AL_NC00->b * v36)
							* (1.0 - v33 * -1.0)
							+ (double)AL_DC->b * (v33 * -1.0));
					}
					AL_NC00 = (NJS_BGRA*)((char*)AL_NC00 + 1);
					AL_NC01 = (NJS_BGRA*)((char*)AL_NC01 + 1);
					++v12;
					AL_DC = (NJS_BGRA*)((char*)AL_DC + 1);
				} while (v12 < 4);
				*v28 = (v11 >> 3) | 8 * (v23 & 0xFC | 32 * (v19 & 0xF8));
				++v28;
				*v34 = v11 | ((v23 | ((v19 | 0xFFFFFF00) << 8)) << 8);
				++v34;
				--AL_HCNa;
			} while (AL_HCNa);
		}
		else
		{
			//hero alignment code
			if (v33 > 1)
			{
				v33 = 1;
			}
			AL_DCa = 0x30;
			do
			{
				v10 = 0;
				do
				{
					v40 = 1.0f - powerrun;
					v44 = 1.0f - flyswim;
					v24 = powerrun * v9;
					v42 = v9 * flyswim;
					if (v10)
					{
						if (v10 == 1)
						{
							v23 = (unsigned char)(((((double)AL_HCF->b * flyswim + (double)AL_HCN->b * v44) * v42
								+ ((double)AL_HCP->b * powerrun + (double)AL_HCN->b * v40) * v24)
								* evolution
								+ (double)AL_HCZ->b * v36)
								* v33
								+ ((double)AL_NC01->b * evolution + (double)AL_NC00->b * v36) * (1.0 - v33));
						}
						else if (v10 == 2)
						{
							v22 = (unsigned char)(((((double)AL_HCF->b * flyswim + (double)AL_HCN->b * v44) * v42
								+ ((double)AL_HCP->b * powerrun + (double)AL_HCN->b * v40) * v24)
								* evolution
								+ (double)AL_HCZ->b * v36)
								* v33
								+ ((double)AL_NC01->b * evolution + (double)AL_NC00->b * v36) * (1.0 - v33));
						}
					}
					else
					{
						v19 = (unsigned char)(((((double)AL_HCF->b * flyswim + (double)AL_HCN->b * v44) * v42
							+ ((double)AL_HCP->b * powerrun + (double)AL_HCN->b * v40) * v24)
							* evolution
							+ (double)AL_HCZ->b * v36)
							* v33
							+ ((double)AL_NC01->b * evolution + (double)AL_NC00->b * v36) * (1.0 - v33));
					}
					AL_HCP = (NJS_BGRA*)((char*)AL_HCP + 1);
					AL_HCZ = (NJS_BGRA*)((char*)AL_HCZ + 1);
					AL_NC00 = (NJS_BGRA*)((char*)AL_NC00 + 1);
					AL_NC01 = (NJS_BGRA*)((char*)AL_NC01 + 1);
					++v10;
					AL_HCN = (NJS_BGRA*)((char*)AL_HCN + 1);
					AL_HCF = (NJS_BGRA*)((char*)AL_HCF + 1);
				} while (v10 < 4);
				*v28 = (v19 >> 3) | 8 * (v23 & 0xFC | 32 * (v22 & 0xF8));
				++v28;
				*v34 = v19 | ((v23 | ((v22 | 0xFFFFFF00) << 8)) << 8);
				++v34;
				--AL_DCa;
			} while (AL_DCa);
		}

	}
	else 
	{
		v13 = v4;
		AL_DCb = gPalette[v13 + 1];
		Magnitude = chaoData->EvolutionProgress;
		v14 = gPalette[v13];
		v15 = gPalette[v13 + 5];
		v16 = gPalette[v13 + 3];
		v37 = (unsigned short*)a1->Data1.Chao->palette;
		powerruna = chaoData->PowerRun;
		if (Magnitude > 1)
		{
			Magnitude = 1;
		}
		if (powerruna == 0)
		{
			powerruna = 0.000001f;
		}
		if (flyswim == 0)
		{
			flyswim = 0.000001f;
		}
		if (powerruna > 1)
		{
			powerruna = 1;
			goto LABEL_66;
		}
		if (powerruna >= -1)
		{
			if (powerruna >= 0)
			{
				goto LABEL_66;
			}
		}
		else
		{
			powerruna = -1;
		}
		v15 = gPalette[v13 + 4];
		powerruna = -powerruna;
	LABEL_66:
		if (flyswim > 1)
		{
			flyswim = 1;
			goto LABEL_72;
		}
		if (flyswim >= -1)
		{
			if (flyswim >= 0)
			{
				goto LABEL_72;
			}
		}
		else
		{
			flyswim = -1;
		}
		v16 = gPalette[v13 + 2];
		flyswim = -flyswim;
	LABEL_72:
		AL_HCNb = 48;
		v17 = 1.0f / (flyswim + powerruna);
		do
		{
			v18 = 0;
			do
			{
				v41 = v17 * flyswim;
				v43 = powerruna * v17;
				if (v18)
				{
					if (v18 == 1)
					{
						v19 = (unsigned char)((((double)(unsigned __int8)*v16 * flyswim
							+ (double)(unsigned __int8)*AL_DCb * (1.0 - flyswim))
							* v41
							+ ((double)(unsigned __int8)*v15 * powerruna
								+ (double)(unsigned __int8)*AL_DCb * (1.0 - powerruna))
							* v43)
							* Magnitude
							+ (double)(unsigned __int8)*v14 * (1.0 - Magnitude));
					}
					else if (v18 == 2)
					{
						v22 = (unsigned char)((((double)(unsigned __int8)*v16 * flyswim
							+ (double)(unsigned __int8)*AL_DCb * (1.0 - flyswim))
							* v41
							+ ((double)(unsigned __int8)*v15 * powerruna
								+ (double)(unsigned __int8)*AL_DCb * (1.0 - powerruna))
							* v43)
							* Magnitude
							+ (double)(unsigned __int8)*v14 * (1.0 - Magnitude));
					}
				}
				else
				{
					v23 = (unsigned char)((((double)(unsigned __int8)*v16 * flyswim
						+ (double)(unsigned __int8)*AL_DCb * (1.0 - flyswim))
						* v41
						+ ((double)(unsigned __int8)*v15 * powerruna
							+ (double)(unsigned __int8)*AL_DCb * (1.0 - powerruna))
						* v43)
						* Magnitude
						+ (double)(unsigned __int8)*v14 * (1.0 - Magnitude));
				}
				++v14;
				++v15;
				++v16;
				++v18;
				++AL_DCb;
			} while (v18 < 4);
			*v37 = (v23 >> 3) | 8 * (v19 & 0xFC | 32 * (v22 & 0xF8));
			++v37;
			*v34 = v23 | ((v19 | ((v22 | 0xFFFFFF00) << 8)) << 8);
			++v34;
			--AL_HCNb;
		} while (AL_HCNb);
	}
	v85 = (int)a1->Data1.Chao->palette;
	v86 = 0;
	do
	{
		*(int*)0x01DBED80 = v85;
		*(int*)0x1DBED84 = 1;
		*(int*)0x01DBED88 = 16;
		if (*(int*)0x1A55790)
		{
			sub_41AB40(cno + v86 + 2 * cno);
		}
		++v86;
		v85 += 32;
	} while (v86 < 3);
}
void __cdecl AL_PaletteSetColorRatio8bpp(ChaoDataBase* chaoData, ObjectMaster* a1, int cno, char gPalette[][256 * 3 * 4])
{
	ChaoType v3; // dl
	signed int v4; // ecx
	float flyswim; // st7
	NJS_BGRA* AL_NC00; // edi
	NJS_BGRA* AL_NC01; // ebp
	NJS_BGRA* AL_HCP; // ebx
	float v9; // st6
	signed int v10; // esi
	unsigned __int8 v11; // bl
	signed int v12; // esi
	unsigned __int8 v19; // [esp+7h] [ebp-101h]
	float powerrun; // [esp+8h] [ebp-100h]
	unsigned __int8 v22; // [esp+Eh] [ebp-FAh]
	unsigned __int8 v23; // [esp+Fh] [ebp-F9h]
	float v24; // [esp+10h] [ebp-F8h]
	NJS_BGRA* AL_DC; // [esp+18h] [ebp-F0h]
	int AL_DCa; // [esp+18h] [ebp-F0h]
	unsigned __int16* v28; // [esp+1Ch] [ebp-ECh]
	NJS_BGRA* AL_HCN; // [esp+20h] [ebp-E8h]
	int AL_HCNa; // [esp+20h] [ebp-E8h]
	float v33; // [esp+24h] [ebp-E4h]
	int* v34; // [esp+28h] [ebp-E0h]
	float evolution; // [esp+2Ch] [ebp-DCh]
	float v36; // [esp+30h] [ebp-D8h]
	NJS_BGRA* AL_HCF; // [esp+34h] [ebp-D4h]
	NJS_BGRA* AL_HCZ; // [esp+38h] [ebp-D0h]
	float v40; // [esp+3Ch] [ebp-CCh]
	float v42; // [esp+40h] [ebp-C8h]
	float v44; // [esp+44h] [ebp-C4h]
	char v45[192]; // [esp+48h] [ebp-C0h]
	int v85;
	int v86;

	v34 = (int*)v45;
	v3 = (ChaoType)chaoData->Type;
	if (AL_IsHero(v3) && chaoData->LobbyTextureValue > 0)
		v3 = (ChaoType)((chaoData->LobbyTextureValue + 1) * 3);
	//lobbytextureval = 1 -> v3 = 6
	//lobbytextureval = 2 -> v3 = 9
	//formula ^ for chaotype override

	switch ((unsigned __int8)v3)
	{
	case ChaoType_Child:
		v4 = PVP_NCZ;
		break;
	case ChaoType_Hero_Normal:
		v4 = PVP_HNZ;
		break;
	case ChaoType_Hero_Swim:
		v4 = PVP_HSZ;
		break;
	case ChaoType_Hero_Fly:
		v4 = PVP_HFZ;
		break;
	case ChaoType_Hero_Run:
		v4 = PVP_HRZ;
		break;
	case ChaoType_Hero_Power:
		v4 = PVP_HPZ;
		break;
	default:
		return;
	}

	flyswim = chaoData->FlySwim;
	if (v3 == ChaoType_Child)
	{
		evolution = chaoData->EvolutionProgress;
		powerrun = chaoData->PowerRun;
		AL_NC00 = (NJS_BGRA*)gPalette[PVP_NCZ];
		AL_NC01 = (NJS_BGRA*)gPalette[PVP_NCN];
		AL_HCZ = (NJS_BGRA*)gPalette[PVP_HCZ];
		AL_HCN = (NJS_BGRA*)gPalette[PVP_HCN];
		AL_HCP = (NJS_BGRA*)gPalette[PVP_HCP];
		AL_HCF = (NJS_BGRA*)gPalette[PVP_HCF];
		AL_DC = (NJS_BGRA*)gPalette[PVP_DCZ];
		v28 = (unsigned short*)a1->Data1.Chao->palette;
		v33 = chaoData->Alignment;
		if (evolution > 1.0)
		{
			evolution = 1.0;
		}
		if (powerrun == 0.0)
		{
			powerrun = 0.000001f;
		}
		if (flyswim == 0)
		{
			flyswim = 0.000001f;
		}
		if (powerrun > 1.0)
		{
			powerrun = 1.0;
			goto LABEL_21;
		}
		if (powerrun >= -1.0)
		{
			if (powerrun >= 0.0)
			{
				goto LABEL_21;
			}
		}
		else
		{
			powerrun = -1.0;
		}
		AL_HCP = (NJS_BGRA*)gPalette[PVP_HCR];
		powerrun = -powerrun;
	LABEL_21:
		if (flyswim > 1.0)
		{
			flyswim = 1.0;
			goto LABEL_27;
		}
		if (flyswim >= -1.0)
		{
			if (flyswim >= 0.0)
			{
				goto LABEL_27;
			}
		}
		else
		{
			flyswim = -1.0;
		}
		AL_HCF = (NJS_BGRA*)gPalette[PVP_HCS];
		flyswim = -flyswim;
	LABEL_27:
		v36 = 1 - evolution;
		v19 = 0;
		v9 = 1.0f / (powerrun + flyswim);
		if (v33 < 0)
		{
			//dark alignment
			if (v33 < -1)
			{
				v33 = -1;
			}
			v11 = v19;
			AL_HCNa = 256 * 3;
			do
			{
				v12 = 0;
				do
				{
					if (v12)
					{
						if (v12 == 1)
						{
							v23 = (unsigned char)(((double)AL_NC01->b * evolution + (double)AL_NC00->b * v36)
								* (1.0 - v33 * -1.0)
								+ (double)AL_DC->b * (v33 * -1.0));
						}
						else if (v12 == 2)
						{
							v19 = (unsigned char)(((double)AL_NC01->b * evolution + (double)AL_NC00->b * v36)
								* (1.0 - v33 * -1.0)
								+ (double)AL_DC->b * (v33 * -1.0));
						}
					}
					else
					{
						v11 = (unsigned char)(((double)AL_NC01->b * evolution + (double)AL_NC00->b * v36)
							* (1.0 - v33 * -1.0)
							+ (double)AL_DC->b * (v33 * -1.0));
					}
					AL_NC00 = (NJS_BGRA*)((char*)AL_NC00 + 1);
					AL_NC01 = (NJS_BGRA*)((char*)AL_NC01 + 1);
					++v12;
					AL_DC = (NJS_BGRA*)((char*)AL_DC + 1);
				} while (v12 < 4);
				*v28 = (v11 >> 3) | 8 * (v23 & 0xFC | 32 * (v19 & 0xF8));
				++v28;
				*v34 = v11 | ((v23 | ((v19 | 0xFFFFFF00) << 8)) << 8);
				++v34;
				--AL_HCNa;
			} while (AL_HCNa);
		}
		else
		{
			//hero alignment code
			if (v33 > 1.0)
			{
				v33 = 1.0;
			}
			AL_DCa = 256 * 3;
			do
			{
				v10 = 0;
				do
				{
					v40 = 1 - powerrun;
					v44 = 1 - flyswim;
					v24 = powerrun * v9;
					v42 = v9 * flyswim;
					if (v10)
					{
						if (v10 == 1)
						{
							v23 = (unsigned char)(((((double)AL_HCF->b * flyswim + (double)AL_HCN->b * v44) * v42
								+ ((double)AL_HCP->b * powerrun + (double)AL_HCN->b * v40) * v24)
								* evolution
								+ (double)AL_HCZ->b * v36)
								* v33
								+ ((double)AL_NC01->b * evolution + (double)AL_NC00->b * v36) * (1.0 - v33));
						}
						else if (v10 == 2)
						{
							v22 = (unsigned char)(((((double)AL_HCF->b * flyswim + (double)AL_HCN->b * v44) * v42
								+ ((double)AL_HCP->b * powerrun + (double)AL_HCN->b * v40) * v24)
								* evolution
								+ (double)AL_HCZ->b * v36)
								* v33
								+ ((double)AL_NC01->b * evolution + (double)AL_NC00->b * v36) * (1.0 - v33));
						}
					}
					else
					{
						v19 = (unsigned char)(((((double)AL_HCF->b * flyswim + (double)AL_HCN->b * v44) * v42
							+ ((double)AL_HCP->b * powerrun + (double)AL_HCN->b * v40) * v24)
							* evolution
							+ (double)AL_HCZ->b * v36)
							* v33
							+ ((double)AL_NC01->b * evolution + (double)AL_NC00->b * v36) * (1.0 - v33));
					}
					AL_HCP = (NJS_BGRA*)((char*)AL_HCP + 1);
					AL_HCZ = (NJS_BGRA*)((char*)AL_HCZ + 1);
					AL_NC00 = (NJS_BGRA*)((char*)AL_NC00 + 1);
					AL_NC01 = (NJS_BGRA*)((char*)AL_NC01 + 1);
					++v10;
					AL_HCN = (NJS_BGRA*)((char*)AL_HCN + 1);
					AL_HCF = (NJS_BGRA*)((char*)AL_HCF + 1);
				} while (v10 < 4);
				*v28 = (v19 >> 3) | 8 * (v23 & 0xFC | 32 * (v22 & 0xF8));
				++v28;
				*v34 = v19 | ((v23 | ((v22 | 0xFFFFFF00) << 8)) << 8);
				++v34;
				--AL_DCa;
			} while (AL_DCa);
		}

	}

	v85 = (int)a1->Data1.Chao->palette;
	v86 = 0;
	do
	{
		*(int*)0x01DBED80 = v85;
		*(int*)0x1DBED84 = 1;
		*(int*)0x01DBED88 = 256;
		if (*(int*)0x1A55790)
		{
			sub_41AB40(cno + v86 + 2 * cno);
		}
		++v86;
		v85 += 256 * 2;
	} while (v86 < 3);
}
void __cdecl UpdateChaoPaletteNew(ChaoDataBase* chaoData, ObjectMaster* a1, int cno)
{
	if (AL_IsNegative(a1))
	{
		AL_PaletteSetColorRatio(chaoData, a1, cno, gPaletteNegative);
	}
	else
	{
		AL_PaletteSetColorRatio(chaoData, a1, cno, gPaletteRegular);
		//AL_PaletteSetColorRatio8bpp(chaoData, a1, cno, index8Buff);
		
	}
}
static void __declspec(naked) UpdateChaoPaletteHook()
{
	__asm
	{
		push[esp + 04h] // a3
		push ecx // a2
		push eax // a1

		// Call your __cdecl function here:
		call UpdateChaoPaletteNew

		pop eax // a1
		pop ecx // a2
		add esp, 4 // a3
		retn
	}
}
void AL_Palette_Init()
{
	//explanation above function
	WriteCall((void*)0x00540C81, alpalSetBankDrawChaoHook);

	//also afaik we are in control of all the calls now so this might be redundant but im keeping it for external mods ig
	WriteJump((void*)0x534E10, alpalSetBankHook);

	//argb8888 fix
	WriteData((int*)0x05345A2, (int)48);
	WriteData((int*)0x05345C2, (int)48);

	WriteData((int*)0x00534381, (int)gPaletteFilename);
	WriteData((int*)0x00534603, (int)&gPaletteFilename[39]);
	WriteData((int*)0x0053437C, (int)gPaletteNegative);
	WriteCall((void*)0x0054FFBE, UpdateChaoPaletteHook);
	WriteCall((void*)0x005A4D18, UpdateChaoPaletteHook);
	WriteCall((void*)0x005A4D56, UpdateChaoPaletteHook);
	WriteCall((void*)0x052ACA0, LoadPaletteHook);
}