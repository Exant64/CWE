#pragma once

#include "stdafx.h"

struct ParticleUserData {
	int a1;
	NJS_TEXLIST *texlist;
	int a2;
	int frames;
	float speed;
	float velocityLoss;
	float gravity;
	float scaleUp;
	void *functionPtr;
	float a7;
	int fill1;
	int fill2;
	int active;
	int next;
};

struct ParticleData {
	int rotY;
	float scale;
	NJS_VECTOR position;
	int color;
	float frame;
	int field_1C;
	NJS_VECTOR velocity;
	int rotYSpeed;
	float filler3;
	float filler4;
	int filler5;
};

ASM_FUNC int AllocateParticle(ParticleUserData* a2);
