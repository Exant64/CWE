#pragma once



void sub_534F80(int a1, NJS_VECTOR* a2, signed int a3);
void sub_561740(int a1);

void SetShaders(int result);

int AdjustAngle(__int16 bams_a, unsigned __int16 bams_b, int dang);
task* ALO_Field_Find_(task* a1, int a2, Uint8 a3);

struct c_colli_hit_info
{
	char my_num;
	char hit_num;
	unsigned __int16 flag;
	task* hit_twp;
};
FunctionPointer(c_colli_hit_info*, CCL_IsHitKindEx,(task* a1, unsigned __int8 a2),0x00486760);


int Scaletask_XYZ(task* a1, float a2, float a3, float a4);

static const void* const AddToCollisionListPtr = (void*)0x47E750;
static inline void AddToCollisionList(task* object)
{
	__asm
	{
		mov esi, [object]
		call AddToCollisionListPtr
	}
}

void DrawToyTransform(task *a1, int a2, int a3);

void sub_5669B0(task* a1, int a2, int a3);


void PlayJingle(const char *a1);