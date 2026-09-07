#pragma once

#include "utils_fwd.h"

#include <type_traits>

namespace nw::geo {

namespace detail {

template<size_t N, typename T> requires(std::is_arithmetic_v<T>)
struct vector_impl;

}

template<size_t N, typename T> requires(std::is_arithmetic_v<T>)
struct vector;

template<typename T = float> using vector1 = vector<1, T>;
template<typename T = float> using vector2 = vector<2, T>;
template<typename T = float> using vector3 = vector<3, T>;
template<typename T = float> using vector4 = vector<4, T>;

NWGEO_GENERATE_SUBTYPES(vector1);
NWGEO_GENERATE_SUBTYPES(vector2);
NWGEO_GENERATE_SUBTYPES(vector3);
NWGEO_GENERATE_SUBTYPES(vector4);

}
