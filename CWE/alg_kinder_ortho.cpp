#include "stdafx.h"

#include "alg_kinder_ortho.h"

const int WcConvFromCStrPtr = 0x0057A680;
void WcConvFromCStr(int result, int a2, signed int a3)
{
    __asm
    {
        mov eax, result
        mov edx, a2
        mov ecx, a3
        call WcConvFromCStrPtr
    }
}

#pragma optimize( "gty", off )
const int DrawChaoHudThingBptr = 0x00558BE0;
void DrawChaoHudThingB(ChaoHudThingB* a1, float a2, float a3, float a4, float a5, float a6, int a7, int a8)
{
    __asm
    {
        push [a8]
        push [a7]
        push [a6]
        push [a5]
        push [a4]
        push [a3]
        push [a2]
        mov eax, a1
        call DrawChaoHudThingBptr
        add esp, 7 * 4
    }
}
#pragma optimize( "gty", on )

const int DrawChaoHudThingPtr = 0x0583F50;
void DrawChaoHudThing(ChaoHudThing* a1, int a2)
{
	__asm
	{
		mov eax, a1
		mov ecx, a2
		call DrawChaoHudThingPtr
	}
}

void __cdecl AlgKinderOrthoQuadDrawArrayTileOffset(ChaoHudThing const* a4, int a5, unsigned int a6, int a7, int a8, int a9, __int16 a10)
{
    ChaoHudThing const* v7; // esi
    __int16 v8; // bx
    __int16 v9; // cx
    __int16 v10; // bp
    __int16 v11; // ax
    __int16 v12; // di
    __int16 v13; // bx
    __int16 v14; // cx
    __int16 v15; // dx
    __int16 v16; // cx
    bool v17; // zf
    __int16 v18; // di
    __int16 v19; // bx
    int v20; // [esp+0h] [ebp-1Ch]
    int v21; // [esp+4h] [ebp-18h]
    __int16 v22; // [esp+8h] [ebp-14h]
    ChaoHudThing a1; // [esp+Ch] [ebp-10h]
    int a5a; // [esp+24h] [ebp+8h]

    if (a5 > 0)
    {
        v7 = a4;
        v21 = a5;
        do
        {
            v8 = v7->x1;
            v9 = v7->y0;
            v10 = v8 - v7->x0;
            v11 = v7->y1 - v9;
            v12 = a9 + v7->x0;
            v13 = a9 + v8;
            a1.y0 = a10 + v9;
            v14 = v7->y1;
            v22 = v11;
            a1.x0 = v12;
            a1.x1 = v13;
            v15 = v7->u1;
            a1.y1 = a10 + v14;
            a1.u0 = v7->u0;
            a1.v0 = v7->v0;
            a1.u1 = v15;
            a1.v1 = v7->v1;
            if (a8 > 0)
            {
                v20 = a8;
                v16 = a9;
                do
                {
                    if (a7 > 0)
                    {
                        a5a = a7;
                        do
                        {
                            DrawChaoHudThing(&a1, a6);
                            v12 += v10;
                            v13 += v10;
                            v17 = a5a == 1;
                            a1.x0 = v12;
                            a1.x1 = v13;
                            --a5a;
                        } while (!v17);
                        v16 = a9;
                        v11 = v22;
                    }
                    v18 = v7->x0;
                    v19 = v7->x1;
                    a1.y0 += v11;
                    a1.y1 += v11;
                    v12 = v16 + v18;
                    v13 = v16 + v19;
                    v17 = v20 == 1;
                    a1.x0 = v12;
                    a1.x1 = v13;
                    --v20;
                } while (!v17);
            }
            ++v7;
            --v21;
        } while (v21);
    }
}
void __cdecl AlgKinderOrthoQuadDrawArrayOffset(ChaoHudThing const* a3, int count, unsigned int color, __int16 oX, __int16 oY)
{
    for(int i = 0; i < count; i++)
    {
        ChaoHudThing a1;
        const ChaoHudThing* v6 = &a3[i];
        a1.x0 = oX + v6->x0;
        a1.y0 = oY + v6->y0;
        a1.x1 = oX + v6->x1;
        a1.y1 = oY + v6->y1;
        a1.u0 = v6->u0;
        a1.u1 = v6->u1;
        a1.v0 = v6->v0;
        a1.v1 = v6->v1;
        DrawChaoHudThing(&a1, color);
    }
}

void __cdecl AlgKinderOrthoQuadDrawArrayTile(ChaoHudThing* a3, int count, Uint32 color, int nx, int ny)
{
    ChaoHudThing* v5; // edi
    __int16 v6; // si
    __int16 v7; // bx
    int v8; // ecx
    int v9; // ebp
    __int16 v10; // dx
    bool v11; // zf
    int v12; // [esp+0h] [ebp-14h]
    ChaoHudThing a1; // [esp+4h] [ebp-10h]
    int a4a; // [esp+1Ch] [ebp+8h]

    if (count > 0)
    {
        v5 = a3;
        v12 = count;
        do
        {
            v6 = v5->x1 - v5->x0;
            v7 = v5->y1 - v5->y0;
            v8 = *(int*)&v5->y0;
            *(int*)&a1.x0 = *(int*)&v5->x0;
            *(int*)&a1.u0 = *(int*)&v5->u0;
            *(int*)&a1.y0 = v8;
            *(int*)&a1.v0 = *(int*)&v5->v0;
            if (ny > 0)
            {
                a4a = ny;
                do
                {
                    v9 = nx;
                    if (nx > 0)
                    {
                        do
                        {
                            DrawChaoHudThing(&a1, color);
                            a1.x0 += v6;
                            a1.x1 += v6;
                            --v9;
                        } while (v9);
                    }
                    a1.y0 += v7;
                    a1.y1 += v7;
                    v10 = v5->x1;
                    v11 = a4a == 1;
                    a1.x0 = v5->x0;
                    a1.x1 = v10;
                    --a4a;
                } while (!v11);
            }
            ++v5;
            --v12;
        } while (v12);
    }
}
void __cdecl AlgKinderOrthoQuadDrawArray(ChaoHudThing* q, int len, Uint32 color)
{
    for (int i = 0; i < len; i++) {
        DrawChaoHudThing(&q[i], color);
    }
}