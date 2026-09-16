#pragma once

#include "code_system/cwe_code.h"

#include <Chao.h>
#include "al_face.h"

ONFRAME_CODE(ChaosEyes);
PATCH_CODE(ChaosMouths);

static void AL_FaceSetEye_r(task* tp, int EyeNum, int timer) {
    AL_FACE_CTRL* pFace = &GET_CHAOWK(tp)->Face;

    pFace->EyeCurrNum = pFace->EyeDefaultNum = GET_CHAOPARAM(tp)->body.DefaultEyeNum;

    AL_FaceChangeEye(tp, pFace->EyeCurrNum);
}

static void ASM_FUNC AL_FaceSetEye_t() {
    ASM_PUSH(eax);
    ASM_PUSH(edx);
    ASM_PUSH(ecx);

    ASM_CALL(AL_FaceSetEye_r);

    ASM_POP(ecx);
    ASM_POP(edx);
    ASM_POP(eax);

    ASM_RET(0);
}

static void AL_FaceSetMouth_r(task* tp, int MouthNum, int timer) {
    AL_FACE_CTRL* pFace = &GET_CHAOWK(tp)->Face;

    pFace->MouthCurrNum = pFace->MouthDefaultNum = GET_CHAOPARAM(tp)->body.DefaultMouthNum;

    AL_FaceChangeMouth(tp, pFace->MouthCurrNum);
}

static void ASM_FUNC AL_FaceSetMouth_t() {
    ASM_PUSH(edx);
    ASM_PUSH(ASM_ESP(2));
    ASM_PUSH(ecx);

    ASM_CALL(AL_FaceSetMouth_r);

    ASM_POP(ecx);
    ASM_ESP_ADD(1);
    ASM_POP(edx);

    ASM_RET(0);
}

void ChaosEyes::OnFrame() {
    if(!m_param) return;

    WriteData((unsigned char*)0x0053A54B, (unsigned char)0xFF);

    if (m_param == 2) {
        //WriteCall((void*)0x0053A1B5, (void*)AL_FaceSetEye_t);
    }
}

void ChaosMouths::Init() {
    if(!m_param) return;

    WriteData((unsigned char*)0x0053A5BC, (unsigned char)0xFF);

    if (m_param == 2) {
        //WriteCall((void*)0x0053A1C7, (void*)AL_FaceSetMouth_t);
    }
}