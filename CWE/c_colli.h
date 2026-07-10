#pragma once

#define CCL_SPHERE(kind, push, damage, attr, cx, cy, cz, a, b, c) \
    { kind, CI_FORM_SPHERE, push, damage, attr, { cx, cy, cz }, a, b, c, 0, 0, 0, 0 }
#define CCL_CYLINDER(kind, push, damage, attr, cx, cy, cz, a, b, c) \
    { kind, CI_FORM_CYLINDER, push, damage, attr, { cx, cy, cz }, a, b, c, 0, 0, 0, 0 }

void CCL_Enable(task* a1, int a2);
void CCL_Disable(task* a1, int a2);

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