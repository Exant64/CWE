#pragma once
#pragma warning( push )
#pragma warning( disable : 4309 )
#pragma warning( disable : 4305 )
#pragma warning( disable : 4838 )

#include "alm_sunaasubi.nam"
#include "alm_triangle.nam"
#include "alm_accordion.nam"
#include "alm_tailsfly.nam"
#include "alm_dance_n.nam"
#include "alm_dance_h.nja"
#include "alm_dance_d.nam"
#include "alm_guitar.nam"
#include "alm_floaty.nam"
#include "alm_duck.h"
#include "alm_egg_crawl.nam"
#include "alm_wrench.nam"

MotionTableAction alm_sunaasubi = { &animation_00BE0FF4,   1, 0, 0xFFFFFFFF, 0xFFFFFFD8, 0, 39.0f, 0.12f };
MotionTableAction alm_tailsfly = { &motion_alm_tailsfly,  0, 0, 0xFFFFFFFF, 0xFFFFFFD8, 0, 30.0f, 0.12f };
MotionTableAction alm_triangle = { &motion_alm_triangle,  0, 0, 0xFFFFFFFF, 0xFFFFFFD8, 0, 15.0f, 0.12f };
MotionTableAction alm_accordion = { &motion_alm_accordion, 2, 0, 0xFFFFFFFF, 0xFFFFFFD8, 0, 8.0f, 0.12f };
MotionTableAction alm_tailsfly_tired = { &motion_alm_tailsfly,  0, 0, 0xFFFFFFFF, 0xFFFFFFD8, 0, 30.0f, 0.12f / 2 };
MotionTableAction alm_dance_n = { &animation_00C3025C ,  1, 0, 0xFFFFFFFF, -40, 0.0f, 25.0f, 0.12f };
MotionTableAction alm_dance_h = { &motion_alm_dance_h ,  1, 0, 0xFFFFFFFF, -40, 0.0f, 30.0f, 0.12f };
MotionTableAction alm_dance_d = { &motion_alm_dance_d,	 1, 0, 0xFFFFFFFF, -40, 0.0f, 34.0f, 0.12f };
MotionTableAction alm_guitar = { &motion_alm_guitar ,   1, 0, 0xFFFFFFFF, -40, 0.0f, 10.0f, 0.12f * 2 };
MotionTableAction alm_float = { &motion_alm_float  ,   1, 0, 0xFFFFFFFF, -40, 0.0f, 14.0f, 0.12f * 2 };
MotionTableAction alm_duck = { &motion_alm_duck  ,    1, 0, 0xFFFFFFFF, -40, 0.0f, 13.0f, 0.12f * 2 };
MotionTableAction alm_wrench_start = { &motion_alm_wrench,    6, 0, 0xFFFFFFFF, -40, 9, 18, 0.15 };
MotionTableAction alm_wrench_start2 = { &motion_alm_wrench,    6, 0, 0xFFFFFFFF, -40, 18, 28, 0.15 };
MotionTableAction alm_wrench = { &motion_alm_wrench,    7, 0, 0xFFFFFFFF, -40, 28, 36, 0.15 };
MotionTableAction alm_egg_crawl = { &motion_alm_egg_crawl, 2, 0, 0xFFFFFFFF, -40, 2.0f, 13.0f, 0.1f };
MotionTableAction alm_egg_crawl_start = { &motion_alm_egg_crawl, 6, 0, 0xFFFFFFFF, -40, 0,    2.0f,  0.05f };

#pragma warning( pop )