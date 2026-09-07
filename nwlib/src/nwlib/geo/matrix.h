#pragma once

#include "fwd_matrix.h"
#include "utils.h"
#include "vector.h"

#include <cassert>
#include <cstdint>

namespace nw::geo {

template<uint32_t C, uint32_t R, typename T>
struct matrix
{
	using row_t = vector<R, T>;

	row_t rows[C] {};

	constexpr matrix() = default;
	template<typename U>
	constexpr explicit matrix(const U& value)
	{
		T* const vptr = get_value_ptr(*this);

		for (uint32_t i = 0; i < C*R; i++)
			vptr[i] = value;
	}
	template<typename... Us>
	constexpr explicit matrix(const Us&... values)
	{
		static_assert(sizeof...(values) <= C*R, "value-pack too big!");

		uint32_t index = 0u;
		T* const vptr  = get_value_ptr(*this);

		((vptr[index++] = values), ...);
	}
	template<uint32_t N, typename U>
	constexpr explicit matrix(const U (&values)[N])
	{
		static_assert(N <= C*R, "value-pack too big!");

		T* const vptr  = get_value_ptr(*this);

		for (uint32_t i = 0; i < C*R; i++)
			vptr[i] = values[i];
	}
	template<uint32_t N, typename U>
	constexpr explicit matrix(const row_t (&values)[N])
	{
		static_assert(N <= C, "value-pack too big!");

		for (uint32_t i = 0; i < R; i++)
			rows[i] = values[i];
	}
	template<typename U>
	constexpr explicit matrix(const matrix<C, R, U>& other)
	{
		for (uint32_t i = 0; i < C; i++) {
			for (uint32_t j = 0; j < R; j++)
				rows[i][j] = other.rows[i][j];
		}
	}

	constexpr const T& operator[](uint32_t x, uint32_t y) const
	{
		assert(x + y * R < C * R);
		assert(x < R);
		assert(y < C);

		return rows[y][x];
	}
	constexpr T& operator[](uint32_t x, uint32_t y)
	{
		assert(x + y * R < C * R);
		assert(x < R);
		assert(y < C);

		return rows[y][x];
	}

	constexpr const row_t& operator[](uint32_t index) const
	{
		assert(index < C);
		return rows[index];
	}
	constexpr row_t& operator[](uint32_t index)
	{
		assert(index < C);
		return rows[index];
	}
};

// if you really think about it.. this aint THAT wrong.
// using rgba_screen_1920_1080 = matrix<1080, 1920, uint32_t>;

}
