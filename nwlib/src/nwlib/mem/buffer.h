#pragma once

#include <cstddef>
#include <cstdint>

namespace nw::mem {

class buffer
{
public:
	buffer() = default;
	buffer(void* memory, size_t capacity);
	explicit buffer(size_t capacity);
	buffer(const buffer& other) = delete;
	buffer(buffer&& other) noexcept;
	~buffer();

	template<typename T>
	T* as() const { return (T*)m_memory.raw; }

	void*    get_memory()    const { return m_memory.raw; }
	uint8_t* get_u8memory()  const { return m_memory.u8;  }
	size_t   get_capacity()  const { return m_capacity;   }
	bool     has_ownership() const { return m_owns_mem;   }

	buffer& operator=(const buffer& other) = delete;
	buffer& operator=(buffer&& other) noexcept;

	operator bool() const { return m_memory.raw && m_capacity > 0; }

private:
	union memo_t { void* raw; uint8_t* u8; };

	memo_t m_memory   {};
	size_t m_capacity {};
	bool   m_owns_mem {};
};

}
