#pragma once

#include <cmath>
#include <cstddef>
#include <type_traits>
#include <utility>

#include "fwd_vector.h"
#include "utils.h"

namespace nw::geo {

template<typename T>
struct vector<1, T>
{
	union { T x {}, r, s; };

	constexpr vector() = default;
	template<typename U>
	constexpr explicit vector(const U& x)
		: x(x) {}
	template<typename U>
	constexpr vector(const vector<1, U>& other)
		: x(other.x) {}

	constexpr const T& operator[](size_t index) const { assert(index < get_element_count(*this)); return *(&x + index); }
	constexpr T&       operator[](size_t index)       { assert(index < get_element_count(*this)); return *(&x + index); }
};

template<typename T>
struct vector<2, T>
{
	union { T x {}, r, s; };
	union { T y {}, g, t; };

	constexpr vector() = default;
	template<typename U>
	constexpr explicit vector(const U& xy)
		: x(xy), y(xy) {}
	template<typename U, typename W>
	constexpr explicit vector(const U& x, const W& y)
		: x(x), y(y) {}
	template<typename U, typename W>
	constexpr explicit vector(const vector<1, U>& x, const vector<1, W>& y)
		: x(x.x), y(y.x) {}
	template<typename U, typename W>
	constexpr explicit vector(const vector<1, U>& x, const W& y)
		: x(x.x), y(y) {}
	template<typename U, typename W>
	constexpr explicit vector(const U& x, const vector<1, W>& y)
		: x(x), y(y.x) {}
	template<typename U>
	constexpr explicit vector(const vector<2, U>& other)
		: x(other.x), y(other.y) {}

	constexpr const T& operator[](size_t index) const { assert(index < get_element_count(*this)); return *(&x + index); }
	constexpr T&       operator[](size_t index)       { assert(index < get_element_count(*this)); return *(&x + index); }
};

template<typename T>
struct vector<3, T>
{
	union { T x {}, r, s; };
	union { T y {}, g, t; };
	union { T z {}, b, p; };

	constexpr vector() = default;
	template<typename U>
	constexpr explicit vector(const U& xyz)
		: x(xyz), y(xyz), z(xyz) {}
	template<typename U, typename W, typename X>
	constexpr explicit vector(const U& x, const W& y, const X& z)
		: x(x), y(y), z(z) {}
	template<typename U, typename W, typename Z>
	constexpr explicit vector(const vector<1, U>& x, const vector<1, W>& y, const vector<1, Z>& z)
		: x(x.x), y(y.x), z(z.x) {}
	template<typename U, typename W, typename Z>
	constexpr explicit vector(const vector<2, U>& xy, const vector<1, W>& z)
		: x(xy.x), y(xy.y), z(z.x) {}
	template<typename U, typename W, typename Z>
	constexpr explicit vector(const vector<1, U>& x, const vector<2, W>& yz)
		: x(x.x), y(yz.x), z(yz.y) {}
	template<typename U>
	constexpr explicit vector(const vector<3, U>& other)
		: x(other.x), y(other.y), z(other.z) {}

	constexpr const T& operator[](size_t index) const { assert(index < get_element_count(*this)); return *(&x + index); }
	constexpr T&       operator[](size_t index)       { assert(index < get_element_count(*this)); return *(&x + index); }
};

template<typename T>
struct vector<4, T>
{
	union { T x {}, r, s; };
	union { T y {}, g, t; };
	union { T z {}, b, p; };
	union { T w {}, a, q; };

	constexpr vector() = default;
	template<typename U>
	constexpr explicit vector(const U& xyzw)
		: x(xyzw), y(xyzw), z(xyzw), w(xyzw) {}
	template<typename U, typename V, typename W, typename X>
	constexpr explicit vector(const U& x, const V& y, const W& z, const X& w)
		: x(x), y(y), z(z), w(w) {}
	template<typename U, typename V, typename W, typename X>
	constexpr explicit vector(const vector<1, U>& x, const vector<1, V>& y, const vector<1, W>& z, const vector<1, X>& w)
		: x(x.x), y(y.x), z(z.x), w(w.x) {}
	template<typename U, typename V, typename W>
	constexpr explicit vector(const vector<2, U>& xy, const vector<1, V>& z, const vector<1, W>& w)
		: x(xy.x), y(xy.y), z(z.x), w(w.x) {}
	template<typename U, typename V, typename W>
	constexpr explicit vector(const vector<1, U>& x, const vector<2, V>& yz, const vector<1, W>& w)
		: x(x.x), y(yz.x), z(yz.y), w(w.x) {}
	template<typename U, typename V, typename W>
	constexpr explicit vector(const vector<1, U>& x, const vector<1, V>& y, const vector<2, W>& zw)
		: x(x.x), y(y.x), z(zw.x), w(zw.y) {}
	template<typename U, typename V>
	constexpr explicit vector(const vector<2, U>& xy, const vector<2, V>& zw)
		: x(xy.x), y(xy.y), z(zw.x), w(zw.y) {}
	template<typename U, typename V>
	constexpr explicit vector(const vector<3, U>& xyz, const vector<1, V>& w)
		: x(xyz.x), y(xyz.y), z(xyz.z), w(w.x) {}
	template<typename U, typename V>
	constexpr explicit vector(const vector<1, U>& x, const vector<3, V>& yzw)
		: x(x.x), y(yzw.x), z(yzw.y), w(yzw.z) {}
	template<typename U>
	constexpr explicit vector(const vector<4, U>& other)
		: x(other.x), y(other.y), z(other.z), w(other.w) {}

	constexpr const T& operator[](size_t index) const { assert(index < get_element_count(*this)); return *(&x + index); }
	constexpr T&       operator[](size_t index)       { assert(index < get_element_count(*this)); return *(&x + index); }
};


template<uint32_t N, typename T, uint32_t M, typename U>
constexpr vector<N, T>& operator+=(vector<N, T>& lhs, const vector<M, U>& rhs)
{
	T* const lhs_vptr = get_value_ptr(lhs);
	U* const rhs_vptr = get_value_ptr(rhs);

	for (uint32_t i = 0; i < (N < M ? N : M); i++)
		lhs[i] += rhs[i];

	return lhs;
}
template<uint32_t N, typename T, typename U>
constexpr vector<N, T>& operator+=(vector<N, T>& lhs, const U& rhs)
{
	T* const lhs_vptr = get_value_ptr(lhs);

	for (uint32_t i = 0; i < N; i++)
		lhs[i] += rhs;

	return lhs;
}

template<uint32_t N, typename T, uint32_t M, typename U>
constexpr auto operator+(vector<N, T>& lhs, const vector<M, U>& rhs)
{
	using result_t = std::remove_cvref_t<decltype(std::declval<T>() + std::declval<U>())>;

	if constexpr (N > M)
		return vector<N, result_t>(lhs) += rhs;
	else
		return vector<M, result_t>(lhs) += rhs;
}
template<uint32_t N, typename T, typename U>
constexpr auto operator+(vector<N, T>& lhs, const U& rhs)
{
	using result_t = std::remove_cvref_t<decltype(std::declval<T>() + std::declval<U>())>;

	return vector<N, result_t>(lhs) += rhs;
}

template<uint32_t N, typename T>
constexpr vector<N, T> operator++(vector<N, T>& lhs, int)
{
	const auto temp = lhs;
	lhs += (T)1;

	return temp;
}
template<uint32_t N, typename T>
constexpr vector<N, T>& operator++(vector<N, T>& lhs)
{
	return lhs += (T)1;
}


template<uint32_t N, typename T, uint32_t M, typename U>
constexpr vector<N, T>& operator-=(vector<N, T>& lhs, const vector<M, U>& rhs)
{
	T* const lhs_vptr = get_value_ptr(lhs);
	U* const rhs_vptr = get_value_ptr(rhs);

	for (uint32_t i = 0; i < (N < M ? N : M); i++)
		lhs[i] -= rhs[i];

	return lhs;
}
template<uint32_t N, typename T, typename U>
constexpr vector<N, T>& operator-=(vector<N, T>& lhs, const U& rhs)
{
	T* const lhs_vptr = get_value_ptr(lhs);

	for (uint32_t i = 0; i < N; i++)
		lhs[i] -= rhs;

	return lhs;
}

template<uint32_t N, typename T, uint32_t M, typename U>
constexpr auto operator-(vector<N, T>& lhs, const vector<M, U>& rhs)
{
	using result_t = std::remove_cvref_t<decltype(std::declval<T>() - std::declval<U>())>;

	if constexpr (N > M)
		return vector<N, result_t>(lhs) -= rhs;
	else
		return vector<M, result_t>(lhs) -= rhs;
}
template<uint32_t N, typename T, typename U>
constexpr auto operator-(vector<N, T>& lhs, const U& rhs)
{
	using result_t = std::remove_cvref_t<decltype(std::declval<T>() - std::declval<U>())>;

	return vector<N, result_t>(lhs) -= rhs;
}

template<uint32_t N, typename T>
constexpr vector<N, T> operator--(vector<N, T>& lhs, int)
{
	const auto temp = lhs;
	lhs -= (T)1;

	return temp;
}
template<uint32_t N, typename T>
constexpr vector<N, T>& operator--(vector<N, T>& lhs)
{
	return lhs -= (T)1;
}


template<uint32_t N, typename T, uint32_t M, typename U>
constexpr vector<N, T>& operator*=(vector<N, T>& lhs, const vector<M, U>& rhs)
{
	T* const lhs_vptr = get_value_ptr(lhs);
	U* const rhs_vptr = get_value_ptr(rhs);

	for (uint32_t i = 0; i < (N < M ? N : M); i++)
		lhs[i] *= rhs[i];

	return lhs;
}
template<uint32_t N, typename T, typename U>
constexpr vector<N, T>& operator*=(vector<N, T>& lhs, const U& rhs)
{
	T* const lhs_vptr = get_value_ptr(lhs);

	for (uint32_t i = 0; i < N; i++)
		lhs[i] *= rhs;

	return lhs;
}

template<uint32_t N, typename T, uint32_t M, typename U>
constexpr auto operator*(vector<N, T>& lhs, const vector<M, U>& rhs)
{
	using result_t = std::remove_cvref_t<decltype(std::declval<T>() * std::declval<U>())>;

	if constexpr (N > M)
		return vector<N, result_t>(lhs) *= rhs;
	else
		return vector<M, result_t>(lhs) *= rhs;
}
template<uint32_t N, typename T, typename U>
constexpr auto operator*(vector<N, T>& lhs, const U& rhs)
{
	using result_t = std::remove_cvref_t<decltype(std::declval<T>() * std::declval<U>())>;

	return vector<N, result_t>(lhs) *= rhs;
}


template<uint32_t N, typename T, uint32_t M, typename U>
constexpr vector<N, T>& operator/=(vector<N, T>& lhs, const vector<M, U>& rhs)
{
	T* const lhs_vptr = get_value_ptr(lhs);
	U* const rhs_vptr = get_value_ptr(rhs);

	for (uint32_t i = 0; i < (N < M ? N : M); i++)
		lhs[i] /= rhs[i];

	return lhs;
}
template<uint32_t N, typename T, typename U>
constexpr vector<N, T>& operator/=(vector<N, T>& lhs, const U& rhs)
{
	T* const lhs_vptr = get_value_ptr(lhs);

	for (uint32_t i = 0; i < N; i++)
		lhs[i] /= rhs;

	return lhs;
}

template<uint32_t N, typename T, uint32_t M, typename U>
constexpr auto operator/(vector<N, T>& lhs, const vector<M, U>& rhs)
{
	using result_t = std::remove_cvref_t<decltype(std::declval<T>() / std::declval<U>())>;

	if constexpr (N > M)
		return vector<N, result_t>(lhs) /= rhs;
	else
		return vector<M, result_t>(lhs) /= rhs;
}
template<uint32_t N, typename T, typename U>
constexpr auto operator/(vector<N, T>& lhs, const U& rhs)
{
	using result_t = std::remove_cvref_t<decltype(std::declval<T>() / std::declval<U>())>;

	return vector<N, result_t>(lhs) /= rhs;
}


template<uint32_t N, typename T>
constexpr auto square_magnitude(const vector<N, T>& vector)
{
	using result_t = std::conditional_t<(sizeof(T) > sizeof(float)), double, float>;

	T* const vptr = get_value_ptr(vector);

	result_t result {};
	for (uint32_t i = 0; i < N; i++)
		result += vptr[i] * vptr[i];

	return result;
}

template<uint32_t N, typename T>
constexpr auto magnitude(const vector<N, T>& vector)
{
	return std::sqrt(square_magnitude(vector));
}

}
