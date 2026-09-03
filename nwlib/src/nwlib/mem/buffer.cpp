#include "buffer.h"

namespace nw::mem {

buffer::buffer(void* memory, size_t capacity)
	: m_memory({ .raw = memory })
	, m_capacity(memory ? capacity : 0uz)
	, m_owns_mem(false) {}

buffer::buffer(size_t capacity)
	: m_memory({.raw = ::operator new(capacity) })
	, m_capacity(capacity)
	, m_owns_mem(true) {}

buffer::buffer(buffer&& other) noexcept
	: m_memory(other.get_memory())
	, m_capacity(other.get_capacity())
	, m_owns_mem(other.has_ownership())
{
	other.m_memory.raw = nullptr;
	other.m_capacity   = 0uz;
	other.m_owns_mem   = false;
}

buffer::~buffer()
{
	if (m_owns_mem)
		::operator delete(m_memory.raw);
}

buffer& buffer::operator=(buffer&& other) noexcept
{
	if (this != &other)
	{
		if (m_owns_mem)
			::operator delete(m_memory.raw);

		m_memory.raw = other.get_memory();
		m_capacity   = other.get_capacity();
		m_owns_mem   = other.has_ownership();

		other.m_memory.raw = nullptr;
		other.m_capacity   = 0uz;
		other.m_owns_mem   = false;
	}

	return *this;
}

}
