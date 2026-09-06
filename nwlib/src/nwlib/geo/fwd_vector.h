#pragma once

#include <cstdint>

namespace nw::geo {

template<uint32_t N, typename T>
struct vector;

template<typename T>
using vector1    = vector<1, T>;
using fvector1   = vector<1, float>;
using dvector1   = vector<1, double>;
using uvector1   = vector<1, uint32_t>;
using ivector1   = vector<1, int32_t>;
using bvector1   = vector<1, bool>;
using f64vector1 = vector<1, double>;
using f32vector1 = vector<1, float>;
using u64vector1 = vector<1, uint64_t>;
using i64vector1 = vector<1, int64_t>;
using u32vector1 = vector<1, uint32_t>;
using i32vector1 = vector<1, int32_t>;
using u16vector1 = vector<1, uint16_t>;
using i16vector1 = vector<1, int16_t>;
using u8vector1  = vector<1, uint8_t>;
using i8vector1  = vector<1, int8_t>;

template<typename T>
using vector2    = vector<2, T>;
using fvector2   = vector<2, float>;
using dvector2   = vector<2, double>;
using uvector2   = vector<2, uint32_t>;
using ivector2   = vector<2, int32_t>;
using bvector2   = vector<2, bool>;
using f64vector2 = vector<2, double>;
using f32vector2 = vector<2, float>;
using u64vector2 = vector<2, uint64_t>;
using i64vector2 = vector<2, int64_t>;
using u32vector2 = vector<2, uint32_t>;
using i32vector2 = vector<2, int32_t>;
using u16vector2 = vector<2, uint16_t>;
using i16vector2 = vector<2, int16_t>;
using u8vector2  = vector<2, uint8_t>;
using i8vector2  = vector<2, int8_t>;

template<typename T>
using vector3    = vector<3, T>;
using fvector3   = vector<3, float>;
using dvector3   = vector<3, double>;
using uvector3   = vector<3, uint32_t>;
using ivector3   = vector<3, int32_t>;
using bvector3   = vector<3, bool>;
using f64vector3 = vector<3, double>;
using f32vector3 = vector<3, float>;
using u64vector3 = vector<3, uint64_t>;
using i64vector3 = vector<3, int64_t>;
using u32vector3 = vector<3, uint32_t>;
using i32vector3 = vector<3, int32_t>;
using u16vector3 = vector<3, uint16_t>;
using i16vector3 = vector<3, int16_t>;
using u8vector3  = vector<3, uint8_t>;
using i8vector3  = vector<3, int8_t>;

template<typename T> 
using vector4    = vector<4, T>;
using fvector4   = vector<4, float>;
using dvector4   = vector<4, double>;
using uvector4   = vector<4, uint32_t>;
using ivector4   = vector<4, int32_t>;
using bvector4   = vector<4, bool>;
using f64vector4 = vector<4, double>;
using f32vector4 = vector<4, float>;
using u64vector4 = vector<4, uint64_t>;
using i64vector4 = vector<4, int64_t>;
using u32vector4 = vector<4, uint32_t>;
using i32vector4 = vector<4, int32_t>;
using u16vector4 = vector<4, uint16_t>;
using i16vector4 = vector<4, int16_t>;
using u8vector4  = vector<4, uint8_t>;
using i8vector4  = vector<4, int8_t>;

}
