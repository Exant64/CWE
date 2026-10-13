#pragma once

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++11-narrowing"
#pragma clang diagnostic ignored "-Wconstant-conversion"
#else
#pragma warning(push)
#pragma warning( disable: 4838 )
#pragma warning( disable : 4309 )
#pragma warning( disable : 4305 )
#endif

#include "als_dcwings.nja"
#include "als_mirror.nja"
#include "als_brush_hn.nja"
#include "als_brush_hs.nja"
#include "als_brush_hf.nja"
#include "als_brush_hr.nja"
#include "als_brush_hp.nja"
#include "als_bottle.nja"
#include "als_box.nja"

#if defined(__clang__)
#pragma clang diagnostic pop
#else
#pragma warning (pop)
#endif