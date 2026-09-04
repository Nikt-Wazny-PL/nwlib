#pragma once

#include "../mem/arena.h"
#include "../mem/buffer.h"
#include "iterator.h"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace nw::stl {

namespace string_utils {

enum class compare_result_t : uint8_t { null = 0, equal, less, greater };

using length_t = decltype(0uz);

constexpr length_t max_length = SIZE_MAX;

template<typename T>
constexpr bool is_character
	=  std::is_same_v<T, char>
	|| std::is_same_v<T, wchar_t>
	|| std::is_same_v<T, char8_t>
	|| std::is_same_v<T, char16_t>
	|| std::is_same_v<T, char32_t>;

template<typename T>
constexpr auto get_default_cstr() -> std::enable_if_t<is_character<T>, const T*>
{
	return std::is_same_v<T, char>     ?   ""
		:  std::is_same_v<T, wchar_t>  ?  L""
		:  std::is_same_v<T, char8_t>  ? u8""
		:  std::is_same_v<T, char16_t> ?  u""
		:  std::is_same_v<T, char32_t> ?  U""
		:  nullptr;
}

template<typename T>
constexpr std::enable_if_t<is_character<T>, T> null_terminator = (T)0;

template<typename T>
constexpr auto calculate_length(const T* cstring) -> std::enable_if_t<is_character<T>, length_t>
{
	if (!cstring)
		return 0uz;

	length_t length {};
	while (cstring[length])
		length++;

	return length;
}

template<typename T>
constexpr auto compare(const T* cstring_a, length_t length_a, const T* cstring_b, length_t length_b) -> std::enable_if_t<is_character<T>, compare_result_t>
{
	if (!cstring_a || !cstring_b)
		return compare_result_t::null;

	if (length_a < length_b)
		return compare_result_t::less;

	if (length_a > length_b)
		return compare_result_t::greater;

	for (length_t i = 0; i < length_a; i++)
	{
		if (cstring_a[i] < cstring_b[i])
			return compare_result_t::less;

		if (cstring_a[i] > cstring_b[i])
			return compare_result_t::greater;
	}

	return compare_result_t::equal;
}

}

template<typename T>
class basic_string
{
	static_assert(string_utils::is_character<T>);
public:
	using value_t           = T;
	using const_value_t     = const value_t;
	using pointer_t         = value_t*;
	using const_pointer_t   = const value_t*;
	using reference_t       = value_t&;
	using const_reference_t = const value_t&;
	using temporary_t       = value_t&;
	using const_temporary_t = const value_t&;

	using length_t  = string_utils::length_t;
	using cstring_t = const T*;

	using forward_iterator       = stl::forward_iterator<basic_string<T>>;
	using const_forward_iterator = stl::const_forward_iterator<basic_string<T>>;
	using reverse_iterator       = stl::reverse_iterator<basic_string<T>>;
	using const_reverse_iterator = stl::const_reverse_iterator<basic_string<T>>;

	using self_t = basic_string<T>;

public:
	basic_string() : basic_string("") {}
	basic_string(const T* cstring, length_t length = string_utils::max_length)
	{
		length = std::min(length, string_utils::calculate_length(cstring));
		if (!cstring || length == 0uz)
			return;

		m_buffer = mem::buffer(const_cast<T*>(cstring), (length + 1uz) * sizeof(T));
		m_length = length;
	}
	basic_string(const self_t& other)
		: m_buffer(other.m_buffer.get_capacity())
		, m_length(other.m_length)
	{
		for (length_t i = 0uz; i < m_length; i++)
			m_buffer.as<T>()[i] = other.m_buffer.as<T>()[i];
	}
	basic_string(self_t&& other) noexcept
		: m_buffer(std::move(other.m_buffer))
		, m_length(other.m_length)
	{
		other.m_length = 0uz;
	}

	forward_iterator       begin()        { return { m_buffer.as<T>() }; }
	forward_iterator       end()          { return { m_buffer.as<T>() + m_length }; }
	const_forward_iterator begin()  const { return { m_buffer.as<T>() }; }
	const_forward_iterator end()    const { return { m_buffer.as<T>() + m_length }; }
	const_forward_iterator cbegin() const { return { m_buffer.as<T>() }; }
	const_forward_iterator cend()   const { return { m_buffer.as<T>() + m_length }; }

	reverse_iterator       rbegin()        { return { m_buffer.as<T>() + m_length - 1 }; }
	reverse_iterator       rend()          { return { m_buffer.as<T>() - 1 }; }
	const_reverse_iterator rbegin()  const { return { m_buffer.as<T>() + m_length - 1 }; }
	const_reverse_iterator rend()    const { return { m_buffer.as<T>() - 1 }; }
	const_reverse_iterator rcbegin() const { return { m_buffer.as<T>() + m_length - 1 }; }
	const_reverse_iterator rcend()   const { return { m_buffer.as<T>() - 1 }; }

	bool reserve(length_t new_capacity)
	{
		if (new_capacity <= get_capacity())
			return false;

		mem::buffer new_buffer(new_capacity * sizeof(T));
		std::memcpy(new_buffer.as<void>(), m_buffer.as<const void>(), m_buffer.get_capacity());

		m_buffer = std::move(new_buffer);

		return true;
	}
	void resize(length_t new_length, T fill = string_utils::null_terminator<T>)
	{
		reserve(new_length);

		if (new_length < m_length)
		{
			m_buffer.as<T>()[new_length] = string_utils::null_terminator<T>;
			m_length = new_length;
		}
		else if (new_length > m_length)
		{
			pointer_t buffer = get_buffer();
			while (m_length != new_length)
				buffer[m_length++] = fill;
		}
	}

	bool      is_view()      const { return !m_buffer.has_ownership(); }
	bool      is_empty()     const { return !m_buffer || m_length == 0; }
	length_t  get_capacity() const { return m_buffer.get_capacity() / sizeof(T); }
	pointer_t get_buffer()   const { return m_buffer.as<T>(); }
	length_t  get_length()   const { return m_length; }
	cstring_t cstring()      const { return m_buffer.as<const T>(); }

	self_t& make_owned()
	{
		if (is_view())
		{
			mem::buffer new_buffer(m_buffer.get_capacity());
			std::memcpy(new_buffer.as<void>(), m_buffer.as<const void>(), m_buffer.get_capacity());

			m_buffer = std::move(new_buffer);
		}

		return *this;
	}

	self_t substring(length_t offset, length_t length) const
	{
		if (offset > m_length)
			return self_t();

		if (m_length - offset > length)
			length = m_length - offset;

		return self_t(cstring() + offset, length);
	}

	self_t& operator=(const self_t& other)
	{
		if (this != &other)
		{
			m_buffer = mem::buffer(other.m_buffer.get_capacity());
			m_length = other.m_length;

			for (length_t i = 0uz; i < m_length; i++)
				m_buffer.as<T>()[i] = other.m_buffer.as<T>()[i];
		}

		return *this;
	}
	self_t& operator=(self_t&& other) noexcept
	{
		if (this != &other)
		{
			m_buffer = std::move(other.m_buffer);
			m_length = other.m_length;

			other.m_length = 0uz;
		}

		return *this;
	}

	const T& operator[](size_t index) const
	{
		assert(m_buffer && (index < m_length)); 
		return *(m_buffer.as<T>() + index);
	}
	T& operator[](size_t index)
	{
		assert(m_buffer && (index < m_length));

		make_owned();
		return *(m_buffer.as<T>() + index);
	}

	bool operator==(const self_t& other) const { return string_utils::compare(cstring(), get_length(), other.cstring(), other.get_length()) == string_utils::compare_result_t::equal;   }
	bool operator!=(const self_t& other) const { return string_utils::compare(cstring(), get_length(), other.cstring(), other.get_length()) != string_utils::compare_result_t::equal;   }
	bool operator> (const self_t& other) const { return string_utils::compare(cstring(), get_length(), other.cstring(), other.get_length()) == string_utils::compare_result_t::greater; }
	bool operator< (const self_t& other) const { return string_utils::compare(cstring(), get_length(), other.cstring(), other.get_length()) == string_utils::compare_result_t::less;    }
	bool operator>=(const self_t& other) const { return (*this > other) || (*this == other); }
	bool operator<=(const self_t& other) const { return (*this < other) || (*this == other); }

private:
	mem::buffer m_buffer {};
	length_t    m_length {};
};

using string    = basic_string<char>;
using wstring   = basic_string<wchar_t>;
using u8string  = basic_string<char8_t>;
using u16string = basic_string<char16_t>;
using u32string = basic_string<char32_t>;


template<typename T>
class basic_arena_string
{
	static_assert(string_utils::is_character<T>);
public:
	using value_t           = T;
	using const_value_t     = const value_t;
	using pointer_t         = value_t*;
	using const_pointer_t   = const value_t*;
	using reference_t       = value_t&;
	using const_reference_t = const value_t&;
	using temporary_t       = value_t&;
	using const_temporary_t = const value_t&;

	using length_t  = string_utils::length_t;
	using cstring_t = const T*;

	using forward_iterator       = stl::forward_iterator<basic_arena_string<T>>;
	using const_forward_iterator = stl::const_forward_iterator<basic_arena_string<T>>;
	using reverse_iterator       = stl::reverse_iterator<basic_arena_string<T>>;
	using const_reverse_iterator = stl::const_reverse_iterator<basic_arena_string<T>>;

	using self_t = basic_arena_string<T>;

public:
	basic_arena_string(mem::arena& arena) : basic_arena_string(arena, "") {}
	basic_arena_string(mem::arena& arena, const T* cstring, length_t length = string_utils::max_length)
	{
		length = std::min(length, string_utils::calculate_length(cstring));
		if (!cstring || length == 0uz)
			return;

		m_buffer = mem::arena_box<T[]>(arena, length + 1);
		for (length_t i = 0; i < m_length; i++)
			m_buffer[i] = cstring[i];
		m_buffer[m_length] = string_utils::null_terminator<T>;

		m_length = length;
	}
	basic_arena_string(const self_t& other)
		: m_buffer(other.m_buffer.get_owner(), other.get_length() + 1)
		, m_length(other.m_length)
	{
		for (length_t i = 0uz; i < m_length; i++)
			m_buffer[i] = other.m_buffer[i];

		m_buffer[m_length] = string_utils::null_terminator<T>;
	}
	basic_arena_string(self_t&& other) noexcept
		: m_buffer(std::move(other.m_buffer))
		, m_length(other.m_length)
	{
		other.m_length = 0uz;
	}

	forward_iterator       begin()        { return { m_buffer.get() }; }
	forward_iterator       end()          { return { m_buffer.get() + m_length }; }
	const_forward_iterator begin()  const { return { m_buffer.get() }; }
	const_forward_iterator end()    const { return { m_buffer.get() + m_length }; }
	const_forward_iterator cbegin() const { return { m_buffer.get() }; }
	const_forward_iterator cend()   const { return { m_buffer.get() + m_length }; }

	reverse_iterator       rbegin()        { return { m_buffer.get() + m_length - 1 }; }
	reverse_iterator       rend()          { return { m_buffer.get() - 1 }; }
	const_reverse_iterator rbegin()  const { return { m_buffer.get() + m_length - 1 }; }
	const_reverse_iterator rend()    const { return { m_buffer.get() - 1 }; }
	const_reverse_iterator rcbegin() const { return { m_buffer.get() + m_length - 1 }; }
	const_reverse_iterator rcend()   const { return { m_buffer.get() - 1 }; }

	bool reserve(length_t new_capacity)
	{
		if (new_capacity <= get_capacity())
			return false;

		mem::arena_box<T[]> new_buffer(*m_buffer.get_owner(), new_capacity);
		for (length_t i = 0; i < m_buffer.get_length(); i++)
			new_buffer[i] = m_buffer[i];

		m_buffer = std::move(new_buffer);

		return true;
	}
	void resize(length_t new_length, T fill = string_utils::null_terminator<T>)
	{
		reserve(new_length);

		if (new_length < m_length)
		{
			m_buffer[new_length] = string_utils::null_terminator<T>;
			m_length = new_length;
		}
		else if (new_length > m_length)
		{
			pointer_t buffer = get_buffer();
			while (m_length != new_length)
				buffer[m_length++] = fill;
		}
	}

	bool      is_empty()     const { return !m_buffer || m_length == 0; }
	length_t  get_capacity() const { return m_buffer.get_capacity() / sizeof(T); }
	pointer_t get_buffer()   const { return m_buffer.get(); }
	length_t  get_length()   const { return m_length; }
	cstring_t cstring()      const { return m_buffer.get(); }

	self_t substring(length_t offset, length_t length) const
	{
		if (offset > m_length)
			return self_t();

		if (m_length - offset > length)
			length = m_length - offset;

		return self_t(cstring() + offset, length);
	}

	self_t& operator=(const self_t& other)
	{
		if (this != &other)
		{
			m_buffer = mem::buffer(other.m_buffer.get_capacity());
			m_length = other.m_length;

			for (length_t i = 0uz; i < m_length; i++)
				m_buffer[i] = other.m_buffer[i];
		}

		return *this;
	}
	self_t& operator=(self_t&& other) noexcept
	{
		if (this != &other)
		{
			m_buffer = std::move(other.m_buffer);
			m_length = other.m_length;

			other.m_length = 0uz;
		}

		return *this;
	}

	const T& operator[](size_t index) const { return m_buffer[index]; }
	T& operator[](size_t index) { return m_buffer[index]; }

	bool operator==(const self_t& other) const { return string_utils::compare(cstring(), get_length(), other.cstring(), other.get_length()) == string_utils::compare_result_t::equal;   }
	bool operator!=(const self_t& other) const { return string_utils::compare(cstring(), get_length(), other.cstring(), other.get_length()) != string_utils::compare_result_t::equal;   }
	bool operator> (const self_t& other) const { return string_utils::compare(cstring(), get_length(), other.cstring(), other.get_length()) == string_utils::compare_result_t::greater; }
	bool operator< (const self_t& other) const { return string_utils::compare(cstring(), get_length(), other.cstring(), other.get_length()) == string_utils::compare_result_t::less;    }
	bool operator>=(const self_t& other) const { return (*this > other) || (*this == other); }
	bool operator<=(const self_t& other) const { return (*this < other) || (*this == other); }

private:
	mem::arena_box<T[]> m_buffer {};
	length_t            m_length {};
};

using arena_string    = basic_arena_string<char>;
using arena_wstring   = basic_arena_string<wchar_t>;
using arena_u8string  = basic_arena_string<char8_t>;
using arena_u16string = basic_arena_string<char16_t>;
using arena_u32string = basic_arena_string<char32_t>;

}
