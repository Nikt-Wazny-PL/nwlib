#pragma once

#include "fwd_utils.h"
#include <cstdint>

namespace nw::geo {

template<uint32_t C, uint32_t R, typename T>
struct matrix;

template<typename T> using matrix1   = matrix<1, 1, T>;
template<typename T> using matrix1x1 = matrix<1, 1, T>;
template<typename T> using matrix1x2 = matrix<1, 2, T>;
template<typename T> using matrix1x3 = matrix<1, 3, T>;
template<typename T> using matrix1x4 = matrix<1, 4, T>;

template<typename T> using matrix2   = matrix<2, 2, T>;
template<typename T> using matrix2x1 = matrix<2, 1, T>;
template<typename T> using matrix2x2 = matrix<2, 2, T>;
template<typename T> using matrix2x3 = matrix<2, 3, T>;
template<typename T> using matrix2x4 = matrix<2, 4, T>;

template<typename T> using matrix3   = matrix<3, 3, T>;
template<typename T> using matrix3x1 = matrix<3, 1, T>;
template<typename T> using matrix3x2 = matrix<3, 2, T>;
template<typename T> using matrix3x3 = matrix<3, 3, T>;
template<typename T> using matrix3x4 = matrix<3, 4, T>;

template<typename T> using matrix4   = matrix<4, 4, T>;
template<typename T> using matrix4x1 = matrix<4, 1, T>;
template<typename T> using matrix4x2 = matrix<4, 2, T>;
template<typename T> using matrix4x3 = matrix<4, 3, T>;
template<typename T> using matrix4x4 = matrix<4, 4, T>;

NWGEO_DEFINE_SUBTYPES(matrix1);
NWGEO_DEFINE_SUBTYPES(matrix1x1);
NWGEO_DEFINE_SUBTYPES(matrix1x2);
NWGEO_DEFINE_SUBTYPES(matrix1x3);
NWGEO_DEFINE_SUBTYPES(matrix1x4);

NWGEO_DEFINE_SUBTYPES(matrix2);
NWGEO_DEFINE_SUBTYPES(matrix2x1);
NWGEO_DEFINE_SUBTYPES(matrix2x2);
NWGEO_DEFINE_SUBTYPES(matrix2x3);
NWGEO_DEFINE_SUBTYPES(matrix2x4);

NWGEO_DEFINE_SUBTYPES(matrix3);
NWGEO_DEFINE_SUBTYPES(matrix3x1);
NWGEO_DEFINE_SUBTYPES(matrix3x2);
NWGEO_DEFINE_SUBTYPES(matrix3x3);
NWGEO_DEFINE_SUBTYPES(matrix3x4);

NWGEO_DEFINE_SUBTYPES(matrix4);
NWGEO_DEFINE_SUBTYPES(matrix4x1);
NWGEO_DEFINE_SUBTYPES(matrix4x2);
NWGEO_DEFINE_SUBTYPES(matrix4x3);
NWGEO_DEFINE_SUBTYPES(matrix4x4);

}
