#pragma once

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++11-narrowing"
#pragma clang diagnostic ignored "-Wconstant-conversion"
#else
#pragma warning(push)
#pragma warning( disable: 4838 )
#endif

#include "HyperSwim.h"
#include "HyperFly.h"
#include "HyperRun.h"
#include "HyperPower.h"
#include "ShinyFruit.h"
#include "alo_missing.nja"
#include "alo_cakeslice.nja"
#include "ali_orange.nja"

#if defined(__clang__)
#pragma clang diagnostic pop
#else
#pragma warning (pop)
#endif