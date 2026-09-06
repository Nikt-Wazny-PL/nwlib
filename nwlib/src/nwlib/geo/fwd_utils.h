#pragma once

#include <cstdint>

#define NWGEO_DEFINE_SUBTYPES(type)  \
	using f##type   = type<float>;    \
	using d##type   = type<double>;   \
	using u##type   = type<uint32_t>; \
	using i##type   = type<int32_t>;  \
	using b##type   = type<bool>;     \
	using f64##type = type<double>;   \
	using f32##type = type<float>;    \
	using u64##type = type<uint64_t>; \
	using i64##type = type<int64_t>;  \
	using u32##type = type<uint32_t>; \
	using i32##type = type<int32_t>;  \
	using u16##type = type<uint16_t>; \
	using i16##type = type<int16_t>;  \
	using u8##type  = type<uint8_t>;  \
	using i8##type  = type<int8_t>
