#include "stdafx.h"
#include "al_daynight_rain.h"
#include <ninja_functions.h>
#include <random>
#include <memory.h>
#include <util.h>
#include <al_texlist.h>
#include <al_omochao_build.h>
#include "ChaoMain.h"

#pragma pack(push, 8)
struct SA2CAMERADATA
{
	int type;
	int dunno;
	int dunno1;
	NJS_VECTOR Position;
	Rotation Rotation;
	NJS_VECTOR field_24;
	NJS_VECTOR Target;
	float distance;
};
#pragma pack(pop)

#define FALL_FRAMES 35

#define GET_WORK(tp) ((RAIN_WORK*)tp->Data2.Undefined)

static char
SE_Call_Timer(int tone, const void* id, int pri, int volofs, int timer)
{
	static const void* const ___fptr = ((void*)0x004374D0);

	char result;

	__asm
	{
		push[timer]
		push[volofs]
		push[pri]
		push[id]
		mov edi, [tone]
		call[___fptr]
		add esp, 16
		mov[result], al
	}

	return result;
}

enum CWE_SOUND {
	CWE_SOUND_ACCORDION = 161,
	CWE_SOUND_GUITAR = 162,
	CWE_SOUND_TRIANGLE = 163,
	CWE_SOUND_SAND = 164,
	CWE_SOUND_RAIN = 165
};

static void AL_DayNightRainExecutor(task* tp) {
	auto* work = GET_WORK(tp);

	if(gConfigVal.DayNightRainSounds) {
		SE_Call_Timer(0x6000 + CWE_SOUND_RAIN, tp, 0, 0, 2);
	}

	// slightly change angle overtime
	work->angle = lerp(work->startAngle, work->targetAngle, work->timer / float(work->timerLimit));

	if (work->timer++ >= work->timerLimit) {
		DeleteObject_(tp);
		return;
	}

	size_t maxRainSpawnCount = 32;

	SA2CAMERADATA* cameraData = (SA2CAMERADATA*)0x1DCFF00;
	const auto& cameraPos = cameraData->Position;
	NJS_VECTOR targetVec = {
		.x = cameraData->Target.x - cameraData->Position.x,
		.y = cameraData->Target.y - cameraData->Position.y,
		.z = cameraData->Target.z - cameraData->Position.z
	};
	njUnitVector(&targetVec);

	for (size_t i = 0; i < _countof(work->splashes); ++i) {
		auto& splash = work->splashes[i];
		
		if (!(splash.scl--)) {
			splash.scl = 0;
		}
	}

	for (size_t i = 0; i < DROP_COUNT && maxRainSpawnCount; i++) {
		auto& drop = work->drops[i];

		if (drop.lifeCount) {
			const float fallSpeed = 0.5f + (drop.scale / 100.f) * (0.85f); //0.85f + (drop.scale / 100.f) * 0.4f;
			drop.progress += fallSpeed / float(FALL_FRAMES);

			if (drop.progress >= 1) {
				for (size_t j = 0; j < _countof(work->splashes); ++j) {
					auto& splash = work->splashes[j];
					if (splash.scl) continue;

					splash.pos = drop.startPos;
					splash.pos.y = drop.collisionY + 0.1f;
					splash.ang[0] = drop.ang[0];
					splash.ang[1] = drop.ang[1] + NJM_DEG_ANG(180 - njRandom() * 90);
					splash.ang[2] = drop.ang[2];
					splash.scl = 3 + Uint16(njRandom() * 10.f);

					break;
				}

				drop.lifeCount--;
				drop.progress = 0;
			}

			continue;
		}

		maxRainSpawnCount--;

		const Angle ang = NJM_DEG_ANG(njRandom() * 360.f);
		const float radius = njRandom() * 100.f;

		drop.startPos.x = cameraPos.x + targetVec.x * 10.f + (-15.f + njRandom() * 30.f) + njSin(ang) * radius;
		drop.startPos.y = cameraPos.y + 100 + njRandom() * 100;
		drop.startPos.z = cameraPos.z + targetVec.z * 10.f + (-15.f + njRandom() * 30.f) + njCos(ang) * radius;
		drop.scale = njRandom() * 100.f;

		{
			Rotation returnAng;
			drop.collisionY = CalculateFalloffPosition_(drop.startPos.x, MainCharObj1[0]->Position.y + 65.f, drop.startPos.z, &returnAng);

			drop.ang[0] = returnAng.x;
			drop.ang[1] = returnAng.y;
			drop.ang[2] = returnAng.z;
		}

		drop.lifeCount = Uint8(5 + njRandom() * 10);
	}
}

static void DrawDrops(task* tp) {
	auto* work = GET_WORK(tp);

	njSetTexture(&CWE_OBJECT_TEXLIST);
	njSetTextureNum(68);
	
	njColorBlendingMode(0, NJD_COLOR_BLENDING_ONE);
	njColorBlendingMode(1, NJD_COLOR_BLENDING_ONE);

	for (size_t i = 0; i < _countof(work->splashes); ++i) {
		const auto& splash = work->splashes[i];
		if (!splash.scl) continue;

		NJS_TEXTURE_VTX verts[4];

		const auto col = (work->color & 0x00FFFFFF) | (0xFF << 24);
		const float scl = 3 * splash.scl / 10.f;

		verts[0] = { 0, 0, 0, 0, 0, col };
		verts[1] = { 0 + scl, 0, 0, 1, 0, col };
		verts[2] = { 0, 0, 0 + scl, 0, 1, col };
		verts[3] = { 0 + scl, 0, 0 + scl, 1, 1, col };

		for (size_t v = 0; v < 4; ++v) {
			verts[v].x -= scl / 2.f;
			verts[v].z -= scl / 2.f;
		}

		njPushMatrixEx();
		njTranslateEx((NJS_VECTOR*) & splash.pos);
		RotateZ(splash.ang[2]);
		RotateY(splash.ang[1]);
		RotateX(splash.ang[0]);
		njDrawTexture3DExSetData(verts, 4, 0);
		njPopMatrixEx();
	}
}

static void AL_DayNightRainDisplayer(task* tp) {
	DrawDrops(tp);

	njColorBlendingMode(0, 8);
	njColorBlendingMode(1, 10);

	static NJS_TEXTURE_VTX drops[DROP_COUNT * 7];
	size_t dropCount = 0;

	auto* work = GET_WORK(tp);

	const float dropWidth = 0.1f;
	const float fallRadius = 0.25f;

	const float angOffsetX = njSin(work->angle);
	const float angOffsetZ = njCos(work->angle);

	const NJS_POINT2 dropOffsets[] = {
		{ 0, 0 },
		{ 1, 0 },
		{ 0, 1 },
		{ 1, 1 }
	};

	for (size_t i = 0; i < DROP_COUNT; i++) {
		const auto& drop = work->drops[i];
		if (!drop.lifeCount) continue;

		const float sclVar = drop.scale / 100.f;
		const Uint8 alpha = Uint8(0xFF * (0.85f + sclVar * 0.25f));

		const float progressX = lerp(drop.startPos.x + angOffsetX * fallRadius, drop.startPos.x, drop.progress);
		const float progressY = lerp(drop.startPos.y, drop.collisionY, drop.progress);
		const float progressZ = lerp(drop.startPos.z + angOffsetZ * fallRadius, drop.startPos.z, drop.progress);

		if (dropCount > 0) {
			if ((dropCount % 2) == 1) {
				drops[dropCount] = drops[dropCount - 1];
				dropCount++;
			}

			drops[dropCount] = drops[dropCount - 1];
			dropCount++;
		}

		for (size_t vtx = 0; vtx < 4; vtx++) {
			auto& visualDrop = drops[dropCount];

			// all these random 0-1 floats are just random magic numbers based on feel
			// the sclVar ones are intended to fit a range, but the base number used was just picked on feel and adjusted a bit
			visualDrop.x = progressX + dropOffsets[vtx].x * dropWidth * (0.65f + sclVar * 0.35f);
			visualDrop.y = progressY + dropOffsets[vtx].y * (0.5f + sclVar * 0.8f);
			visualDrop.z = progressZ;

			visualDrop.x += angOffsetX * dropOffsets[vtx].y * fallRadius * 0.75f;
			visualDrop.z += angOffsetZ * dropOffsets[vtx].y * fallRadius * 0.75f;

			visualDrop.u = dropOffsets[vtx].x;
			visualDrop.v = dropOffsets[vtx].y;

			visualDrop.col = (work->color & 0x00FFFFFF) | (alpha << 24);

			dropCount++;

			if (vtx == 0 && dropCount > 1) {
				drops[dropCount] = drops[dropCount - 1];
				dropCount++;
			}
		}
	}

	if (!dropCount) return;

	njSetTexture(&CWE_OBJECT_TEXLIST);
	njSetTextureNum(67);
	njDrawTexture3DExSetData(drops, dropCount, 0);
}

task* AL_CreateDayNightRain(Uint32 timer, Uint32 color) {
	task* tp = LoadObject(4, "AL_DayNightRain", AL_DayNightRainExecutor, LoadObj(0));

	tp->DisplaySub = AL_DayNightRainDisplayer;

	RAIN_WORK* work = ALLOC(RAIN_WORK);
	tp->Data2.Undefined = work;

	memset(work, 0, sizeof(RAIN_WORK));

	const float maxAngleChange = 40.f;
	work->angle = 0;
	work->startAngle = NJM_DEG_ANG(njRandom() * (360.f - maxAngleChange));
	work->targetAngle = work->angle + NJM_DEG_ANG(-maxAngleChange + njRandom() * maxAngleChange * 2.f);

	work->color = color;
	work->timerLimit = timer;

	return tp;
}