#include "arena.h"

#include <algorithm>
#include <cstddef>
#include <cstring>

namespace nw::mem {

arena::arena(arena&& other) noexcept
	: m_buffer(std::move(other.m_buffer))
	, m_offset(other.m_offset)
{
	other.m_offset = 0uz;
}

void* arena::allocate(size_t size, size_t alignment)
{
	const auto aligned_offset = (m_offset + alignment - 1) & ~(alignment - 1);
	const auto padding        = aligned_offset - m_offset;

	while (padding + size >= get_remaining())
	{
		if constexpr (can_reallocate)
			reallocate();
		else if (can_reallocate_without_ownership && !m_buffer.has_ownership())
			reallocate();
		else
			return nullptr;
	}

	m_offset = aligned_offset + size;
	return m_buffer.get_u8memory() + aligned_offset;
}

void arena::reallocate()
{
	size_t new_capacity = m_buffer.get_capacity() + m_buffer.get_capacity() / 2uz;
	new_capacity = std::max(1024uz, new_capacity);

	buffer new_buffer(new_capacity);

	std::memcpy(new_buffer.get_memory(), m_buffer.get_memory(), m_offset);

	m_buffer = std::move(new_buffer);
}

arena& arena::operator=(arena&& other) noexcept
{
	if (this != &other)
	{
		m_buffer = std::move(other.m_buffer);
		m_offset = other.m_offset;

		other.m_offset = 0uz;
	}

	return *this;
}

}
