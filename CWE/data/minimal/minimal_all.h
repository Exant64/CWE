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

#include "bee.mini"
#include "lady.mini"
#include "scorpion.mini"

#if defined(__clang__)
#pragma clang diagnostic pop
#else
#pragma warning (pop)
#endif