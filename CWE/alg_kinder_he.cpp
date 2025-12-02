#include "stdafx.h"

#include "ninja_functions.h"
#include "ChaoMain.h"
#include "al_minimal.h"
#include "alo_fruit.h"
#include "alg_kinder_he.h"
#include "alg_kinder_bl.h"
#include "al_sandhole.h"
#include "alg_kinder_ortho.h"
#include "al_behavior/al_knowledge.h"
#include "Chao.h"
#include "al_odekake.h"
#include "alo_obakehead.h"
#include "al_growtree.h"
#include "al_modelcontainer.h"
#include <api/api_tree.h>
#include <al_garden_info.h>

DataPointer(ChaoData*, dword_19F6454, 0x19F6454);
DataPointer(float, flt_B18F54, 0xB18F54);
FunctionPointer(void, sub_782780, (int a1, int a2, float a3), 0x782780);
DataArray(NJS_OBJECT*, dword_171A240, 0x171A240, 4);
DataArray(NJS_OBJECT*, off_12E537C, 0x12E537C, 7);

const int sub_42D690Ptr = 0x42D690;
void sub_42D690(int a1)
{
	__asm
	{
		mov eax, a1
		call sub_42D690Ptr
	}
}

void __cdecl sub_58F980(ObjectMaster* a1)
{
	signed int v1; // ebp
	float a3; // ST20_4
	Data2Ptr v9; // edx

	v9.Undefined = (void*)a1->Data2.Undefined;
	if (*((char*)v9.Undefined + 40))
	{
		v1 = AL_GetHoldingItemKind();

		njPushMatrixEx();
		a3 = *((float*)v9.Undefined + 11) + 3.284631013870239f;
		njTranslate(NULL, 0.34388199f, a3, -2.164542f);
		switch (AL_GetHoldingItemCategory())
		{
		case 2:
			if (v1 >= 21 && v1 <= 24)
			{
				njScale(NULL, 1.2f, 1.2f, 1.2f);
				njTranslate(NULL, 0.0f, flt_B18F54, 0.0f);
				njSetTexture(&CHAOSDRIVE_TEXLIST);
				njCnkDrawObject((NJS_OBJECT*)dword_171A240[v1]);
			}
			else
			{
				njSetTexture(ModAPI_MinimalTexlists[v1]);
				char* v5 = (char*)v9.Undefined;
				sub_782780((int)ModAPI_MinimalModels[v1], (int)ModAPI_MinimalMotion0[v1], *(float*)(v5 + 52));
			}
			break;
		case 3:
			njTranslate(NULL, 0.0f, 1.3f, 0.0f);
			ObjectRegistry::DrawObject<njCnkDrawObject>(ChaoItemCategory_Fruit, v1);
			break;
		case 7:
			njSetTexture(ModAPI_SeedTexlists[v1]);
			njScale(NULL, 1.5f, 1.5f, 1.5f);
			sub_42D690((int)ModAPI_SeedModels[v1]->model);
			break;
		case 9:
			switch (v1)
			{
			case 4:
			case 6:
			case 7:
			case 8:
			case 9:
				njTranslate(NULL, 0, 2.8f, 0);
				njRotateX(NULL, 0x8000);
				break;
			default:
				break;
			}
			ALO_ObakeHeadDraw(v1);
			break;
		default:
			break;
		}
		njPopMatrixEx();
	}
}


__int16 word_8A2754[] = { 3464, 2824, 3464, 3464, 2824, 3464, 2184, 3464, 1544, 3464, 1544, 2824, 2200, 2824, 2200, 2824 };

//                             E    D       C    B      A        S         X 
int GradePurchasePrice[] = { 10000,20000,30000,40000, 9999999,9999999 ,9999999 };
ChaoHudThing plusHUD = { 0x240,0x255,0,0, (short)((39 / 76.0f) * 4096) ,4096,0, 4096 };

int UpgradeButtons[] = { Buttons_Up, Buttons_Down, Buttons_Right, Buttons_Left, Buttons_L};

const int SE_Call_TIMERPtr = 0x04374D0;
void SE_Call_TIMER(int a1, int a2, char a3, char a4, __int16 a5)
{
	__asm
	{
		push dword ptr[a5]
		push dword ptr[a4]
		push dword ptr[a3]
		push a2
		mov edi, a1
		call SE_Call_TIMERPtr
		add esp, 4 * 4
	}
}
/*
			write32 0058EA9A 0x80909090
			write32 0058EA65 0x90909090
			write32 0058EAA3 0x90909090
	
	ifeq16 01A529EC 0
			write32 0058EA9A 0x801547FE
			write32 0058EA65 0xFF154780
			write32 0058EAA3 0x001547C6
*/
const int DrawMedicalChartTextPtr = 0x0058D880;
void DrawMedicalChartText(const char* TextPtr, float XPos, float YPos, float TextBoxLength, float TextSize, char TextAlignment)
{
	__asm
	{
		mov ecx, TextPtr
		push dword ptr[TextAlignment]
		push TextSize
		push TextBoxLength
		push YPos
		push XPos
		call DrawMedicalChartTextPtr
		add esp, 20
	}
}
bool CanStatBeUpgraded(ChaoData1* data1, int stat)
{
	if (data1->pParamGC->UpgradeCounter < 3 &&
		data1->pParamGC->StatLevels[stat] == 99 &&
		data1->pParamGC->StatGrades[stat] < ChaoGrade_A &&
		TotalRings >= GradePurchasePrice[data1->pParamGC->StatGrades[stat]])
	{
		return true;
	}

	return false;
}
bool CanChaoUpgrade(ChaoData1* data1)
{
	for (int i = 0; i < 5; i++)
	{
		if (CanStatBeUpgraded(data1,i))
		{
			return true;
		}
	}
	return false;
}
void PurchaseGradesCode(ChaoData1* data1)
{
	char buff[256];
	
	if (data1->pParamGC->UpgradeCounter < 3 )
	{
		if (CanChaoUpgrade(data1)) 
		{
			if (MenuButtons_Held[0] & Buttons_Y)
				sprintf(buff, "Upgrades Left: %d", 3 - data1->pParamGC->UpgradeCounter);
			else
				sprintf(buff, "Hold Y to Upgrade");
		}
		else 
			sprintf(buff, "Upgrades Left: %d", 3 - data1->pParamGC->UpgradeCounter);
	}
	else 
		sprintf(buff, "No Upgrades Left");

	DrawMedicalChartText(
		buff,
		350,
		251.0,
		999.0,
		13.0f,
		5);
	sub_583C60();
	njSetTexture(&CWE_UI_TEXLIST);
	njSetTextureNum(25, 0, 0, 0);
	if (MenuButtons_Held[0] & Buttons_Y)
	{
		for (int i = 0; i < 5; i++)
		{
			if (CanStatBeUpgraded(data1, i))
			{
				if (MenuButtons_Pressed[0] & UpgradeButtons[i])
				{
					data1->pParamGC->StatLevels[i] = 1;
					data1->pParamGC->StatPoints[i] /= 100;
					data1->pParamGC->UpgradeCounter++;
					SE_Call_TIMER(0x8004, 0x616C7277, 1, 0, 120);
					SE_Call_TIMER(0x1005, 0x616C7277, 1, 0, 120);

					TotalRings -= GradePurchasePrice[data1->pParamGC->StatGrades[i]];
					data1->pParamGC->StatGrades[i]++;
				}
				plusHUD.y0 = 32 * i + 262;
				plusHUD.y1 = 32 * i + 282;
				DrawChaoHudThing(&plusHUD, -1);
			}

		}
	}
	sub_583C60();
	njSetTexture((NJS_TEXLIST*)0x011D2ACC);
	njSetTextureNum(7, 0, 0, 0);
}
__declspec(naked)void PurchaseGradesHook()
{
	__asm
	{
		mov ecx, [ebp + 8]
		//mov ecx, [ecx + 34h]
		push[ecx + 34h]
		call PurchaseGradesCode
		add esp, 4
		retn
	}

}

const int DisplayHealthCenterMedicalChartPtr = 0x0058DB70;
void DisplayHealthCenterMedicalChart(int a1, HealthCenter* TextLocation)
{
	__asm
	{
		push TextLocation
		mov esi, a1
		call DisplayHealthCenterMedicalChartPtr
		add esp, 4
	}
}

int isInDNAMenu = 0;
ChaoHudThingOld HealthCenter_MenuPink_[44] =
{
  { {264, 272 }, { 40, 56 }, { 16, 240 }, { 16, 496 } },
  { {272, 352 }, { 40, 56 }, { 272, 496 }, { 16, 496 } },
  { {352, 360 }, { 40, 56 }, { 528, 752 }, { 16, 496 } },
  { {291, 323 }, { 40, 56 }, { 2320, 3312 }, { 2576, 3056 } },
  { {264, 272 }, { 56, 64 }, { 784, 1008 }, { 400, 496 } },
  { {360, 368 }, { 40, 56 }, { 1552, 1776 }, { 16, 496 } },
  { {368, 448 }, { 40, 56 }, { 1808, 2032 }, { 16, 496 } },
  { {448, 456 }, { 40, 56 }, { 2064, 2288 }, { 16, 496 } },
  { {456, 464 }, { 40, 56 }, { 16, 240 }, { 2064, 2544 } },
  { {464, 544 }, { 40, 56 }, { 272, 496 }, { 2064, 2544 } },
  { {544, 552 }, { 40, 56 }, { 528, 752 }, { 2064, 2544 } },
  { {498, 530 }, { 40, 56 }, { 2320, 3312 }, { 2064, 2544 } },
  { {264, 272 }, { 56, 72 }, { 784, 1008 }, { 2064, 2544 } },
  { {272, 456 }, { 56, 72 }, { 1040, 1264 }, { 2064, 2544 } },
  { {456, 552 }, { 56, 72 }, { 1040, 1264 }, { 2448, 2544 } },
  { {552, 616 }, { 56, 72 }, { 1040, 1264 }, { 2064, 2544 } },
  { {616, 624 }, { 56, 72 }, { 1296, 1520 }, { 2064, 2544 } },
  { {264, 272 }, { 72, 480 }, { 784, 1008 }, { 2448, 2544 } },
  { {272, 616 }, { 72, 480 }, { 1040, 1264 }, { 2448, 2544 } },
  { {616, 624 }, { 72, 480 }, { 1296, 1520 }, { 2448, 2544 } },
  { {280, 288 }, { 72, 96 }, { 3344, 3568 }, { 1936, 2672 } },
  { {288, 600 }, { 72, 96 }, { 3600, 3824 }, { 1936, 2672 } },
  { {600, 608 }, { 72, 96 }, { 3856, 4080 }, { 1936, 2672 } },
  { {280, 288 }, { 108, 128 }, { 16, 240 }, { 2576, 3184 } },
  { {288, 600 }, { 108, 128 }, { 272, 496 }, { 2576, 3184 } },
  { {600, 608 }, { 108, 128 }, { 528, 752 }, { 2576, 3184 } },
  { {280, 288 }, { 128, 254 }, { 16, 240 }, { 3344, 3568 } },
  { {288, 600 }, { 128, 254 }, { 272, 496 }, { 3344, 3568 } },
  { {600, 608 }, { 128, 254 }, { 528, 752 }, { 3344, 3568 } },
  { {280, 288 }, { 254, 262 }, { 16, 240 }, { 3856, 4080 } },
  { {288, 600 }, { 254, 262 }, { 272, 496 }, { 3856, 4080 } },
  { {600, 608 }, { 254, 262 }, { 528, 752 }, { 3856, 4080 } },
  { {280, 288 }, { 274, 294 }, { 16, 240 }, { 2576, 3184 } },
  { {288, 600 }, { 274, 294 }, { 272, 496 }, { 2576, 3184 } },
  { {600, 608 }, { 274, 294 }, { 528, 752 }, { 2576, 3184 } },
  { {280, 288 }, { 294, 368 }, { 16, 240 }, { 3344, 3568 } },
  { {288, 600 }, { 294, 368 }, { 272, 496 }, { 3344, 3568 } },
  { {600, 608 }, { 294, 368 }, { 528, 752 }, { 3344, 3568 } },
  { {280, 288 }, { 368, 388 }, { 784, 1008 }, { 2576, 3184 } },
  { {288, 600 }, { 368, 388 }, { 1040, 1264 }, { 2576, 3184 } },
  { {600, 608 }, { 368, 388 }, { 1296, 1520 }, { 2576, 3184 } },
  { {280, 288 }, { 388, 416 }, { 16, 240 }, { 3216, 4080 } },
  { {288, 600 }, { 388, 416 }, { 272, 496 }, { 3216, 4080 } },
  { {600, 608 }, { 388, 416 }, { 528, 752 }, { 3216, 4080 } }
};
#define OFFSET_ALL 40
ChaoHudThingOld HealthCenter_DNAMenu[] =
{
  { { 264, 272 }, { 40, 56 }, { 16, 240 }, { 16, 496 } },
  { { 272, 352 }, { 40, 56 }, { 272, 496 }, { 16, 496 } },
  { { 352, 360 }, { 40, 56 }, { 528, 752 }, { 16, 496 } },
  { { 360, 368 }, { 40, 56 }, { 1552, 1776 }, { 16, 496 } },
  { { 368, 448 }, { 40, 56 }, { 1808, 2032 }, { 16, 496 } },
  { { 448, 456 }, { 40, 56 }, { 2064, 2288 }, { 16, 496 } },
  { { 456, 464 }, { 40, 56 }, { 16, 240 }, { 2064, 2544 } },
  { { 464, 544 }, { 40, 56 }, { 272, 496 }, { 2064, 2544 } },
  { { 544, 552 }, { 40, 56 }, { 528, 752 }, { 2064, 2544 } },

  { { 264, 272 }, { 56, 480 }, { 784, 1008 }, { 400, 496 } },
  { { 272, 360 }, { 56, 72 }, { 1040, 1264 }, { 400, 496 } },
  { { 360, 616 }, { 56, 72 }, { 1040, 1264 }, { 16, 496 } },
  { { 616, 624 }, { 56, 72 }, { 1296, 1520 }, { 16, 496 } },
  { { 272, 616 }, { 72, 480 }, { 1040, 1264 }, { 400, 496 } },
  { { 616, 624 }, { 72, 480 }, { 1296, 1520 }, { 400, 496 } },

  //maybe the top genetics text
  { { 280, 288 }, { 72, 96 }, { 3344, 3568 }, { 1936 - 736, 2672 - 736 } },
  { { 288, 600 }, { 72, 96 }, { 3600, 3824 }, { 1936 - 736, 2672 - 736} },
  { { 600, 608 }, { 72, 96 }, { 3856, 4080 }, { 1936 - 736, 2672 - 736} },

};
ChaoHudThingOld HealthCenter_DNAMenu2[] =
{


  { { 280, 288 }, { 250 + OFFSET_ALL, 270 + OFFSET_ALL }, { 784 - 784, 1008 - 784 }, { 528, 1136 } }, //"Grandparents" display part
  { { 288, 600 - 8 }, { 250 + OFFSET_ALL, 270 + OFFSET_ALL }, { 1040 - 784, 1264 - 784 }, { 528, 1136 } },
  { { 600 - 8, 600 }, { 250 + OFFSET_ALL, 270 + OFFSET_ALL }, { 1296 - 784, 1520 - 784 }, { 528, 1136 } },
  { { 280, 288 }, { 270 + OFFSET_ALL, 350 + OFFSET_ALL }, { 16, 240 }, { 1168, 2032 } }, //grandparents table
  { { 288, 600 - 8 }, { 270 + OFFSET_ALL, 350 + OFFSET_ALL }, { 272, 496 }, { 1168, 2032 } },
  { { 600 - 8, 600 }, { 270 + OFFSET_ALL, 350 + OFFSET_ALL }, { 528, 752 }, { 1168, 2032 } },


  //name display left
  { { 280, 288 }, { 72 + OFFSET_ALL, 108 + OFFSET_ALL }, { 3344, 3568 }, { 16, 1136 } }, //left part
  { { 288, 592 - 200 }, { 72 + OFFSET_ALL, 108 + OFFSET_ALL }, { 3600, 3824 }, { 16, 1136 } }, //middle part
  { { 592 - 200, 600 - 200  }, { 72 + OFFSET_ALL, 108 + OFFSET_ALL }, { 3856, 4080 }, { 16, 1136 } }, //right part

  //name display right
   { { 280 + 200, 288 + 200}, { 72 + OFFSET_ALL, 108 + OFFSET_ALL }, { 3344, 3568 }, { 16, 1136 } }, //left part
  { { 288 + 200, 592 }, { 72 + OFFSET_ALL, 108 + OFFSET_ALL }, { 3600, 3824 }, { 16, 1136 } }, //middle part
  { { 592, 600  }, { 72 + OFFSET_ALL, 108 + OFFSET_ALL }, { 3856, 4080 }, { 16, 1136 } }, //right part

  { { 496 - 210,   600 - 210 }, { 124 + OFFSET_ALL - 6, 232 + OFFSET_ALL - 6}, { 3088, 3312 }, { 16, 240 } }, //left chao display
  { { 496, 600 }, { 124 + OFFSET_ALL - 6, 232 + OFFSET_ALL - 6 }, { 3088, 3312 }, { 16, 240 } }, //right chao display
  /*
  { { 280, 288 }, { 124, 144 }, { 16, 240 }, { 528, 1136 } },
  { { 288, 480 }, { 124, 144 }, { 272, 496 }, { 528, 1136 } },
  { { 480, 488 }, { 124, 144 }, { 528, 752 }, { 528, 1136 } },
  { { 280, 288 }, { 144, 170 }, { 16, 240 }, { 1296, 1520 } },
  { { 288, 480 }, { 144, 170 }, { 272, 496 }, { 1296, 1520 } },
  { { 480, 488 }, { 144, 170 }, { 528, 752 }, { 1296, 1520 } },
  { { 280, 288 }, { 170, 190 }, { 784, 1008 }, { 528, 1136 } },
  { { 288, 480 }, { 170, 190 }, { 1040, 1264 }, { 528, 1136 } },
  { { 480, 488 }, { 170, 190 }, { 1296, 1520 }, { 528, 1136 } },
  { { 280, 288 }, { 190, 218 }, { 16, 240 }, { 1168, 2032 } },
  { { 288, 480 }, { 190, 218 }, { 272, 496 }, { 1168, 2032 } },
  { { 480, 488 }, { 190, 218 }, { 528, 752 }, { 1168, 2032 } },
  { { 280, 288 }, { 240, 248 }, { 3344, 3568 }, { 1168, 1392 } }, // top left corner of thing
  { { 288, 600 }, { 240, 248 }, { 3600, 3824 }, { 1168, 1392 } }, // top middle part
  { { 600, 608 }, { 240, 248 }, { 3856, 4080 }, { 1168, 1392 } }, //top right ocrner
  { { 280, 288 }, { 248, 416 }, { 3344, 3568 }, { 1424, 1648 } },
  { { 288, 600 }, { 248, 416 }, { 3600, 3824 }, { 1424, 1648 } },
  { { 600, 608 }, { 248, 416 }, { 3856, 4080 }, { 1424, 1648 } },
  { { 280, 288 }, { 416, 424 }, { 3344, 3568 }, { 1680, 1904 } },
  { { 288, 600 }, { 416, 424 }, { 3600, 3824 }, { 1680, 1904 } },
  { { 600, 608 }, { 416, 424 }, { 3856, 4080 }, { 1680, 1904 } },
  { { 296, 580 }, { 260, 284 }, { 1680, 1776 }, { 2448, 2544 } },
  { { 580, 596 }, { 260, 284 }, { 1552, 2032 }, { 2064, 2800 } },
  { { 296, 580 }, { 292, 316 }, { 1680, 1776 }, { 2448, 2544 } },
  { { 580, 596 }, { 292, 316 }, { 1552, 2032 }, { 2064, 2800 } },
  { { 296, 580 }, { 324, 348 }, { 1680, 1776 }, { 2448, 2544 } },
  { { 580, 596 }, { 324, 348 }, { 1552, 2032 }, { 2064, 2800 } },
  { { 296, 580 }, { 356, 380 }, { 1680, 1776 }, { 2448, 2544 } },
  { { 580, 596 }, { 356, 380 }, { 1552, 2032 }, { 2064, 2800 } },
  { { 296, 580 }, { 388, 412 }, { 1680, 1776 }, { 2448, 2544 } },
  { { 580, 596 }, { 388, 412 }, { 1552, 2032 }, { 2064, 2800 } },

  */
};
ChaoHudThingOld HealthCenter_FriendMenu[] = {
	//top 3 little tabs
  { { 264, 272 }, { 40, 56 }, { 16, 240 }, { 16, 496 } },
  { { 272, 352 }, { 40, 56 }, { 272, 496 }, { 16, 496 } },
  { { 352, 360 }, { 40, 56 }, { 528, 752 }, { 16, 496 } },
  { { 360, 368 }, { 40, 56 }, { 1552, 1776 }, { 16, 496 } },
  { { 368, 448 }, { 40, 56 }, { 1808, 2032 }, { 16, 496 } },
  { { 448, 456 }, { 40, 56 }, { 2064, 2288 }, { 16, 496 } },
  { { 456, 464 }, { 40, 56 }, { 16, 240 }, { 2064, 2544 } },
  { { 464, 544 }, { 40, 56 }, { 272, 496 }, { 2064, 2544 } },
  { { 544, 552 }, { 40, 56 }, { 528, 752 }, { 2064, 2544 } },

  //base panel
  { { 264, 272 }, { 56, 480 }, { 784, 1008 }, { 400, 496 } },
  { { 272, 360 }, { 56, 72 }, { 1040, 1264 }, { 400, 496 } },
  { { 360, 616 }, { 56, 72 }, { 1040, 1264 }, { 16, 496 } },
  { { 616, 624 }, { 56, 72 }, { 1296, 1520 }, { 16, 496 } },
  { { 272, 616 }, { 72, 480 }, { 1040, 1264 }, { 400, 496 } },
  { { 616, 624 }, { 72, 480 }, { 1296, 1520 }, { 400, 496 } },

  //friend display middle
  { { 391, 391 + 104 }, { 124, 232}, { 3088, 3312 }, { 16, 240 } },
};




FastcallFunctionPointer(void, sub_57A6F0, (char* a1, __int16* a2), 0x57A6F0);



__int16 wcbuffer[8] = { 0,0,0,0,0,0,0,0 };
void ConvertName(char* original_, char* output)
{
	char buffer[8];
	unsigned char* original = (unsigned char*)original_;
	for (size_t i = 0; i < strlen(original_); i++)
	{
		if (original[i] >= 0x10 && original[i] <= 0x19)
			output[i] = 0x30 + (original[i] - 0x10);
		else if (original[i] >= 0x21 && original[i] <= 0x3A)
			output[i] = 0x41 + (original[i] - 0x21);
		else if (original[i] >= 0x41 && original[i] <= 0x5A)
			output[i] = 0x61 + (original[i] - 0x41);
		else// if (original[i] == 0x5F)
			output[i] = ' ';
	}
	output[strlen(original_)] = 0;
	return;

	*(int*)(&buffer[0]) = *(int*)(original);
	*(short*)(&buffer[4]) = *(short*)(&original[4]);
	buffer[6] = original[6];
	buffer[7] = 0;

	sub_57A6F0(buffer, wcbuffer);
	WcConvFromCStr((int)output, (int)wcbuffer, TextLanguage == 0);
}

ObjectMaster* pMother = 0;
ObjectMaster* pFather = 0;
ObjectFuncPtr pDisplaySub = 0;
ObjectFuncPtr pField20Sub = 0;
bool noMother;
AL_NAME motherName;
bool noFather;
AL_NAME fatherName;
AL_NAME mgmName;
AL_NAME mgfName;
AL_NAME fgmName;
AL_NAME fgfName;

void HealthCenterDNAMenu(HealthCenter* a1)
{
	sub_583C60();
	njSetTexture((NJS_TEXLIST*)0x011D2ACC);
	njSetTextureNum(7, 0, 0, 0);
	for (size_t i = 0; i < LengthOfArray(HealthCenter_DNAMenu); i++)
		DrawChaoHudThing((ChaoHudThing*)&HealthCenter_DNAMenu[i], -1);

	if (!noMother && !noFather) {

		ConvertName(a1->medicalChartChao_->Data1.Chao->pParamGC->MotherName, motherName);
		ConvertName(a1->medicalChartChao_->Data1.Chao->pParamGC->MGroundMotherName, mgmName);
		ConvertName(a1->medicalChartChao_->Data1.Chao->pParamGC->MGroundFatherName, mgfName);
		ConvertName(a1->medicalChartChao_->Data1.Chao->pParamGC->FatherName, fatherName);
		ConvertName(a1->medicalChartChao_->Data1.Chao->pParamGC->FGroundMotherName, fgmName);
		ConvertName(a1->medicalChartChao_->Data1.Chao->pParamGC->FGroundFatherName, fgfName);

		for (size_t i = 0; i < LengthOfArray(HealthCenter_DNAMenu2); i++)
			DrawChaoHudThing((ChaoHudThing*)&HealthCenter_DNAMenu2[i], -1);

		DrawMedicalChartText("Parents", 444, 83.5f, 999, 20.0f, 5);

		DrawMedicalChartText(
			"Grandparents",
			288.0,
			252 + OFFSET_ALL,
			999.0,
			16.0,
			0);

		DrawMedicalChartText(motherName, 340, 90.0f + OFFSET_ALL, 999, 24.0f, 5);
		DrawMedicalChartText(fatherName, 540, 90.0f + OFFSET_ALL, 999, 24.0f, 5);
		DrawMedicalChartText(mgmName, 288.0, 265 + OFFSET_ALL + 20.0f, 999, 24.0f, 0);
		DrawMedicalChartText(mgfName, 288.0, 265 + OFFSET_ALL + 40.0f, 999, 24.0f, 0);
		DrawMedicalChartText(fgmName, 288.0f + 24 * 8.5f, 265 + OFFSET_ALL + 20.0f, 999, 24.0f, 0);
		DrawMedicalChartText(fgfName, 288.0f + 24 * 8.5f, 265 + OFFSET_ALL + 40.0f, 999, 24.0f, 0);

		VoidFunc(sub_42C170, 0x42C170);
		VoidFunc(sub_42C0A0, 0x42C0A0);
		VoidFunc(sub_42C030, 0x42C030);
		if (*(unsigned char*)0x1A55831 != 1)
		{
			if (*(int*)0x25EFE50)
			{
				if (*(int*)0x25EFE50 == 1)
				{
					*(int*)0x25EFE50 = 3;
					sub_42C0A0();
				}
				else if (*(int*)0x25EFE50 == 2)
				{
					*(int*)0x25EFE50 = 3;
					sub_42C170();
				}
			}
			else
			{
				*(int*)0x25EFE50 = 3;
				sub_42C030();
			}
		}
		//Renderer_SetRenderState(*(void**)0x1A557C0, 4, 7);
		DoLighting(10);

		if (!noMother)
		{
			njPushMatrixEx();
			memset(_nj_current_matrix_ptr_, 0, 0x30u);
			*_nj_current_matrix_ptr_ = 1.0;
			_nj_current_matrix_ptr_[5] = 1.0;
			_nj_current_matrix_ptr_[10] = 1.0;
			ProjectToScreen(388 - 50, 232.0 + OFFSET_ALL - 6, -23.0f);
			njRotateY(NULL, -0xE000);
			pDisplaySub(pMother);
			pField20Sub(pMother);
			njPushMatrixEx();
		}

		if (!noFather)
		{
			njPushMatrixEx();
			memset(_nj_current_matrix_ptr_, 0, 0x30u);
			*_nj_current_matrix_ptr_ = 1.0;
			_nj_current_matrix_ptr_[5] = 1.0;
			_nj_current_matrix_ptr_[10] = 1.0;
			ProjectToScreen(548, 232.0 + OFFSET_ALL - 6, -23.0f);
			njRotateY(NULL, 0xE000);
			pDisplaySub(pFather);
			pField20Sub(pFather);
			njPushMatrixEx();
		}

		DoLighting(LightIndexBackupMaybe);
	}
	else
	{
		DrawMedicalChartText("Parents", 444, 83.5f, 999, 20.0f, 5);
		DrawMedicalChartText("No data", 444, 90.0f + OFFSET_ALL, 999, 20.0f, 5);
	}
}

int FriendsSelection = 0;
void HealthCenterFriendsMenu(HealthCenter* a1)
{
	if (FriendsSelection > 0 && MenuButtons_Pressed[0] & Buttons_Left)
		FriendsSelection--;
	if (FriendsSelection < AL_KW_GetFriendCount(a1->medicalChartChao_) && MenuButtons_Pressed[0] & Buttons_Right)
		FriendsSelection++;

	sub_583C60();
	njSetTexture((NJS_TEXLIST*)0x011D2ACC);
	njSetTextureNum(7, 0, 0, 0);
	for (size_t i = 0; i < LengthOfArray(HealthCenter_FriendMenu); i++)
		DrawChaoHudThing((ChaoHudThing*)&HealthCenter_FriendMenu[i], -1);
	DrawMedicalChartText("Friends", 444, 83.5f, 999, 20.0f, 5);
	char friendCountStr[8];
	sprintf_s(friendCountStr, "(%d/%d)", FriendsSelection + 1, AL_KW_GetFriendCount(a1->medicalChartChao_));
	DrawMedicalChartText(friendCountStr, 444, 110, 999, 20.0f, 5);
}

const int sub_536770Ptr = 0x536770;
void sub_536770(unsigned int a1, float a2, float a3, float a4, float a5, float a6)
{
	__asm
	{
		push a6
		push a5
		push a4
		push a3
		push a2
		mov eax, a1
		call sub_536770Ptr
		add esp, 20
	}
}

const int sub_5369F0Ptr = 0x5369F0;
void sub_5369F0(unsigned int a1, float a2, float a3, float a4)
{
	__asm
	{
		push a4
		push a3
		push a2
		mov eax, a1
		call sub_5369F0Ptr
		add esp, 12
	}
}




int bodyTypeBackup = -1; //HACK: used to make the chao disappear temporarily
float tempAdjustment = 0.5f;
DataArray(char, byte_8A274C, 0x008A274C, 8);

void __cdecl HealthCenterDNAHook(int a1, HealthCenter* TextLocation)
{
	if (MenuButtons_Pressed[0] & Buttons_Up)
		isInDNAMenu = 0;
	else if (MenuButtons_Pressed[0] & Buttons_Down)
		isInDNAMenu = 1;
	//if (TextLocation->openedMedicalChart && TextLocation->medicalChartMenu == 0 && !(MenuButtons_Held[0] & Buttons_Y) && MenuButtons_Held[0] & Buttons_Up)
		//TextLocation->medicalChartMenu = 4;
	if (TextLocation->openedMedicalChart && TextLocation->medicalChartMenu == 0 && !(MenuButtons_Held[0] & Buttons_Y) && MenuButtons_Held[0] & Buttons_Down)
	{
		if (TextLocation->medicalChartChao && bodyTypeBackup < 0) {
			bodyTypeBackup = TextLocation->medicalChartChao->Data1.Chao->pParamGC->BodyType;
			TextLocation->medicalChartChao->Data1.Chao->pParamGC->BodyType = (SADXBodyType)9; //setting it to a bodytype that doesnt exist makes the switch case in the chao draw code quit
		}
		if (!pMother)
		{
			noMother = false;
			noFather = false;
			ChaoDataBase* data = nullptr;
			KarateOpponent* opponentPtr = nullptr;

			noMother = TextLocation->medicalChartChao_->Data1.Chao->pParamGC->Gene.MotherID.id[0] == 0;
			if (noMother)
			{
				data = nullptr;
				opponentPtr = nullptr;
			}
			else
			{
				opponentPtr = nullptr;
				data = AL_KW_FindChaoBasedOnId(TextLocation->medicalChartChao_->Data1.Chao->pParamGC->Gene.MotherID);
				if (data == nullptr)
				{
					opponentPtr = &TextLocation->medicalChartChao_->Data1.Chao->pParamGC->motherData;
				}
			}

			//init
			NJS_VECTOR pos = { 0,0,0 };

			pMother = CreateChao(
				(ChaoData*)data,
				1,
				opponentPtr,
				&pos,
				0);
			pMother->Data1.Chao->field_B0 &= ~8u;
			pMother->Data1.Chao->field_B0 &= ~2u;
			pMother->Data1.Chao->field_B0 |= 0x2000;
			pMother->Data1.Chao->field_B0 &= ~0x10;
			pMother->Data1.Chao->field_B0 &= ~0x20000;

			noFather = TextLocation->medicalChartChao_->Data1.Chao->pParamGC->Gene.FatherID.id[0] == 0;
			if (noFather) //if id is invalid then dont do anything
			{
				data = nullptr;
				opponentPtr = nullptr;
			}
			else
			{
				opponentPtr = nullptr;
				data = AL_KW_FindChaoBasedOnId(TextLocation->medicalChartChao_->Data1.Chao->pParamGC->Gene.FatherID); //find current parent
				if (data == nullptr)
				{
					//if cant find it, use last "image" of them
					opponentPtr = &TextLocation->medicalChartChao_->Data1.Chao->pParamGC->fatherData;
				}
			}

			pFather = CreateChao(
				(ChaoData*)data,
				1,
				opponentPtr,
				&pos,
				0);
			pFather->Data1.Chao->field_B0 &= ~8u;
			pFather->Data1.Chao->field_B0 &= ~2u;
			pFather->Data1.Chao->field_B0 |= 0x2000;
			pFather->Data1.Chao->field_B0 &= ~0x10;
			pFather->Data1.Chao->field_B0 &= ~0x20000;

			pDisplaySub = pMother->DisplaySub;
			pField20Sub = pMother->field_20;
			if (!pDisplaySub) pDisplaySub = (ObjectFuncPtr)nullsub_1;
			if (!pField20Sub) pField20Sub = (ObjectFuncPtr)nullsub_1;
			pMother->DisplaySub = 0;
			pMother->field_20 = 0;
			pFather->DisplaySub = 0;
			pFather->field_20 = 0;
		}
		HealthCenterDNAMenu(TextLocation);
	}
	else
	{
		if (TextLocation->medicalChartChao && bodyTypeBackup >= 0) {
			TextLocation->medicalChartChao->Data1.Chao->pParamGC->BodyType = (SADXBodyType)bodyTypeBackup;
			bodyTypeBackup = -1;
		}

		if (pMother)
		{
			//deinit
			pMother->MainSub = DeleteObject_;
			pMother = 0;
			pFather->MainSub = DeleteObject_;
			pFather = 0;
		}

		if (!TextLocation->Chao_field_20) TextLocation->Chao_field_20 = (int)nullsub_1;
		if (!TextLocation->Chao_DisplaySub) TextLocation->Chao_DisplaySub = (int)nullsub_1;

		if (TextLocation->medicalChartMenu < 4)
			DisplayHealthCenterMedicalChart(a1, TextLocation);
		VoidFunc(sub_4293B0, 0x4293B0);
		sub_4293B0();
		//njColorBlendingMode(0, NJD_COLOR_BLENDING_SRCALPHA);
		//njColorBlendingMode(1, NJD_COLOR_BLENDING_INVSRCALPHA);
		if (TextLocation->openedMedicalChart && TextLocation->medicalChartMenu == 0)
		{
			if (TextLocation->field_8)
			{
				int v11 = TextLocation->field_8->Data1.Chao->pParamGC->Gene.LifeTime[1];
				int v12 = TextLocation->field_8->Data1.Chao->pParamGC->Gene.LifeTime[0];
				if ((unsigned __int8)v12 < v11)
				{
					v12 = v11;
				}

				ChaoHudThingOld bar1 = { { 296, 580 }, { (264 + 32 * 5) - 5, (264 + 32 * 5) + 24 - 5 }, { 1680, 1776 }, { 2448, 2544 } };
				ChaoHudThingOld bar2 = { { 580, 596 }, { (264 + 32 * 5) - 5, (264 + 32 * 5) + 24 - 5 }, { 1552, 2032 }, { 2064, 2800 } };
				sub_583C60();
				njSetTexture((NJS_TEXLIST*)0x011D2ACC);
				njSetTextureNum(7, 0, 0, 0);

				{
					ChaoHudThing gradeElem;
					gradeElem.x0 = 558;
					gradeElem.x1 = 578;
					for (int i = 0; i < 5; i++)
					{
						int gradeIndex = byte_8A274C[i];
						if (gradeIndex >= 8u)
							gradeIndex = 0;

						int grade = TextLocation->field_8->Data1.Chao->pParamGC->StatGrades[gradeIndex];
						if (grade == 6 || grade == 7) //Grade_X
						{
							njSetTexture(&CWE_UI_TEXLIST);
							njSetTextureNum(25, 0, 0, 0);
							gradeElem.u0 = 0;
							gradeElem.u1 = (short)((39 / 76.0f) * 4096);
							gradeElem.v0 = 0;
							gradeElem.v1 = 4096;
						}
						else
						{
							njSetTexture((NJS_TEXLIST*)0x011D2ACC);
							njSetTextureNum(7, 0, 0, 0);
							gradeElem.u0 = word_8A2754[2 * grade];
							gradeElem.u1 = word_8A2754[2 * grade] + 624;
							gradeElem.v0 = word_8A2754[(2 * grade) + 1];
							gradeElem.v1 = word_8A2754[(2 * grade) + 1] + 624;
						}
						gradeElem.y0 = 32 * i + 262;
						gradeElem.y1 = 32 * i + 282;
						DrawChaoHudThing(&gradeElem, -1);       // grades	
					}
				}
				sub_583C60();
				njSetTexture((NJS_TEXLIST*)0x011D2ACC);
				njSetTextureNum(7, 0, 0, 0);
				DrawChaoHudThing((ChaoHudThing*)&bar1, -1);
				DrawChaoHudThing((ChaoHudThing*)&bar2, -1);

				DataArray(ChaoHudThingB, stru_13128B0, 0x13128B0, 10);
				ChaoHudThingB lifespan = { 1, 128 * 0.55f, 34 * 0.5f, 0,0,0.995f,0.98f, &CWE_UI_TEXLIST, 4 };
				//ChaoHudThing lifespan = { {0x132, (264 + 32 * 5.25f)}, {}, {0,0}, {4096, 4096} };
				sub_536770(
					(Uint32)((TextLocation->field_8->Data1.Chao->pParamGC->Lifespan / 3900.0f) * 1000),
					380.0f, 
					(264 + 32 * 5) + 1, 
					92.0f, 
					14.0f, 
					1.0f
				);
				sub_5369F0(TextLocation->field_8->Data1.Chao->pParamGC->Lifespan, 476, (264 + 32 * 5), 1.0f);
				DrawChaoHudThingB(&lifespan, 306, (264 + 32 * 5), -1.2f, 1.0f, 1.0f, -1, -1);
				SetChaoHUDThingBColor(1, 0.72f, 1, 0);
				DrawChaoHudThingB(&stru_13128B0[v12], 561, (264 + 32 * 5) - 2, -1.2f, 1.25f, 1.25f, -1, -1);

			}
		}
	}
	if (TextLocation->medicalChartMenu == 4)
	{
		if (MenuButtons_Pressed[0] & Buttons_Down)
			TextLocation->medicalChartMenu = 0;
		HealthCenterFriendsMenu(TextLocation);
	}
	if (TextLocation->medicalChartMenu == 1 && TextLocation->openedMedicalChart)
	{
		char buffer[256];
		sprintf(
			buffer,
			&TextLocation->dword60[*((int*)TextLocation->dword60 + 65)],
			TextLocation->field_8->Data1.Chao->pParamGC->ClockRollovers / TextLocation->field_8->Data1.Chao->pParamGC->Birthday
		);
		DrawMedicalChartText((const char *)buffer, 288.0, 243.0, 999.0, 20.0, 0);
		sprintf(
			buffer,
			"Birthday in %d Chao days.",
			TextLocation->field_8->Data1.Chao->pParamGC->Birthday - (TextLocation->field_8->Data1.Chao->pParamGC->ClockRollovers % TextLocation->field_8->Data1.Chao->pParamGC->Birthday)
		);
		DrawMedicalChartText((const char*)buffer, 288.0, 392, 999.0, 20.0, 0);
	}
}
void __declspec(naked) DisplayHealthCenterMedicalChartCall()
{
	__asm
	{
		push[esp + 04h] // TextLocation
		push esi // a1

		// Call your __cdecl function here:
		call HealthCenterDNAHook

		pop esi // a1
		add esp, 4 // TextLocation
		retn
	}
}
const int sub_58DB30Ptr = 0x58DB30;
void sub_58DB30(int a1)
{
	__asm
	{
		mov edi, a1
		call sub_58DB30Ptr
	}
}
void __cdecl sub_58EA40(HealthCenter* a1)
{
	if (a1->openedMedicalChart)
	{
		if (!(a1->medicalChartMenu == 0 && MenuButtons_Held[0] & Buttons_Y)) 
		{
			if ((ControllerPointers[0]->press & Buttons_L) != 0 || (MenuButtons_Pressed[0] & 0x40) != 0)
			{
				if (--a1->medicalChartMenu < 0)
				{
					a1->medicalChartMenu = 2;
				}
				PlaySoundProbably(0x8000, 0, 0, 0);
			}
			if ((ControllerPointers[0]->press & (Buttons_R | Buttons_A)) != 0 || ControllerPointers[0]->press & 0x80)
			{
				if (++a1->medicalChartMenu == 3)
				{
					a1->medicalChartMenu = 0;
				}
				PlaySoundProbably(0x8000, 0, 0, 0);
			}
			if ((ControllerPointers[0]->press & 0x402) != 0)
			{
				sub_58DB30((int)a1);
				PlaySoundProbably(0x100A, 0, 0, 0);
			}
		}
		a1->field_16 += 16;
	}
}
const char* MessageCondition;
#define SETMSGVAR(b) messageValues.push_back(b) 

void __fastcall InitDoctorHook(HealthCenter* a1, int a2)
{
	MessageCondition = "Your Chao is very healthy.\nPlease take good care of your Chao.";
	AL_HealthCenter_SetMessage(a1, 108);
}
void __fastcall AL_HealthCenter_SetMessageStr(HealthCenter* a1, const char* a2)
{
	*(uint32_t*)0x01A267D0 = 0xFFFFFFFF;
	AlMsgWinAddLineC(a1->pkindercomessagething3C, a2, TextLanguage == 0);
}
signed int AL_HealthCenter_GetSickness(ObjectMaster* a1)
{
	AL_EMOTION* v1; // ecx
	int v2; // eax
	signed int result; // eax

	v1 = &a1->Data1.Chao->pParamGC->Emotion;
	v2 = 0;
	while (*(&v1->CoughLevel + v2) >= 0)
	{
	LABEL_4:
		if (++v2 >= 6)
		{
			return -1;
		}
	}
	*(&v1->CoughLevel + v2) = 100;
	switch (v2)
	{
	case 0:
		result = 3;
		break;
	case 1:
		result = 0;
		break;
	case 2:
		result = 4;
		break;
	case 3:
		result = 2;
		break;
	case 4:
		result = 5;
		break;
	case 5:
		result = 1;
		break;
	default:
		goto LABEL_4;
	}
	return result;
}
DataArray(short, word_8A1DA0, 0x008A1DA0, 6);
void __cdecl ConditionText(HealthCenter* a2)
{
	int msgID = AL_HealthCenter_GetSickness(a2->field_8);
	if(msgID == -1)
		DrawMedicalChartText(MessageCondition, 288.0, 307.0, 312.0, 20.0, 0);
	else
		DrawMedicalChartText((const char*)&a2->dword60[*(int*)&a2->dword60[4 * word_8A1DA0[msgID]]], 288.0, 307.0, 312.0, 20.0, 0);
}

std::vector<const char*> messageValues;

void Doctor_PersonalityText(ObjectMaster* tp, int emotion, const char* a1, const char* a2, const char* a3) {
	int val = AL_EmotionGetValue(tp, emotion);

	if (val >= -80 && val <= -40 && a1) 
		SETMSGVAR(a1);
	else if (val == 0 && a2)
		SETMSGVAR(a2);
	else if (val >= 40 && val <= 80 && a3)
		SETMSGVAR(a3);
}

void __fastcall DoctorMessage(HealthCenter* a1, int a2)
{
	if (a1->field_8 == nullptr)
		return;

	ObjectMaster* pChaoTask = a1->field_8;
	ChaoDataBase* pParam = a1->field_8->Data1.Chao->pParamGC;

	if (pParam == nullptr)
		return;

	messageValues.clear();

	int age = pParam->ClockRollovers / pParam->Birthday;
	
	SETMSGVAR("Your Chao is very healthy.\nPlease take good care of your Chao.");
	
	if (AL_IsChild(pChaoTask))
		SETMSGVAR("This Chao is still a baby. \n It'll be fun to see your Chao grow!");
	else
	{
		if (pParam->Happiness >= 0 && pParam->Happiness < 50)
			SETMSGVAR("It doesn't look like you're\ntaking very good care of your Chao");
		else if (pParam->Happiness < 80)
			SETMSGVAR("Looks like you're taking  \ngood care of your Chao.            ");
		else if (pParam->Happiness <= 100)
			SETMSGVAR("This Chao looks very happy\nThis is how you take care of Chao! ");

	}
	
	if (pParam->Reincarnations == 1 || pParam->Reincarnations == 2)
		SETMSGVAR("Take care of this Chao the\nway you have been so far.          ");
	else if (pParam->Reincarnations == 3 || pParam->Reincarnations == 4)
		SETMSGVAR("You really love your Chao.\nI can't believe this!              ");
	else if (pParam->Reincarnations > 4)
		SETMSGVAR("Looks like this Chao      \ntransformed so many times!         ");
	else if (age >= 4 && age <= 5)
		SETMSGVAR("This Chao has had a long  \nlife! Please take care of your Chao");
	
	if (AL_EmotionGetValue(pChaoTask, EM_ST_BREED) > 8000) SETMSGVAR("Your chao is gonna have a \nbaby! Make sure you have space.    ");
	else if (AL_EmotionGetValue(pChaoTask, EM_ST_BREED) > 5000) SETMSGVAR("Your Chao can mate another\nChao without Heart Fruits.         ");
	
	if (AL_EmotionGetValue(pChaoTask, EM_ST_HUNGER) > 8000) SETMSGVAR("This Chao is hungry.      \nPlease feed your Chao.             ");
	if (AL_EmotionGetValue(pChaoTask, EM_ST_SLEEPY) > 8000) SETMSGVAR("Looks like this Chao is sleepy.\nLet your Chao rest.           ");
	if (AL_EmotionGetValue(pChaoTask, EM_ST_SLEEP_DEPTH) > 0) SETMSGVAR("This Chao looks tired.    \nI think this Chao needs a nap.     ");
	if (AL_EmotionGetValue(pChaoTask, EM_ST_TEDIOUS) > 4000) SETMSGVAR("Looks like this Chao is   \nbored. It wants to play.           ");

	//maybe condition? doesnt make much sense atm so i commented it out
	//if (pParam->Emotion.Energy < 900) SETMSGVAR("This Chao is not healthy. \nBe careful not to catch a cold.    ");

	if (AL_EmotionGetValue(pChaoTask, EM_ST_THIRSTY) > 8000) SETMSGVAR("Looks like this Chao is   \nready to swim.                     ");

	if (AL_EmotionGetValue(pChaoTask, EM_MD_SORROW) > 0 
		&& AL_EmotionGetValue(pChaoTask, EM_MD_FEAR) > 0 
		&& AL_EmotionGetValue(pChaoTask, EM_MD_PAIN) > 0)
		SETMSGVAR("Looks like Chao is in pain\nDid you hurt your Chao?            ");
	
	//hat
	switch (pParam->Headgear)
	{
	case OBAKE_HEAD_PARTS_PUMPKIN:
		SETMSGVAR("This Chao is wearing a    \npumpkin hat. You scared me!        ");
		break;
	case OBAKE_HEAD_PARTS_SKULL:
		SETMSGVAR("Are you wearing a skeleton\nhat? Oh No. What are you?          ");
		break;
	case OBAKE_HEAD_PARTS_WMELON:
		SETMSGVAR("A watermelon?             \nThat's strange...                  ");
		break;
	case OBAKE_HEAD_PARTS_APPLE:
		SETMSGVAR("Is this Chao wearing an   \napple? What happened?              ");
		break;
	case OBAKE_HEAD_PARTS_FLOWERPOT:
		SETMSGVAR("This Chao is wearing a pot\nCan it cook?                       ");
		break;
	case OBAKE_HEAD_PARTS_CAN:
		SETMSGVAR("Is that a tin can?        \nAre you okay?                      ");
		break;
	case OBAKE_HEAD_PARTS_PAPERBAG:
		SETMSGVAR("This chao is wearing a    \npaper bag. Is it embarrassed?      ");
		break;
	case OBAKE_HEAD_PARTS_CDBOX:
		SETMSGVAR("A carboard box? Don't be  \nafraid to show your face okay?     ");
		break;
	default:
		if (pParam->Headgear >= OBAKE_HEAD_PARTS_EGG && pParam->Headgear <= OBAKE_HEAD_PARTS_EGG_Env2)
			SETMSGVAR("This chao is wearing an   \neggshell. Did it hatch properly?   ");

		break;
	}
	

	if (AL_EmotionGetValue(pChaoTask, EM_ST_LONELY) > 6000)
		SETMSGVAR("This Chao looks lonely.   \nDo you spend time with your Chao?  ");
	else if (AL_KW_GetFriendCount(pChaoTask) > 12)
		SETMSGVAR("This Chao is so popular.  \nThis Chao is friends with everybody");

	if (pParam->BodyType == 1)
		SETMSGVAR("This is an egg....   \nWait a minute...  ");
	else if (pParam->BodyType == 2)
		SETMSGVAR("This is a nice Omochao.  \nThe Chao that built this did a good job.");

	if (pParam->StatGrades[7] >= 10)
		SETMSGVAR("This Chao is very smart.  \nThis Chao must be a fast learner.  ");
	else if (pParam->StatGrades[7] >= 4 && pParam->StatGrades[7] <= 9)
		SETMSGVAR("This Chao is well-behaved.\nit will learn a lot.               ");
	if (pParam->StatGrades[6] >= 10)
		SETMSGVAR("This is a very lucky Chao.\nI'm so jealous!!                   ");
	else if (pParam->StatGrades[6] >= 4 && pParam->StatGrades[6] <= 9)
		SETMSGVAR("This Chao is lucky.       \nPlease take good care of your Chao.");
	if (pParam->StatLevels[0] == 99 && pParam->StatLevels[1] == 99 &&
		pParam->StatLevels[2] == 99 && pParam->StatLevels[3] == 99 &&
		pParam->StatLevels[4] == 99)
		SETMSGVAR("You really love your Chao.\nYou're an excellent Chao trainer!  ");
	if (pParam->Type > 2 &&
		pParam->StatLevels[0] < 10 && pParam->StatLevels[1] < 10 &&
		pParam->StatLevels[2] < 10 && pParam->StatLevels[3] < 10 &&
		pParam->StatLevels[4] < 10)
		SETMSGVAR("This Chao is spoiled.     \nIs this Chao potty trained?        ");

	Doctor_PersonalityText(pChaoTask, EM_PER_CURIOSITY,
		"This Chao is easily bored.\nThis Chao must keep you busy.      ",
		"This Chao is so quiet.    \nBe sure to take it outside to play.",
		"This Chao is very curious.\nThis Chao will try anything.       ");

	Doctor_PersonalityText(pChaoTask, EM_PER_KINDNESS,
		"This Chao gets frustrated easily.\n It's not healthy.          ",
		"This Chao seems to behave normally\nTake good care of your Chao",
		"This Chao is very kind!\nYour Chao must have a lot of friends. ");

	Doctor_PersonalityText(pChaoTask, EM_PER_AGRESSIVE,
		"This Chao cries a lot.    \nEveryone will make fun of you.     ",
		nullptr,
		"Such an energetic Chao!   \nTake it easy, OK?                  ");

	Doctor_PersonalityText(pChaoTask, EM_PER_SLEEPY_HEAD,
		"This Chao is very naive.  \nYou have to teach it!              ",
		nullptr,
		"This Chao is experienced.  \nPlease take care of your Chao.     ");

	Doctor_PersonalityText(pChaoTask, EM_PER_SOLITUDE,
		"This Chao seems pretty confident.\nDon't overdo it, okay?      ",
		nullptr,
		"This Chao like to be lonely.\nTake good care of your Chao.     ");

	Doctor_PersonalityText(pChaoTask, EM_PER_VITALITY,
		"This Chao likes to sit around a lot.\nYou have a laid back Chao",
		nullptr,
		"It seems mating season\nis coming soon for the young one!      ");

	Doctor_PersonalityText(pChaoTask, EM_PER_SOLITUDE,
		nullptr,
		nullptr,
		"This Chao seems to learn  \nquickly. Take care of your Chao.   ");

	Doctor_PersonalityText(pChaoTask, EM_PER_CHATTY,
		"This Chao is very shy.    \nIt's not very confident, is it?    ",
		"This Chao seems to behave normally\nTake good care of your Chao",
		"This Chao is very chatty!\nYour Chao must have a lot of friends");

	Doctor_PersonalityText(pChaoTask, EM_PER_GLUTTON,
		"This chao doesn't eat much\nDon't over feed your Chao!         ",
		"This Chao eats very well. \nPlease take good care of your Chao.",
		"This Chao eats more than  \nusual. Make sure you feed it!      ");

	Doctor_PersonalityText(pChaoTask, EM_PER_CALM,
		"This Chao seems to be anxious and stressed out\nTake it easy.  ",
		"This Chao has some potential.\nPlease take care of your Chao.  ",
		"This Chao is relaxed.     \nPlease take care of your Chao.     ");
	
	const char* msgVal = messageValues[(size_t)(njRandom() * ((float)messageValues.size() - 0.01f))];
	AL_HealthCenter_SetMessageStr(a1, msgVal);
	MessageCondition = msgVal;
}

static void __declspec(naked) sub_58EA40Hook()
{
	__asm
	{
		push eax // a1

		// Call your __cdecl function here:
		call sub_58EA40

		pop eax // a1
		retn
	}
}
static void __declspec(naked) sub_58D9F0Hook()
{
	__asm
	{
		push eax // a2
		// Call your __cdecl function here:
		call ConditionText
		pop eax // a2

		retn
	}
}

void alg_kinder_he_Init() {
	//health center
	WriteCall((void*)0x0058E01F, nullsub_1); //killing the grade draw call
	WriteCall((void*)0x0058E8FC, nullsub_1); //killing the age call so that i can run my own 
	WriteCall((void*)0x0058F9A3, DisplayHealthCenterMedicalChartCall);
	WriteJump((void*)0x58EA40, sub_58EA40Hook);
	if (gConfigVal.DoctorChaoInfo)
	{
		WriteCall((void*)0x0058F6FE, DoctorMessage);
		WriteCall((void*)0x0058F558, InitDoctorHook);
		WriteCall((void*)0x0058E9BA, sub_58D9F0Hook);
	}

	//doctor item display
	WriteJump((void*)0x58F980, sub_58F980);

	//Upgrade stats
	WriteCall((void*)0x0058DF7E, PurchaseGradesHook);
	WriteData((int*)0x0058EFE5, (int)0x800);
	WriteData((int*)0x0058F040, (int)0x800);

	//stretch panel to fit lifespan
	HealthCenter_MenuBlue[35].y1 = (short)(264 + 32 * 5.25) + 8;
	HealthCenter_MenuBlue[36].y1 = (short)(264 + 32 * 5.25) + 8;
	HealthCenter_MenuBlue[37].y1 = (short)(264 + 32 * 5.25) + 8;
	HealthCenter_MenuBlue[38].y0 = (short)(264 + 32 * 5.25) + 8;
	HealthCenter_MenuBlue[39].y0 = (short)(264 + 32 * 5.25) + 8;
	HealthCenter_MenuBlue[40].y0 = (short)(264 + 32 * 5.25) + 8;
	HealthCenter_MenuBlue[38].y1 = HealthCenter_MenuBlue[38].y0 + 8;
	HealthCenter_MenuBlue[39].y1 = HealthCenter_MenuBlue[39].y0 + 8;
	HealthCenter_MenuBlue[40].y1 = HealthCenter_MenuBlue[40].y0 + 8;
}