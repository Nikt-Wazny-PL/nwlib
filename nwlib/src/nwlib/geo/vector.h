#pragma once

#include "vector_fwd.h"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <type_traits>

namespace nw::geo {

namespace detail {

template<typename T> requires(std::is_arithmetic_v<T>)
struct vector_impl<1, T>
{
	union { T x, r, s {}; };

	constexpr const T* value_ptr() const { return &x; }
	constexpr T*       value_ptr()       { return &x; }
};

template<typename T> requires(std::is_arithmetic_v<T>)
struct vector_impl<2, T>
{
	union { T x, r, s {}; };
	union { T y, g, t {}; };

	constexpr const T* value_ptr() const { return &x; }
	constexpr T*       value_ptr()       { return &x; }
};

template<typename T> requires(std::is_arithmetic_v<T>)
struct vector_impl<3, T>
{
	union { T x, r, s {}; };
	union { T y, g, t {}; };
	union { T z, b, p {}; };

	constexpr const T* value_ptr() const { return &x; }
	constexpr T*       value_ptr()       { return &x; }
};

template<typename T> requires(std::is_arithmetic_v<T>)
struct vector_impl<4, T>
{
	union { T x, r, s {}; };
	union { T y, g, t {}; };
	union { T z, b, p {}; };
	union { T w, a, q {}; };

	constexpr const T* value_ptr() const { return &x; }
	constexpr T*       value_ptr()       { return &x; }
};

template<size_t N, typename T> requires(std::is_arithmetic_v<T>)
struct vector_impl
{
	T elements[N];

	constexpr const T* value_ptr() const { return elements; }
	constexpr T*       value_ptr()       { return elements; }
};


template<typename T>
struct is_vector : std::false_type {};

template<size_t N, typename T> requires(std::is_arithmetic_v<T>)
struct is_vector<vector<N, T>> : std::true_type {};


constexpr size_t get_total_count(const auto&... vectors)
{
	size_t total = 0uz;
	((total += vectors.element_count()), ...);

	return total;
}

}

template<size_t N, typename T> requires(std::is_arithmetic_v<T>)
struct vector : public detail::vector_impl<N, T>
{
	constexpr vector() = default;
	template<typename... Ts>
	constexpr vector(const Ts&... values)
	{
		T* const begin = this->value_ptr();
		size_t   index = 0uz;

		if constexpr ((std::is_arithmetic_v<Ts> && ...))
		{
			if constexpr (sizeof...(Ts) == 1)
			{
				const auto& element = std::get<0>(std::tuple(values...));
				for (size_t i = 0; i < N; i++)
					begin[i] = element;

				return;
			}

			static_assert(sizeof...(Ts) <= N);
			((begin[index++] = values), ...);
		}
		else if constexpr ((detail::is_vector<Ts>::value && ...))
		{
			constexpr size_t total_count = detail::get_total_count(values...);
			static_assert(total_count <= N);

			([&]
			{
				for (size_t i = 0; i < values.element_count(); i++)
					(begin[index++]) = values[i];
			}, ...);
		}
		else
			static_assert(detail::is_vector<int>::value, "Invalid datamember!");
	}

	constexpr T*       begin()        { return this->value_ptr() + 0; }
	constexpr T*       end()          { return this->value_ptr() + N; }
	constexpr const T* begin()  const { return this->value_ptr() + 0; }
	constexpr const T* end()    const { return this->value_ptr() + N; }
	constexpr const T* cbegin() const { return this->value_ptr() + 0; }
	constexpr const T* cend()   const { return this->value_ptr() + N; }

	constexpr size_t element_count() const { return N; }

	template<typename U>
	constexpr vector& operator+=(const U& value)
	{
		T* const begin = this->value_ptr();

		if constexpr (std::is_arithmetic_v<U>)
		{
			for (size_t i = 0; i < N; i++)
				begin[i] += value;
		}
		else if (detail::is_vector<U>::value)
		{
			constexpr size_t min = std::min(N, value.element_count());

			for (size_t i = 0; i < min; i++)
				begin[i] += value[i];
		}
		else
			static_assert(detail::is_vector<int>::value, "Invalid datamember!");

		return *this;
	}
	constexpr vector  operator++(int) 
	{
		const auto temp = *this;
		*this += (T)1;

		return temp;
	}
	constexpr vector& operator++() 
	{
		return *this += (T)1;
	}
	template<typename U>
	constexpr vector  operator+(const U& value) const { return vector(*this) += value; }

	template<typename U>
	constexpr vector& operator-=(const U& value)
	{
		T* const begin = this->value_ptr();

		if constexpr (std::is_arithmetic_v<U>)
		{
			for (size_t i = 0; i < N; i++)
				begin[i] -= value;
		}
		else if (detail::is_vector<U>::value)
		{
			constexpr size_t min = std::min(N, value.element_count());

			for (size_t i = 0; i < min; i++)
				begin[i] -= value[i];
		}
		else
			static_assert(detail::is_vector<int>::value, "Invalid datamember!");

		return *this;
	}
	constexpr vector  operator--(int) 
	{
		const auto temp = *this;
		*this -= (T)1;

		return temp;
	}
	constexpr vector& operator--() 
	{
		return *this -= (T)1;
	}
	template<typename U>
	constexpr vector  operator-(const U& value) const { return vector(*this) -= value; }

	template<typename U>
	constexpr vector& operator*=(const U& value)
	{
		T* const begin = this->value_ptr();

		if constexpr (std::is_arithmetic_v<U>)
		{
			for (size_t i = 0; i < N; i++)
				begin[i] *= value;
		}
		else if (detail::is_vector<U>::value)
		{
			constexpr size_t min = std::min(N, value.element_count());

			for (size_t i = 0; i < min; i++)
				begin[i] *= value[i];
		}
		else
			static_assert(detail::is_vector<int>::value, "Invalid datamember!");

		return *this;
	}
	template<typename U>
	constexpr vector  operator*(const U& value) const { return vector(*this) *= value; }

	template<typename U>
	constexpr vector& operator/=(const U& value)
	{
		T* const begin = this->value_ptr();

		if constexpr (std::is_arithmetic_v<U>)
		{
			for (size_t i = 0; i < N; i++)
				begin[i] /= value;
		}
		else if (detail::is_vector<U>::value)
		{
			constexpr size_t min = std::min(N, value.element_count());

			for (size_t i = 0; i < min; i++)
				begin[i] /= value[i];
		}
		else
			static_assert(detail::is_vector<int>::value, "Invalid datamember!");

		return *this;
	}
	template<typename U>
	constexpr vector  operator/(const U& value) const { return vector(*this) /= value; }

	constexpr const T& operator[](size_t index) const { assert(index < N); return *(value_ptr() + index); }
	constexpr T&       operator[](size_t index)       { assert(index < N); return *(value_ptr() + index); }
};

}
