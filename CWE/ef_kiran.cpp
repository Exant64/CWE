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

static ASM_FUNC int AllocateParticle(ParticleUserData* a2) {
    // save regs
    ASM_PUSH( ebx );

    // arguments
    ASM_MOVE( ebx, ASM_ESP(1+0+1) ); // a2

    // call
    ASM_CALL_R( edx, 0x0492660 );

    // restore regs
    ASM_POP( ebx );

    // return
    ASM_RET( 0 );
}

static bool KiranExecutor(ParticleUserData *a1, ParticleData *a2) {
	const int frameTbl[] = { 0, 1, 2, 3, 2, 1, 0 };

    a2->velocity.y += a2->velocity.x / 2.f;
    a2->frame = float(frameTbl[int(a2->velocity.y)]);

    return (int)a2->velocity.y < a1->frames;
}

static ParticleUserData KiranInfo = {
    1,
    (NJS_TEXLIST*)0xB09644,
    0,
    7,
    0.4f,
    0.98000002,
    -0.0099999998,
    0.0049999999,
    (void*)KiranExecutor,
    40000.0,
    0,
    0,
    0,
    0
};

void CreateKiran2 (NJS_POINT3 *pos, float scl, float spd) {
	ParticleData* particle = (ParticleData *)AllocateParticle(&KiranInfo);

	if (particle) {
		particle->position = *pos;
		particle->velocity = {spd};
		particle->scale = scl * 0.1f * 0x20;
		particle->rotY = 0;
		particle->rotYSpeed = 0;
		particle->filler5 = 0;
	}
}