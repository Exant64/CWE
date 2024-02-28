#include "stdafx.h"

#include "ALifeSDK_Functions.h"

DataArray(__int16, word_8A7A70, 0x8A7A70, 3);
DataArray(__int16, word_8A7AC0, 0x8A7AC0, 3);
DataPointer(__int8, byte_0053A5BC, 0x0053A5BC);
const int SetChunkTextureIDPtr = 0x0055EA00;
void  SetChunkTextureID_(NJS_CNK_MODEL* a1, int a2)
{
	__asm
	{
		mov ecx, a1
		mov edi, a2
		call SetChunkTextureIDPtr
	}
}
void __cdecl AL_FaceSetEyeCWE(ObjectMaster* a3, int a2, int a1)
{
	ChaoFacialData* v3; // esi
	ChaoData1* v4; // ecx
	int v5; // eax
	__int16 v6; // di
	NJS_OBJECT* v7; // eax
	NJS_CNK_MODEL* v8; // eax
	NJS_OBJECT* v9; // esi
	NJS_CNK_MODEL* v10; // ecx

	v3 = &a3->Data1.Chao->Face;
	v3->EyeTimer = a1;
	v3->EyeCurrNum = a2;
	if (!a3->Data1.Chao->Face.EyeCurrNum)
	{
		if (v3->EyeDefaultNum == 10)
		{
			v3->EyeLidExpressionAimCloseAng = 0x3555;
			v3->EyeLidExpressionAimSlopeAng = 0xE38;
			goto LABEL_10;
		}
		goto LABEL_8;
	}
	if (a3->Data1.Chao->Face.EyeCurrNum == 9)
	{
		v3->EyeLidExpressionAimCloseAng = 0x4000;
	LABEL_9:
		v3->EyeLidExpressionAimSlopeAng = 0;
		goto LABEL_10;
	}
	if (a3->Data1.Chao->Face.EyeCurrNum != 10)
	{
	LABEL_8:
		v3->EyeLidExpressionAimCloseAng = 0;
		goto LABEL_9;
	}
	v3->EyeLidExpressionAimCloseAng = 0x3555;
	v3->EyeLidExpressionAimSlopeAng = 0xE38;
LABEL_10:
	v4 = a3->Data1.Chao;
	v5 = v4->Face.EyeCurrNum;
	if (v4->Face.EyeCurrNum && (v5 <= 8 || v5 > 10))
	{
		v6 = word_8A7A70[a2];
	}
	else
	{
		v6 = word_8A7AC0[v3->EyeColorNum];
	}
	if ((unsigned __int8)(v4->pParamGC->Type - byte_0053A5BC) <= 2u)
	{
		if (v5 == 7)
		{
			v6 = word_8A7A70[a2];
		}
		else
		{
			v6 = word_8A7AC0[v3->EyeColorNum];
		}
	}
	//alignment 
	switch (v4->pParamGC->EyeAlignment)
	{
	case 1:
		switch (v6)
		{
		case 8:
		case 9:
			v6 = 0;
			break;
		case 11:
		case 12:
			v6 = 10;
			break;
		}
		break;
	case 2:
		switch (v6)
		{
		case 0:
		case 8:
			v6 = 9;
			break;
		case 10:
		case 12:
			v6 = 11;
			break;
		}
		break;
	case 3:
		switch (v6)
		{
		case 0:
		case 9:
			v6 = 8;
			break;
		case 10:
		case 11:
			v6 = 12;
			break;
		}
		break;
	}

	if (v4->pParamGC->BodyType != 2) 
	{
		//colors
		if (v4->pParamGC->EyeColor)
		{
			v3->Flag = 1;
			switch (v6)
			{
			case 0:
				v6 = 0;
				break;
			case 5:
				v6 = 1;
				break;
			case 8:
				v6 = 2;
				break;
			case 9:
				v6 = 3;
				break;
			case 10:
				v6 = 4;
				break;
			case 11:
				v6 = 5;
				break;
			case 12:
				v6 = 6;
				break;
			default:
				v3->Flag = 0;
				break;
			}
		}
		else
			v3->Flag = 0;
	}
	else
	{
		switch (v6)
		{
		case 10:
			v6 = 0;
			break;
		case 11:
			v6 = 9;
			break;
		case 12:
			v6 = 8;
			break;
		}
	}
	v7 = v3->pEyeObject[0];
	if (v7)
	{
		v8 = (NJS_CNK_MODEL*)v7->chunkmodel;
		if (v8)
		{
			SetChunkTextureID_(v8, v6);
		}
	}
	v9 = v3->pEyeObject[1];
	if (v9)
	{
		v10 = (NJS_CNK_MODEL*)v9->chunkmodel;
		if (v10)
		{
			SetChunkTextureID_(v10, v6);
		}
	}
}
static void __declspec(naked) AL_FaceSetEyeHook()
{
	__asm
	{
		push eax // a1
		push edx // a2
		push ecx // a3

		// Call your __cdecl function here:
		call AL_FaceSetEyeCWE

		pop ecx // a3
		pop edx // a2
		pop eax // a1
		retn
	}
}

unsigned __int8 eye_default_num[3][3][3] =
{
  { { ChaoEyes_HalfClosed, 0u, 0u }, { ChaoEyes_HalfClosed, 0u, 0u }, { 0u, 0u, 0u } },
  { { ChaoEyes_HalfClosed, 0u, 0u }, { 0u, 0u, ChaoEyes_ClosedUp }, { 0u, 0u, ChaoEyes_ClosedUp } },
  { { ChaoEyes_Mean, ChaoEyes_Mean, ChaoEyes_Mean }, { ChaoEyes_HalfClosed, 0u, ChaoEyes_ClosedUp }, { 0u, ChaoEyes_ClosedUp, ChaoEyes_ClosedUp } }
};

unsigned __int8 mouse_default_num[3][3][3] =
{
	//aggressive < -45
	{
		{ ChaoMouth_Squiggly, 0u, ChaoMouth_ClosedFrown },							//kindness < -45
		{ ChaoMouth_Squiggly, ChaoMouth_ClosedFrown, ChaoMouth_ClosedFrown },		//kindness >= -45 && kindness < 45
		{ ChaoMouth_Squiggly, ChaoMouth_ClosedSmile, ChaoMouth_ClosedSmile }		//kindness >= 45 
	},
	//aggressive >= -45 && aggressive < 45
	{
		{ 0u, ChaoMouth_ToothySmile, ChaoMouth_ToothySmile },						//kindness < -45
		{ ChaoMouth_ClosedSmile, ChaoMouth_ClosedSmile, ChaoMouth_ClosedSmile }, 	//kindness >= -45 && kindness < 45
		{ 0u, ChaoMouth_ClosedSmile, ChaoMouth_ClosedSmile }						//kindness >= 45 
	},
	//aggressive >= 45
	{
		{ 0u, ChaoMouth_ToothySmile, ChaoMouth_ToothySmile },						//kindness < -45
		{ 0u, ChaoMouth_ClosedFrown, ChaoMouth_ClosedFrown },						//kindness >= -45 && kindness < 45
		{ ChaoMouth_ClosedSmile, ChaoMouth_ClosedSmile, ChaoMouth_ClosedSmile }		//kindness >= 45 
	}
};

const int Chao_SetMouthPtr = 0x0053A5A0;
void AL_FaceSetMouth(ObjectMaster* a2, int a3, int a1)
{
	__asm
	{
		mov edx, a1
		mov ecx, a2
		push a3
		call Chao_SetMouthPtr
		add esp, 4
	}
}

const int Chao_SetEyePtr = 0x0053A4B0;
void AL_FaceSetEye(ObjectMaster* a3, int a2, int a1)
{
	__asm
	{
		mov eax, a1
		mov edx, a2
		mov ecx, a3
		call Chao_SetEyePtr
	}
}

void AL_FaceChangeEye(ObjectMaster* tp, int EyeNum) {
	AL_FaceSetEye(tp, EyeNum, -1);
}

void AL_FaceChangeMouth(ObjectMaster* tp, int MouthNum) {
	AL_FaceSetMouth(tp, MouthNum, -1);
}

void AL_FaceInit() {
	//new mouth tex IDs
	WriteData((unsigned char*)0x008A7AA8, (unsigned char)0x09); //7-0
	WriteData((unsigned char*)0x008A7AAA, (unsigned char)0x08); //8-1
	WriteData((unsigned char*)0x008A7AB4, (unsigned char)0x0E); //11-0
	WriteData((unsigned char*)0x008A7AB6, (unsigned char)0x00); //11-1
	WriteData((unsigned char*)0x008A7ABC, (unsigned char)0x12); //13-0
	WriteData((unsigned char*)0x008A7ABE, (unsigned char)0x11); //13-1

	WriteJump((void*)Chao_SetEyePtr, AL_FaceSetEyeHook);

	//eye coloring
	//game sets texture ID on eyes to 5 if half closed eyes
	WriteCall((void*)0x0053A49F, nullsub_1);
	WriteCall((void*)0x0053A487, nullsub_1);
}