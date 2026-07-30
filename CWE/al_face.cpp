#include "stdafx.h"

#include "Chao.h"
#include "asmutil.h"

DataArray(__int16, word_8A7A70, 0x8A7A70, 3);
DataArray(__int16, word_8A7AC0, 0x8A7AC0, 3);
DataPointer(__int8, byte_0053A5BC, 0x0053A5BC);

ASM_FUNC void AL_MatChangeModelTexture(NJS_CNK_MODEL* pModel, uint16_t TexID) {
    // save regs
    ASM_PUSH( edi );

    // arguments
    ASM_MOVE( edi, ASM_ESP(2+0 +1) ); // TexID
    ASM_MOVE( ecx, ASM_ESP(1+0 +1) ); // pModel

    // call
    ASM_CALL_R( edx, 0x0055EA00 );

    // pull regs
    ASM_POP( edi );

    // return
    ASM_RET( 0 );
}


// todo: refactor with decomp
void __cdecl AL_FaceSetEyeCWE(task* a3, int a2, int a1)
{
	AL_FACE_CTRL* v3; // esi
	chaowk* v4; // ecx
	int v5; // eax
	__int16 v6; // di
	AL_OBJECT* v7; // eax
	NJS_CNK_MODEL* v8; // eax
	AL_OBJECT* v9; // esi
	NJS_CNK_MODEL* v10; // ecx

	v3 = &GET_CHAOWK(a3)->Face;
	v3->EyeTimer = a1;
	v3->EyeCurrNum = a2;
	if (!v3->EyeCurrNum)
	{
		if (v3->EyeDefaultNum == 10)
		{
			v3->EyeLidExpressionAimCloseAng = 0x3555;
			v3->EyeLidExpressionAimSlopeAng = 0xE38;
			goto LABEL_10;
		}
		goto LABEL_8;
	}
	if (GET_CHAOWK(a3)->Face.EyeCurrNum == 9)
	{
		v3->EyeLidExpressionAimCloseAng = 0x4000;
	LABEL_9:
		v3->EyeLidExpressionAimSlopeAng = 0;
		goto LABEL_10;
	}
	if (GET_CHAOWK(a3)->Face.EyeCurrNum != 10)
	{
	LABEL_8:
		v3->EyeLidExpressionAimCloseAng = 0;
		goto LABEL_9;
	}
	v3->EyeLidExpressionAimCloseAng = 0x3555;
	v3->EyeLidExpressionAimSlopeAng = 0xE38;
LABEL_10:
	v4 = GET_CHAOWK(a3);
	v5 = v4->Face.EyeCurrNum;
	if (v4->Face.EyeCurrNum && (v5 <= 8 || v5 > 10))
	{
		v6 = word_8A7A70[a2];
	}
	else
	{
		v6 = word_8A7AC0[v3->EyeColorNum];
	}
	if ((unsigned __int8)(v4->pParamGC->type - byte_0053A5BC) <= 2u)
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
	switch (GET_CWEPARAM(a3)->EyeAlignment)
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

	if (v4->pParamGC->body.FormNum != 2) 
	{
		//colors
		if (GET_CWEPARAM(a3)->EyeColor)
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
		v8 = (NJS_CNK_MODEL*)v7->pModel;
		if (v8)
		{
			AL_MatChangeModelTexture(v8, v6);
		}
	}
	v9 = v3->pEyeObject[1];
	if (v9)
	{
		v10 = (NJS_CNK_MODEL*)v9->pModel;
		if (v10)
		{
			AL_MatChangeModelTexture(v10, v6);
		}
	}
}
static void ASM_FUNC AL_FaceSetEyeHook() {
	ASM_PUSH(eax); // a1
	ASM_PUSH(edx); // a2
	ASM_PUSH(ecx); // a3

	ASM_CALL (AL_FaceSetEyeCWE);

	ASM_POP(ecx); // a3
	ASM_POP(edx); // a2
	ASM_POP(eax); // a1
	ASM_RET(0);
}

ASM_FUNC void  AL_FaceSetMouth(task* tp, int MouthNum, int32_t timer) {
    // arguments
    ASM_MOVE( edx, ASM_ESP(3+0) ); // timer
    ASM_PUSH(      ASM_ESP(2+0) ); // MouthNum
    ASM_MOVE( ecx, ASM_ESP(1+1) ); // tp

    // call
    ASM_CALL_R( eax, 0x0053A5A0 );

    // end arguments
    ASM_ESP_ADD( 1 );

    // return
    ASM_RET( 0 );
}

ASM_FUNC void AL_FaceSetEye(task* tp, int EyeNum, int32_t timer) {
    // save regs
    ASM_PUSH( ebx );

    // arguments
    ASM_MOVE( eax, ASM_ESP(3+0 +1) ); // timer
    ASM_MOVE( edx, ASM_ESP(2+0 +1) ); // EyeNum
    ASM_MOVE( ecx, ASM_ESP(1+0 +1) ); // tp

    // call
    ASM_CALL_R( ebx, 0x0053A4B0 );

    // pull regs
    ASM_POP( ebx );

    // return
    ASM_RET( 0 );
}

void AL_FaceChangeEye(task* tp, int EyeNum) {
	AL_FaceSetEye(tp, EyeNum, -1);
}

void AL_FaceChangeMouth(task* tp, int MouthNum) {
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

	WriteJump((void*)0x0053A4B0, (void*)AL_FaceSetEyeHook);

	//eye coloring
	//game sets texture ID on eyes to 5 if half closed eyes
	WriteCall((void*)0x0053A49F, (void*)nullsub_1);
	WriteCall((void*)0x0053A487, (void*)nullsub_1);
}