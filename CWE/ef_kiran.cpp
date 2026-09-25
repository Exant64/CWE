#include "stdafx.h"
#include "particlecore.h"

static Bool KiranExecutor(ParticleUserData *a1, ParticleData *a2) {
	const int frameTbl[] = { 0, 1, 2, 3, 2, 1, 0 };

    a2->frame = float(frameTbl[NJM_MIN(_countof(frameTbl) - 1, size_t(a2->velocity.y))]);
    a2->velocity.y += a2->velocity.x / 2.f;

    return Bool((int)a2->velocity.y < a1->frames);
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