#pragma once

#include "fwd_utils.h"

namespace nw::geo {

template<uint32_t N, typename T> struct vector;
template<typename T> using vector1 = vector<1, T>;
template<typename T> using vector2 = vector<2, T>;
template<typename T> using vector3 = vector<3, T>;
template<typename T> using vector4 = vector<4, T>;

NWGEO_DEFINE_SUBTYPES(vector1);
NWGEO_DEFINE_SUBTYPES(vector2);
NWGEO_DEFINE_SUBTYPES(vector3);
NWGEO_DEFINE_SUBTYPES(vector4);

}
