#include "random.h"

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <random>

namespace nw::utils {

static std::random_device s_Device {};

static constexpr uint64_t random_hash(uint64_t seed)
{
	seed ^= seed >> 30;
	seed *= 0xbf58476d1ce4e5b9;
	seed ^= seed >> 27;
	seed *= 0x94d049bb133111eb;
	seed ^= seed >> 31;

	return seed;
}

template<typename T>
static constexpr T generate_integral(uint64_t& seed) noexcept
{
	constexpr auto min_size = std::min(sizeof(uint64_t), sizeof(T));

	seed = random_hash(seed);

	T result {};
	std::memcpy(&result, &seed, min_size);

	return result;
}


random::random()
	: m_seed(s_Device()) {}

random::random(uint64_t seed)
	: m_seed(seed) {}

uint64_t random::get_integral(uint64_t a, uint64_t b) const
{
	const uint64_t min = std::min(a, b);
	const uint64_t max = std::max(a, b);

	return min + (generate_integral<uint64_t>(m_seed) % (max - min + 1));
}

double random::get_floating(double a, double b) const
{
	return a + (get<uint64_t>() / (double)UINT64_MAX) * (b - a);
}

}
