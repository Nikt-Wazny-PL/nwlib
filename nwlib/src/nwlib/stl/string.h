#pragma once

#include "../types.h"

#include <cstddef>
#include <cstring>
#include <initializer_list>
#include <type_traits>
#include <utility>

namespace nw::stl {

namespace detail {

constexpr usize length_of(const auto* cstr)
{
	if (!cstr)
		return 0;

	usize length {};
	while (cstr[length] != 0)
		length++;

	return length;
}

}

template<typename T, typename TAllocator>
class string
{
public:
	using value_t     = T;
	using allocator_t = TAllocator;
	using iterator_t  = T*;
	using self_t      = string<T, allocator_t>;

	using cstr_t = 
		std::conditional_t<std::is_same_v<T, chr>, nw::cstr_t,
		std::conditional_t<std::is_same_v<T, wc>,  nw::wcstr_t,
		std::conditional_t<std::is_same_v<T, c8>,  nw::u8cstr_t,
		std::conditional_t<std::is_same_v<T, c16>, nw::u16cstr_t,
		std::conditional_t<std::is_same_v<T, c32>, nw::u32cstr_t, void
	>>>>>;

	constexpr static float growth_factor   { 1.5f    };
	constexpr static T     null_terminator { (T)'\0' };
	constexpr static T*    default_cstring { []() {
		if constexpr (std::is_same_v<T, chr>) return "";
		if constexpr (std::is_same_v<T, wc>)  return L"";
		if constexpr (std::is_same_v<T, c8>)  return u8"";
		if constexpr (std::is_same_v<T, c16>) return u"";
		if constexpr (std::is_same_v<T, c32>) return U"";

		std::unreachable();
	}() };

public:
	explicit string(const TAllocator& allocator = {})
		: m_allocator(allocator) {}
	explicit string(usize capacity, const TAllocator& allocator = {})
		: m_allocator(allocator) { reserve(capacity); }
	string(const T* buffer, usize count = 0, const TAllocator& allocator = {})
		: m_allocator(allocator)
	{
		if (!m_buffer)
			return;

		count = std::min(count, detail::length_of(buffer));

		reserve(count);

		std::memcpy(buffer, m_buffer, count * sizeof(T));
		m_length = count;
		m_buffer[m_length] = null_terminator;
	}
	string(std::initializer_list<T> list, const TAllocator& allocator = {})
		: string(list.begin(), list.size(), allocator) {}
	string(const self_t& other)
		: m_buffer(other.get_buffer(), other.get_length(), other.get_allocator()) {}
	string(self_t&& other) noexcept
		: m_buffer(other.get_buffer())
		, m_length(other.get_length())
		, m_capacity(other.get_capacity())
		, m_allocator(std::move(other.get_allocator()))
	{
		other.m_buffer   = nullptr;
		other.m_length   = 0uz;
		other.m_capacity = 0uz;
	}
	~string() { m_allocator.deallocate(m_buffer, m_length); }

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
		new_capacity += 1uz;

		if (new_capacity <= m_length)
			return false;

		T* const new_buffer = m_allocator.allocate(new_capacity);
		std::memcpy(m_buffer, new_buffer, m_length * sizeof(T));

		m_allocator.deallocate(m_buffer, m_capacity);

		m_buffer   = new_buffer;
		m_capacity = new_capacity;

		return true;
	}
	void resize(usize new_length, char c = ' ')
	{
		if (new_length > m_length)
		{
			if (new_length > m_capacity)
				reserve(new_length);

			while (m_length < new_length)
				m_buffer[m_length++] = c;
		}

		m_length = new_length;
		m_buffer[m_length] = null_terminator;
	}
	
	T& push(T c)
	{
		if (m_length + 1 < m_capacity)
			reallocate();

		T& chr = m_buffer[m_length++];
		chr = c;
		m_buffer[m_length] = null_terminator;

		return chr;
	}
	T* push(cstr_t cstr)
	{
		if (!cstr)
			return get_buffer();

		const usize length = detail::length_of(cstr);

		if (m_length + length < m_capacity)
			reserve(m_length + length);

		T* const result = m_buffer + m_length;

		for (usize i = 0; i < length; i++)
			m_buffer[m_length + i] = cstr[i];

		m_buffer[m_length] = null_terminator;

		return result;
	}

	self_t& operator=(const self_t& other)
	{
		if (this == &other)
			return *this;

		reserve(other.get_length());
		std::memcpy(other.get_buffer(), m_buffer, other.get_length() * sizeof(T));

		return *this;
	}
	self_t& operator=(self_t&& other) noexcept
	{
		if (this == &other)
			return *this;

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

	self_t& operator+=(T c) { push(c); return *this; }
	self_t& operator+=(cstr_t cstr) { push(cstr); return *this; }

	self_t operator+(T instance)  const { return self_t(*this) += instance; }
	self_t operator+(cstr_t cstr) const { return self_t(*this) += cstr; }

private:
	T*    m_buffer   {};
	usize m_length   {};
	usize m_capacity {};

	TAllocator m_allocator {};
};

}