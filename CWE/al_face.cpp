#include "stdafx.h"
#include "ChaoMain.h"
#include "chaoenum.h"

#include "Chao.h"
#include "asmutil.h"

DataArray(__int16, AL_EyeTexIdList, 0x8A7A70, 3);
DataArray(__int16, EyeColorTexIdList, 0x8A7AC0, 3);
DataPointer(Uint8, byte_0053A5BC, 0x0053A5BC);

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

void AL_MatChangeObjectTexture(AL_OBJECT* pObject, Uint16 TexId) {
    if (pObject) {
        if (pObject->pModel) {
            AL_MatChangeModelTexture((NJS_CNK_MODEL*)pObject->pModel, TexId);
        }
    }
}

Sint16 AL_FaceGetEyeCurrNum(task* tp) {
    AL_FACE_CTRL* pFace = &GET_CHAOWK(tp)->Face;
    return pFace->EyeCurrNum;
}

static void AL_FaceSetEyeCWE(task* tp, int EyeNum, int timer) {
	AL_FACE_CTRL* pFace = &GET_CHAOWK(tp)->Face;
    Uint16 texid;

	pFace->EyeCurrNum = EyeNum;
	pFace->EyeTimer = timer;

	switch (AL_FaceGetEyeCurrNum(tp)) {
        default:
            pFace->EyeLidExpressionAimCloseAng = 0;
            pFace->EyeLidExpressionAimSlopeAng = 0;
            break;
        case AL_EYE_NUM_TRON:
            pFace->EyeLidExpressionAimCloseAng = NJM_DEG_ANG(90);
            pFace->EyeLidExpressionAimSlopeAng = 0;
            break;
        case AL_EYE_NUM_NORMAL: {
			auto defaultNum = pFace->EyeDefaultNum;
			if(gConfigVal.MoreAnimation) {
				if(AL_EmotionGetValue(tp, EM_MD_ANGER) > 80 && AL_EmotionGetValue(tp, EM_PER_AGRESSIVE) > 30) {
					defaultNum = AL_EYE_NUM_ANGER;
				}
			}

            if (pFace->EyeDefaultNum == AL_EYE_NUM_ANGER) {
                pFace->EyeLidExpressionAimCloseAng = NJM_DEG_ANG(75);
                pFace->EyeLidExpressionAimSlopeAng = NJM_DEG_ANG(20);
            } else {
                pFace->EyeLidExpressionAimCloseAng = 0;
                pFace->EyeLidExpressionAimSlopeAng = 0;
            }
		} break;
        case AL_EYE_NUM_ANGER:
            pFace->EyeLidExpressionAimCloseAng = NJM_DEG_ANG(75);
            pFace->EyeLidExpressionAimSlopeAng = NJM_DEG_ANG(20);
            break;
    }

    switch (AL_FaceGetEyeCurrNum(tp)) {
        case AL_EYE_NUM_NORMAL:
        case AL_EYE_NUM_TRON:
        case AL_EYE_NUM_ANGER:
            texid = EyeColorTexIdList[pFace->EyeColorNum];
            break;
        default:
            texid = AL_EyeTexIdList[EyeNum];
            break;
    }

	// check for patch by ChaosEyes code
	if (byte_0053A5BC != 0xFF) {
		switch (GET_CHAOPARAM(tp)->type) {
			case TYPE_N_CHAOS:
			case TYPE_H_CHAOS:
			case TYPE_D_CHAOS:
				if (AL_FaceGetEyeCurrNum(tp) == AL_EYE_NUM_SUYASUYA) {
					texid = AL_EyeTexIdList[EyeNum];
				}
				else {
					texid = EyeColorTexIdList[pFace->EyeColorNum];
				}

				break;
		}
	}

	// TODO: refactor once we rewrite EyeColor for IDs
	//alignment 
	switch (GET_CWEPARAM(tp)->EyeAlignment) {
		case 1:
			switch (texid) {
				case 8:
				case 9:
					texid = 0;
					break;
				case 11:
				case 12:
					texid = 10;
					break;
			}
			break;

		case 2:
			switch (texid) {
				case 0:
				case 8:
					texid = 9;
					break;
				case 10:
				case 12:
					texid = 11;
					break;
			}
			break;

		case 3:
			switch (texid) {
				case 0:
				case 9:
					texid = 8;
					break;
				case 10:
				case 11:
					texid = 12;
					break;
			}
			break;
	}

	if (GET_CHAOPARAM(tp)->body.FormNum != AL_FORM_OMOCHAO)  {
		//colors
		if (GET_CWEPARAM(tp)->EyeColor) {
			pFace->Flag = 1;

			switch (texid) {
				case 0:
					texid = 0;
					break;
				case 5:
					texid = 1;
					break;
				case 8:
					texid = 2;
					break;
				case 9:
					texid = 3;
					break;
				case 10:
					texid = 4;
					break;
				case 11:
					texid = 5;
					break;
				case 12:
					texid = 6;
					break;
				default:
					pFace->Flag = 0;
					break;
			}
		}
		else {
			pFace->Flag = 0;
		}
	}
	else {
		switch (texid) {
			case 10:
				texid = 0;
				break;
			case 11:
				texid = 9;
				break;
			case 12:
				texid = 8;
				break;
		}
	}

	AL_MatChangeObjectTexture(pFace->pEyeObject[0], texid);
    AL_MatChangeObjectTexture(pFace->pEyeObject[1], texid);
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