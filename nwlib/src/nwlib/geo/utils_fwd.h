#pragma once

#include <cstdint>

#define NWGEO_GENERATE_SUBTYPES(type)  \
	using i##type    = type<int>;      \
	using u##type    = type<unsigned>; \
	using f##type    = type<float>;    \
	using d##type    = type<double>;   \
	using c##type    = type<char>;     \
	using b##type    = type<bool>;     \
	using i8##type   = type<int8_t>;   \
	using i16##type  = type<int16_t>;  \
	using i32##type  = type<int32_t>;  \
	using i64##type  = type<int64_t>;  \
	using u8##type   = type<uint8_t>;  \
	using u16##type  = type<uint16_t>; \
	using u32##type  = type<uint32_t>; \
	using u64##type  = type<uint64_t>; \
	using f32##type  = type<float>;    \
	using f64##type  = type<double>;   \
	using wc##type   = type<wchar_t>;  \
	using c8##type   = type<char8_t>;  \
	using c16##type  = type<char16_t>; \
	using c32##type  = type<char32_t>
