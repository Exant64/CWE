#pragma once
enum DrawAnchorH
{
	DrawAncorV_Left = 0xFFFFFFFF,
	DrawAncorV_Center = 0x0,
	DrawAncorV_Right = 0x1,
};

enum DrawAnchorV
{
	DrawAncorV_Top = 0xFFFFFFFF,
	DrawAncorV_Middle = 0x0,
	DrawAncorV_Bottom = 0x1,
};

void DisplayChaoName_NewFont(char* name, float xpos, float ypos, float xsize, float ysize, NJS_COLOR col, int FreeStrlen, DrawAnchorH ancH);
void AL_Name_Init();