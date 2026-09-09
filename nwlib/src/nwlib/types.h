#pragma once

#include <cstddef>
#include <cstdint>

namespace nw {

using u8  = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using i8  = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using chr = char;
using wc  = wchar_t;
using c8  = char8_t;
using c16 = char16_t;
using c32 = char32_t;

using f32 = float;
using f64 = double;

using cstr_t    = const chr*;
using wcstr_t   = const wc*;
using u8cstr_t  = const c8*;
using u16cstr_t = const c16*;
using u32cstr_t = const c32*;

template<typename T, size_t N>
using array_reference_t = const T (&)[N];

namespace literals {

constexpr uint8_t  operator""_u8(long double value)  { return (uint8_t)value; }
constexpr uint16_t operator""_u16(long double value) { return (uint16_t)value; }
constexpr uint32_t operator""_u32(long double value) { return (uint32_t)value; }
constexpr uint64_t operator""_u64(long double value) { return (uint64_t)value; }

constexpr int8_t  operator""_i8(long double value)  { return (int8_t)value; }
constexpr int16_t operator""_i16(long double value) { return (int16_t)value; }
constexpr int32_t operator""_i32(long double value) { return (int32_t)value; }
constexpr int64_t operator""_i64(long double value) { return (int64_t)value; }

constexpr float   operator""_f32(long double value) { return (float)value; }
constexpr double  operator""_f64(long double value) { return (double)value; }

}

}
