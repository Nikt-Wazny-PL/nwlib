#pragma once

#include <cstdint>
#include <limits>
#include <type_traits>

namespace nw::utils {

class random
{
public:
	random();
	explicit random(uint64_t seed);
	random(const random&) = delete;
	random(random&&) = delete;

	uint64_t set_seed(uint64_t seed) { return m_seed = seed; }
	uint64_t seed() const { return m_seed; }

	template<typename T>
	T get() const
	{
		if constexpr (std::is_floating_point_v<T>)
			return get_floating(0.0, 1.0);
		else
		{
			return get_integral(
				std::numeric_limits<T>::min(),
				std::numeric_limits<T>::max()
			);
		}
	}

	template<typename T>
	T get(T a) const
	{
		if constexpr (std::is_floating_point_v<T>)
			return get_floating(0.0, a);
		else if (std::is_unsigned_v<T>)
			return get_integral(0, a);
		else
			return get_integral(-a, +a);
	}

	template<typename T>
	T get(T a, T b) const
	{
		if constexpr (std::is_floating_point_v<T>)
			return get_floating(a, b);
		else
			return get_integral(a, b);
	}

	random& operator=(const random&) = delete;
	random& operator=(random&&) = delete;

private:
	uint64_t get_integral(uint64_t a, uint64_t b) const;
	double   get_floating(double a, double b) const;

private:
	mutable uint64_t m_seed {};
};

}
