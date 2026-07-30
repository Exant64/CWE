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

#include "data/omochao/omobuild_arms.nja"
#include "data/omochao/omobuild_legs.nja"
#include "data/omochao/omobuild_eyel.nja"
#include "data/omochao/omobuild_eyer.nja"
#include "data/omochao/omobuild_prop.nja"
#include "data/omochao/omobuild_mouth.nja"
#include "data/omochao/omobuild_base.nja"

#if defined(__clang__)
#pragma clang diagnostic pop
#else
#pragma warning (pop)
#endif