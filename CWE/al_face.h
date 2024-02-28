#pragma once
void AL_FaceInit();
void  SetChunkTextureID_(NJS_CNK_MODEL* a1, short a2);
void Chao_MoreFaceExpression(ObjectMaster* a1);
void AL_FaceSetEyeCWE(ObjectMaster* a3, int a2, int a1);

void AL_FaceSetMouth(ObjectMaster* a2, int a3, int a1);
void AL_FaceSetEye(ObjectMaster* a3, int a2, int a1);

void AL_FaceChangeEye(ObjectMaster* tp, int EyeNum);
void AL_FaceChangeMouth(ObjectMaster* tp, int MouthNum);