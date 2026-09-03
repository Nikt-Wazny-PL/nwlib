#pragma once

#include <algorithm>
#include <cstdint>

namespace nw::utils {

class uuid
{
public:
	using word_t = uint64_t;
	using byte_t = uint8_t;

public:
	uuid() { regenerate(); }
	uuid(const uuid& other) = default;
	uuid(uuid&& other) noexcept = default;

	uuid& regenerate();

	uuid& operator=(const uuid& other) = default;
	uuid& operator=(uuid&& other) noexcept = default;

	auto  operator<=>(const uuid& other) const { return std::lexicographical_compare_three_way(std::begin(m_bytes), std::end(m_bytes), std::begin(other.m_bytes), std::end(other.m_bytes)); }

private:
	union {
		word_t m_words[0x02];
		byte_t m_bytes[0x10];
	};
};

}
