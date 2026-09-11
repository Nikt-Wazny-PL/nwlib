#pragma once

#include "../types.h"
#include "../mem/arena.h"

#include <initializer_list>
#include <memory>
#include <type_traits>

namespace nw::stl {

namespace detail {

template<typename T>
constexpr void call_dtors(T* values, usize count)
{
	if constexpr (std::is_trivially_destructible_v<T>)
		return;

	for (usize i = 0; i < count; i++)
		(values + i)->~T();
}

template<typename T>
constexpr void move_buffer(T* source, T* destination, usize count)
{
	for (usize i = 0; i < count; i++)
	{
		if (std::is_nothrow_move_constructible_v<T>)
			new (destination + i) T(std::move(source[i]));
		else
			new (destination + i) T(source[i]);
	}
}

template<typename T>
constexpr void copy_buffer(T* source, T* destination, usize count)
{
	for (usize i = 0; i < count; i++)
		new (destination + i) T(source[i]);
}

}

template<typename T, typename TAllocator = std::allocator<T>>
class vector
{
public:
	using value_t     = T;
	using allocator_t = TAllocator;
	using iterator_t  = T*;
	using self_t      = vector<T, allocator_t>;

	constexpr static float growth_factor { 1.5f };

public:
	explicit vector(const TAllocator& allocator = {})
		: m_allocator(allocator) {}
	explicit vector(usize capacity, const TAllocator& allocator = {})
		: m_allocator(allocator) { reserve(capacity); }
	vector(const T* buffer, usize count, const TAllocator& allocator = {})
		: m_allocator(allocator)
	{
		if (!m_buffer)
			return;

		reserve(count);

		detail::copy_buffer(buffer, m_buffer, count);
		m_length = count;
	}
	vector(std::initializer_list<T> list, const TAllocator& allocator = {})
		: vector(list.begin(), list.size(), allocator) {}
	vector(const self_t& other)
		: m_buffer(other.get_buffer(), other.get_length(), other.get_allocator()) {}
	vector(self_t&& other) noexcept
		: m_buffer(other.get_buffer())
		, m_length(other.get_length())
		, m_capacity(other.get_capacity())
		, m_allocator(std::move(other.get_allocator()))
	{
		other.m_buffer   = nullptr;
		other.m_length   = 0uz;
		other.m_capacity = 0uz;
	}
	~vector() { detail::call_dtors(m_buffer, m_length); m_allocator.deallocate(m_buffer, m_length); }

	const T* begin() const { return m_buffer; }
	const T* end()   const { return m_buffer + m_length; }
	T*       begin()       { return m_buffer; }
	T*       end()         { return m_buffer + m_length; }

	T*    get_buffer()   const { return m_buffer;   }
	usize get_length()   const { return m_length;   }
	usize get_capacity() const { return m_capacity; }

	TAllocator&       get_allocator()       { return m_allocator; }
	const TAllocator& get_allocator() const { return m_allocator; }

	bool reserve(usize new_capacity)
	{
		if (new_capacity <= m_length)
			return false;

		T* const new_buffer = m_allocator.allocate(new_capacity);

		detail::move_buffer(m_buffer, new_buffer, m_length);
		detail::call_dtors(m_buffer, m_length);

		m_allocator.deallocate(m_buffer, m_capacity);

		m_buffer   = new_buffer;
		m_capacity = new_capacity;

		return true;
	}
	
	template<typename... Ts>
	void resize(usize new_length, Ts&&... args)
	{
		if (new_length < m_length)
		{
			const usize count  = m_length - new_length;
			const usize offset = m_length - count;

			detail::call_dtors(m_buffer + offset, count);
		}
		else if (new_length > m_length)
		{
			if (new_length > m_capacity)
				reserve(new_length);
				
			while (m_length < new_length)
				new (m_buffer + m_length++) T(std::forward<Ts>(args)...);
		}

		m_length = new_length;
	}

	template<typename... Ts>
	T& push(Ts&&... args)
	{
		if (m_length + 1 < m_capacity)
			reallocate();

		T* const instance_ptr = new (m_buffer + m_length++) T(std::forward<Ts>(args)...);
		return *instance_ptr;
	}

	void remove_at(usize index)
	{
		if (index > m_length)
			return;

		detail::call_dtors(m_buffer + index, 1);
		for (usize i = index; i < m_length - 1; i++)
			new (m_buffer + index) T(std::move(m_buffer[index + 1]));

		m_length--;
	}

	void swapback_remove_at(usize index)
	{
		if (index > m_length)
			return;

		detail::call_dtors(m_buffer + index, 1);
		new (m_buffer + index) T(std::move(m_buffer[m_length - 1]));
		detail::call_dtors(m_buffer + m_length - 1, 1);

		m_length--;
	}

	isize find(const T& element)
	{
		for (usize i = 0; i < m_length; i++)
		{
			if (m_buffer[i] == element)
				return i;
		}

		return -1z;
	}

	self_t& operator=(const self_t& other)
	{
		if (this == &other)
			return *this;

		detail::call_dtors(m_buffer, m_length);
		
		reserve(other.get_length());
		detail::copy_buffer(other.get_buffer(), m_buffer, other.get_length());

		return *this;
	}
	self_t& operator=(self_t&& other) noexcept
	{
		if (this == &other)
			return *this;

		detail::call_dtors(m_buffer, m_length);
		m_allocator.deallocate(m_buffer, m_capacity);

		m_buffer    = other.get_buffer();
		m_length    = other.get_length();
		m_capacity  = other.get_capacity();
		m_allocator = std::move(other.get_allocator());

		other.m_buffer   = nullptr;
		other.m_length   = 0uz;
		other.m_capacity = 0uz;

		return *this;
	}

	const T& operator[](usize index) const { assert(index < m_length); return *(m_buffer + index); }
	T&       operator[](usize index)       { assert(index < m_length); return *(m_buffer + index); }

	self_t& operator+=(const T& instance) { push(instance); return *this; }
	self_t& operator+=(T&& instance)      { push(std::move(instance)); return *this; }

	self_t operator+(const T& instance) const { return self_t(*this) += instance; }
	self_t operator+(T&& instance)      const { return self_t(*this) += std::move(instance); }

	bool operator==(const self_t& other) const
	{
		if (m_length != other.get_length())
			return false;

		for (usize i = 0; i < m_length; i++)
		{
			if (m_buffer[i] != other.get_buffer()[i])
				return false;
		}

		if constexpr (!std::allocator_traits<TAllocator>::is_always_equal)
		{
			if (m_allocator != other.get_allocator())
				return false;
		}

		return true;
	}
	bool operator!=(const self_t& other) const { return !(*this == other); }

private:
	void reallocate() { reserve(m_capacity + m_capacity / 2); }

private:
	T*    m_buffer   {};
	usize m_length   {};
	usize m_capacity {};

	TAllocator m_allocator {};
};

template<typename T>
using arena_vector = vector<T, mem::arena_allocator<T>>;

}
