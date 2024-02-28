#pragma once
#include "alo_accessory.h"

enum 
{
	AL_PART_ROOT = 0x0,
	AL_PART_BODY = 0x1,
	AL_PART_ARM_L_POS = 0x2,
	AL_PART_ARM_L = 0x3,
	AL_PART_HAND_L = 0x4,
	AL_PART_REG_L_POS = 0x5,
	AL_PART_REG_L = 0x6,
	AL_PART_TAIL_POS = 0x7,
	AL_PART_TAIL = 0x8,
	AL_PART_ARM_R_POS = 0x9,
	AL_PART_ARM_R = 0xA,
	AL_PART_HAND_R = 0xB,
	AL_PART_REG_R_POS = 0xC,
	AL_PART_REG_R = 0xD,
	AL_PART_NECK = 0xE,
	AL_PART_HEAD_POS = 0xF,
	AL_PART_HEAD = 0x10,
	AL_PART_EYE_L_POS = 0x11,
	AL_PART_EYE_L = 0x12,
	AL_PART_EYELID_L = 0x13,
	AL_PART_EYE_R_POS = 0x14,
	AL_PART_EYE_R = 0x15,
	AL_PART_EYELID_R = 0x16,
	AL_PART_FEELER_L = 0x17,
	AL_PART_EAR_L = 0x18,
	AL_PART_FEELER_R = 0x19,
	AL_PART_EAR_R = 0x1A,
	AL_PART_MOUTH = 0x1B,
	AL_PART_TONGUE_POS = 0x1C,
	AL_PART_FOREHEAD = 0x1D,
	AL_PART_HORN_L = 0x1E,
	AL_PART_HORN_R = 0x1F,
	AL_PART_OFFICE_POS = 0x20,
	AL_PART_OFFICE = 0x21,
	AL_PART_BACK = 0x22,
	AL_PART_NAVEL = 0x23,
	AL_PART_WING_L_POS = 0x24,
	AL_PART_WING_L = 0x25,
	AL_PART_WING_R_POS = 0x26,
	AL_PART_WING_R = 0x27,
	AL_PART_END = 0x28,
};


struct AL_SURFACE
{
	unsigned char Color;
	unsigned char Shiny;
	unsigned char Monotone;
	unsigned char Jewel;
	unsigned char ShinyJewelMonotone;
};

enum ChunkMaterialFlagsEnum
{
	DontUseTexture = 0x1,
	HasSecondTexture = 0x2,
	SecondTextureEnvironmentMap = 0x4,
	UseChunkObjectColor = 0x8,
};

void AL_RenderAccessory(ObjectMaster* a1, EAccessoryType type);
void AL_SetMotionMatrix(ObjectMaster* a1, ChunkObjectPointer* ptr);
void AL_ValidateAccessory(ObjectMaster* a1, EAccessoryType type);
void AL_RenderRigAccessory(ObjectMaster* a1, ChunkObjectPointer* chunkObjectPointer, EAccessoryType type);
void AL_Draw_Init();
extern int AccessoryNodeIndex;
void AnimateChao(int a1);
void sub_56E9C0(ObjectMaster* a1);
void DrawAccessory(ObjectMaster* a1, ChunkObjectPointer* chunkObjectPointer, NJS_OBJECT* traverseAccessory);
void ChaoColoring(int texture, int color, int shiny, int highlights, int shinyJewelMonotone, NJS_CNK_MODEL* model);

void AL_SetBodyTexture(ObjectMaster* a1);
void AL_SetRareMaterial(ObjectMaster* a1, NJS_CNK_MODEL* model);

void __cdecl DrawEggChao(ObjectMaster* a1);
void AL_DrawToy(ChunkObjectPointer* chunkObjectPointer);

DataPointer(unsigned char, CharacterIndex, 0x540212);
DataArray(int, dword_1298448, 0x01298448, 20);
DataArray(NJS_OBJECT*, dword_12E58B8, 0x12E58B8, 12);
DataPointer(int, Chao_NodeIndex, 0x01A259D8);
DataPointer(int, ChunkMatEnable, 0x01AED2CC);
DataPointer(int, ChunkMatFlag, 0x01A27594);
DataPointer(int, ChunkObjectColor, 0x01A275A4);