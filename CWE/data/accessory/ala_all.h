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

#include "ala_glasses_round.nja"
#include "ala_glasses_square.nja"
#include "ala_bowtie_red.nja"
#include "ala_baseballcap.nja"
#include "ala_glasses_aviators.nja"
#include "ala_headphones.nja"
#include "ala_pacifier.nja"
#include "ala_presshat.nja"
#include "ala_hoodie.nja"
#include "ala_hoodie_pink.nja"
#include "ala_neckbowtie.nja"
#include "ala_hoodie_low.nja"
#include "ala_hoodie_pink_low.nja"

#if defined(__clang__)
#pragma clang diagnostic pop
#else
#pragma warning (pop)
#endif