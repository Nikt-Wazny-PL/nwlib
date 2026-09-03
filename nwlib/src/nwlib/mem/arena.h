#pragma once

#include "buffer.h"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <new>
#include <type_traits>

namespace nw::mem {

class arena
{
public:
	constexpr static bool can_reallocate                   { true  }; // Can reallocate overall ?
	constexpr static bool can_reallocate_without_ownership { false }; // Can reallocate when !m_buffer.has_ownership() ?

public:
	arena() = default;
	arena(void* buffer, size_t capacity) : m_buffer(buffer, capacity) {}
	explicit arena(size_t capacity)      : m_buffer(capacity) {}
	explicit arena(buffer&& buffer)      : m_buffer(std::move(buffer)) {}
	arena(const arena& other) = delete;
	arena(arena&& other) noexcept;

	void*    get_memory()    const { return m_buffer.get_memory(); }
	uint8_t* get_u8memory()  const { return m_buffer.get_u8memory(); }
	size_t   get_capacity()  const { return m_buffer.get_capacity(); }
	size_t   get_offset()    const { return m_offset; }
	size_t   get_remaining() const { return get_capacity() - get_offset(); }

	void* allocate(size_t size, size_t alignment = alignof(::max_align_t));

	arena& operator=(const arena& other) = delete;
	arena& operator=(arena&& other) noexcept;

private:
	void reallocate();

private:
	buffer m_buffer {};
	size_t m_offset {};
};

template<typename T>
struct arena_allocator
{
	arena& owner;

	using value_type = T;
	
	using propagate_on_container_copy_assignment = std::true_type;
	using propagate_on_container_move_assignment = std::true_type;
	using propagate_on_container_swap            = std::true_type;
	using is_always_equal                        = std::false_type;

	explicit arena_allocator(arena& arena)
		: owner(arena) {}

	T* allocate(size_t size) { owner.allocate(size * sizeof(T), alignof(T)); }
	T* deallocate(T*, size_t) {}

	bool operator==(const arena_allocator<T>& other) const { return &owner == &other.owner; }
	bool operator!=(const arena_allocator<T>& other) const { return &owner != &other.owner; }
};

template<typename T>
class arena_box;

template<>
class arena_box<void>
{
	template<typename> friend class arena_box;
public:
	arena_box() = default;
	arena_box(arena& owner, size_t size, size_t alignment = alignof(::max_align_t))
		: m_owner(&owner), m_size(size)
	{
		uint8_t* const u8instance = (uint8_t*)owner.allocate(size, alignment);
		m_offset = u8instance - owner.get_u8memory();
	}
	arena_box(const arena_box<void>& other) = delete;
	arena_box(arena_box<void>&& other) noexcept
		: m_owner(other.m_owner)
		, m_offset(other.m_offset)
		, m_size(other.m_size)
	{
		other.m_owner  = nullptr;
		other.m_offset = SIZE_MAX;
		other.m_size   = 0uz;
	}
	~arena_box() = default;

	template<typename T, typename... TArgs>
	arena_box<T> as(TArgs&&... args) const
	{
		assert(m_size >= sizeof(T));

		return is_valid()
			? arena_box<T>(*this, std::forward<TArgs>(args)...)
			: arena_box<T>();
	}

	bool is_valid() const { return m_owner && (m_offset != SIZE_MAX) && (m_size > 0uz); }

	arena* get_owner()  const { return m_owner;  }
	size_t get_offset() const { return m_offset; }
	size_t get_size()   const { return m_size;   }

	void* get() const { return is_valid() ? m_owner->get_u8memory() + m_offset : nullptr; }

	arena_box<void>& operator=(const arena_box<void>& other) = delete;
	arena_box<void>& operator=(arena_box<void>&& other) noexcept
	{
		if (this != &other)
		{
			m_owner  = other.m_owner;
			m_offset = other.m_offset;
			m_size   = other.m_size;

			other.m_owner  = nullptr;
			other.m_offset = SIZE_MAX;
			other.m_size   = 0uz;
		}

		return *this;
	}

	operator bool() const { return is_valid(); }

private:
	arena* m_owner  {};
	size_t m_offset { SIZE_MAX };
	size_t m_size   {};
};

template<typename T>
class arena_box
{
	template<typename> friend class arena_box;
public:
	arena_box() = default;
	template<typename... TArgs>
	arena_box(arena& owner, TArgs&&... args)
		: m_owner(&owner)
	{
		uint8_t* const u8instance = (uint8_t*)owner.allocate(sizeof(T), alignof(T));
		new (u8instance) T(std::forward<TArgs>(args)...);

		m_offset = u8instance - owner.get_u8memory();
	}
	template<typename... TArgs>
	arena_box(const arena_box<void>& void_box, TArgs&&... args)
		: m_owner(void_box.m_owner)
		, m_offset(void_box.m_offset)
	{
		new (get()) T(std::forward<TArgs>(args)...);
	}
	arena_box(const arena_box<T>& other) = delete;
	arena_box(arena_box<T>&& other) noexcept
		: m_owner(other.m_owner)
		, m_offset(other.m_offset)
	{
		other.m_owner  = nullptr;
		other.m_offset = SIZE_MAX;
	}
	~arena_box() { if constexpr (std::is_trivially_default_constructible_v<T>) if (is_valid()) get()->~T(); }

	bool is_valid() const { return m_owner && (m_offset != SIZE_MAX); }

	arena* get_owner()  const { return m_owner;   }
	size_t get_offset() const { return m_offset;  }
	size_t get_size()   const { return sizeof(T); }

	T* get() const { return is_valid() ? std::launder((T*)(m_owner->get_u8memory() + m_offset)) : nullptr; }

	arena_box<T>& operator=(const arena_box<T>& other)       = delete;
	arena_box<T>& operator=(arena_box<T>&& other) noexcept
	{
		if (this != &other)
		{
			if (is_valid())
				get()->~T();

			m_owner  = other.m_owner;
			m_offset = other.m_offset;

			other.m_owner  = nullptr;
			other.m_offset = SIZE_MAX;
		}

		return *this;
	}

	const T* operator->() const { assert(is_valid()); return  get(); }
	T*       operator->()       { assert(is_valid()); return  get(); }

	// std::enable_if_t was required because the compiler saw that T could be potentially 'void' and 'void&' is invalid code
	//                  and it only saw in in this implementation and not in the T[] implementation. Love the C++ compiler [here: clang++].

	template<typename U = T>
	std::enable_if_t<!std::is_void_v<U>, const U&> operator*() const { assert(is_valid()); return *get(); }

	template<typename U = T>
	std::enable_if_t<!std::is_void_v<U>, U&> operator*() { assert(is_valid()); return *get(); }

	operator bool() const { return is_valid(); }

	operator arena_box<void>() const
	{
		arena_box<void> result {};
		result.m_owner  = m_owner;
		result.m_offset = m_offset;
		result.m_size   = sizeof(T);

		return result;
	}

private:
	arena* m_owner  {};
	size_t m_offset { SIZE_MAX };
};

template<typename T>
class arena_box<T[]>
{
	template<typename> friend class arena_box;
public:
	arena_box() = default;
	template<typename... TArgs>
	arena_box(arena& owner, size_t length)
		: m_owner(&owner), m_length(length)
	{
		uint8_t* const u8instance = (uint8_t*)owner.allocate(sizeof(T) * length, alignof(T));
		m_offset = u8instance - owner.get_u8memory();

		T* const instances = get();

		for (size_t i = 0; i < length; i++)
			new (instances + i) T();
	}
	template<typename... TArgs>
	arena_box(const arena_box<void>& void_box, size_t length)
		: m_owner(void_box.m_owner), m_offset(void_box.m_offset), m_length(length)
	{
		for (size_t i = 0; i < length; i++)
			new (get() + i) T();
	}
	arena_box(const arena_box<T[]>& other) = delete;
	arena_box(arena_box<T[]>&& other) noexcept
		: m_owner(other.m_owner)
		, m_offset(other.m_offset)
		, m_length(other.m_length)
	{
		other.m_owner  = nullptr;
		other.m_offset = SIZE_MAX;
		other.m_length = 0uz;
	}
	~arena_box()
	{
		if constexpr (std::is_trivially_default_constructible_v<T>) 
		{
			if (!is_valid())
				return;

			T* const pointer = get();
			
			for (size_t i = 0; i < m_length; i++)
				pointer[i].~T();
		}
	}

	bool is_valid() const { return m_owner && (m_offset != SIZE_MAX) && (m_length > 0); }

	T* begin() { return get(); }
	T* end()   { return get() + m_length; }

	const T* begin() const { return get(); }
	const T* end()   const { return get() + m_length; }

	arena* get_owner()  const { return m_owner;   }
	size_t get_offset() const { return m_offset;  }
	size_t get_size()   const { return sizeof(T) * m_length; }
	size_t get_length() const { return m_length; }

	T* get() const { return is_valid() ? std::launder((T*)(m_owner->get_u8memory() + m_offset)) : nullptr; }

	arena_box<T[]>& operator=(const arena_box<T[]>& other)   = delete;
	arena_box<T[]>& operator=(arena_box<T[]>&& other) noexcept
	{
		if (this != &other)
		{
			if (is_valid())
				get()->~T();

			m_owner  = other.m_owner;
			m_offset = other.m_offset;
			m_length = other.m_length;

			other.m_owner  = nullptr;
			other.m_offset = SIZE_MAX;
			other.m_length = 0uz;
		}

		return *this;
	}

	const T& operator[](size_t index) const  { assert(is_valid() && index < m_length); return *(get() + index); }
	T&       operator[](size_t index)        { assert(is_valid() && index < m_length); return *(get() + index); }

	operator bool() const { return is_valid(); }

	operator arena_box<void>() const
	{
		arena_box<void> result {};
		result.m_owner  = m_owner;
		result.m_offset = m_offset;
		result.m_size   = get_size();
		
		return result;
	}

private:
	arena* m_owner  {};
	size_t m_offset { SIZE_MAX };
	size_t m_length {};
};

}
