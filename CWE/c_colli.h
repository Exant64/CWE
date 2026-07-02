#pragma once

enum CI_FORM {
    CI_FORM_SPHERE = 0x0,
    CI_FORM_CYLINDER = 0x1,
    CI_FORM_CYLINDER2 = 0x2,
    CI_FORM_RECTANGLE = 0x3,
    CI_FORM_RECTANGLE2 = 0x4,
    CI_FORM_RECTANGLE3 = 0x5,
    CI_FORM_CAPSULE = 0x6,
    CI_FORM_PERSON = 0x7,
    CI_FORM_LINE = 0x8,
    CI_FORM_WALL_PLANE = 0x9,
    CI_FORM_WALL_CIRCLE = 0xA,
};

struct c_colli_hit_info {
	char my_num;
	char hit_num;
	unsigned __int16 flag;
	task* hit_twp;
};

struct CCL_INFO {
	char kind;
	char form;
	char push;
	char damage;
	int attr;
	NJS_POINT3 center;
	float a;
	float b;
	float c;
	float d;
	int angx;
	int angy;
	int angz;
};

#define CCL_SPHERE(kind, push, damage, attr, cx, cy, cz, a, b, c) \
    { kind, CI_FORM_SPHERE, push, damage, attr, { cx, cy, cz }, a, b, c, 0, 0, 0, 0 }
#define CCL_CYLINDER(kind, push, damage, attr, cx, cy, cz, a, b, c) \
    { kind, CI_FORM_CYLINDER, push, damage, attr, { cx, cy, cz }, a, b, c, 0, 0, 0, 0 }

void CCL_Enable(task* a1, int a2);
void CCL_Disable(task* a1, int a2);

FunctionPointer(c_colli_hit_info*, CCL_IsHitKindEx,(task* a1, unsigned __int8 a2),0x00486760);

static const void* const AddToCollisionListPtr = (void*)0x47E750;
static inline void CCL_Entry(task* object)
{
	__asm
	{
		mov esi, [object]
		call AddToCollisionListPtr
	}
}

task* AL_IsHitKindWithNum(task *a1, int a2, Uint8 a3);