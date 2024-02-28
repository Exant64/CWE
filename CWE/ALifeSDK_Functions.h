#pragma once



void sub_534F80(int a1, NJS_VECTOR* a2, signed int a3);
void sub_561740(int a1);

void SetShaders(int result);


ObjectMaster* ALO_Field_Find_(ObjectMaster* a1, int a2, Uint8 a3);

struct c_colli_hit_info
{
	char my_num;
	char hit_num;
	unsigned __int16 flag;
	ObjectMaster* hit_twp;
};
FunctionPointer(c_colli_hit_info*, CCL_IsHitKindEx,(ObjectMaster* a1, unsigned __int8 a2),0x00486760);


int ScaleObjectMaster_XYZ(ObjectMaster* a1, float a2, float a3, float a4);

static const void* const AddToCollisionListPtr = (void*)0x47E750;
static inline void AddToCollisionList(ObjectMaster* object)
{
	__asm
	{
		mov esi, [object]
		call AddToCollisionListPtr
	}
}

void DrawToyTransform(ObjectMaster *a1, int a2, int a3);

void sub_5669B0(ObjectMaster* a1, int a2, int a3);


void PlayJingle(const char *a1);