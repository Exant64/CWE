#pragma once

// thanks kell for the CI_ATTR entries!

#define CI_ATTR_NO_SEND        0x4      /* Do not send information to other collisions */
#define CI_ATTR_NO_RECEIVE     0x8      /* Do not receive information from other collisions */
#define CI_ATTR_IGNORE         0x10     /* Do not run physical collision check */
#define CI_ATTR_POS_ABSOLUTE   0x20     /* The position is no longer relative to the entity */
#define CI_ATTR_ANG_YZX        0x200    /* Use YZX rotation order instead of ZXY */
#define CI_ATTR_DAMAGE         0x400    /* Allow damage */
#define CI_ATTR_2000           0x2000   /* ? */
#define CI_ATTR_NO_PUSH        0x4000   /* Do not get pushed by other collisions */
#define CI_ATTR_ANG_ABSOLUTE   0x8000   /* The angle is no longer relative to the entity */
#define CI_ATTR_IGNORE_PLAYER  0x40000  /* Do not interact with player collisions */
#define CI_ATTR_IGNORE_ENEMY   0x80000  /* Do not interact with enemy collisions */
#define CI_ATTR_IGNORE_OBJECT  0x100000 /* Do not interact with object collisions */
#define CI_ATTR_IGNORE_ITEM    0x200000 /* Do not interact with item collisions */
#define CI_ATTR_IGNORE_CHAO    0x400000 /* Do not interact with chao collisions */
#define CI_ATTR_800000         0x800000 /* ? */

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