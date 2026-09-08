#pragma once

#include <cassert>
#include <cstddef>
#include <initializer_list>
#include <new>
#include <memory>
#include <type_traits>

namespace nw::stl {

template<typename T, typename TAllocator = std::allocator<T>>
class vector
{
public:
	vector() = default;
	vector(size_t length) { resize(length); }
	vector(const T* buffer, size_t length)
	{
		if (!buffer || length == 0)
			return;

		reserve(length);
		while (m_length != length)
			new (m_buffer + m_length++) T(buffer[m_length - 1]);
	}
	vector(std::initializer_list<T> list)
		: vector(list.begin(), list.size()) {}
	vector(const vector<T>& other)
		: vector(other.m_buffer, other.m_length)
		, m_allocator(other.m_allocator) {}
	vector(vector<T>&& other) noexcept
		: m_buffer(other.m_buffer)
		, m_length(other.m_length)
		, m_capacity(other.m_capacity)
		, m_allocator(other.m_allocator)
	{
		other.m_buffer   = nullptr;
		other.m_length   = 0uz;
		other.m_capacity = 0uz;
	}
	~vector()
	{
		call_dtors();

		::operator delete(m_buffer);
	}

	const T* begin()  const { return m_buffer; }
	const T* end()    const { return m_buffer + m_length; }
	T*       begin()        { return m_buffer; }
	T*       end()          { return m_buffer + m_length; }
	const T* cbegin() const { return m_buffer; }
	const T* cend()   const { return m_buffer + m_length; }

	T*     get_buffer()   const { return m_buffer; }
	size_t get_length()   const { return m_length; }
	size_t get_capacity() const { return m_capacity; }

	bool reserve(size_t new_capacity)
	{
		if (new_capacity <= m_capacity)
			return false;

		T* const new_buffer = m_allocator.allocate(new_capacity);

		for (size_t i = 0; i < m_length; i++)
		{
			if constexpr (std::is_nothrow_move_constructible_v<T>)
				new (new_buffer + i) T(std::move(m_buffer[i]));
			else
				new (new_buffer + i) T(m_buffer[i]);
		}

		call_dtors();

		m_allocator.deallocate(m_buffer, m_capacity);

		m_buffer   = new_buffer;
		m_capacity = new_capacity;

		return true;
	}
	
	void resize(size_t new_length)
	{
		if (new_length > m_length)
		{
			reserve(new_length);

			while (m_length < new_length)
				new (&m_buffer[m_length++]) T();
		}
		else if (new_length < m_length) 
		{
			if constexpr (std::is_trivially_destructible_v<T>)
			{
				m_length = new_length;
				return;
			}

			while (new_length > m_length)
				(m_buffer + --m_length)->~T();
		}
	}

	template<typename... Ts>
	T& push(Ts&&... args)
	{
		if (m_length + 1 >= m_capacity)
			reallocate();

		new (m_buffer + m_length++) T(std::forward<Ts>(args)...);
	}

	void remove(size_t index)
	{
		assert(index < m_length);

		if constexpr (!std::is_trivially_destructible_v<T>)
			(m_buffer + index)->~T();

		for (size_t i = index; i < m_length - index; i++)
		{
			if constexpr (std::is_nothrow_move_constructible_v<T>)
				new (m_buffer + i) T(std::move(m_buffer[i + 1]));
			else 
				new (m_buffer + i) T(m_buffer[i + 1]);;
		}
	}

	void swapback_remove(size_t index)
	{
		assert(index < m_length);

		if constexpr (!std::is_trivially_destructible_v<T>)
			(m_buffer + index)->~T();

		if constexpr (std::is_nothrow_move_constructible_v<T>)
			new (m_buffer + index) T(std::move(m_buffer[m_length - 1]));
		else
			new (m_buffer + index) T(m_buffer[m_length - 1]);

		m_length--;
	}

	void remove_all(auto&& predicate)
	{
		size_t* indices = new size_t[m_length];
		size_t  index_count {};

		for (size_t i = 0; i < m_length; i++)
		{
			if (predicate(m_buffer[i]))
				*(indices + index_count++) = i;
		}

		for (size_t i = 0; i < index_count; i++)
			remove(*(indices + i)); // bulk remove -- no branches

		delete[] indices;
	}

	void swapback_remove_all(auto&& predicate)
	{
		size_t* indices = new size_t[m_length];
		size_t  index_count {};

		for (size_t i = 0; i < m_length; i++)
		{
			if (predicate(m_buffer[i]))
				*(indices + index_count++) = i;
		}

		for (size_t i = 0; i < index_count; i++)
			swapback_remove(*(indices + i)); // bulk remove -- no branches

		delete[] indices;
	}

	vector<T>& operator=(const vector<T>& other)
	{
		if (this != &other)
		{
			call_dtors();
			::operator delete(m_buffer);

			if (!other.m_buffer || other.m_length == 0)
				return;

			m_length = 0;

			reserve(other.m_length);
			while (m_length != other.m_length)
				new (m_buffer + m_length++) T(other.m_buffer[m_length - 1]);

			m_allocator = other.m_allocator;
		}

		return *this;
	}
	vector<T>& operator=(vector<T>&& other) noexcept
	{
		if (this != &other)
		{
			call_dtors();
			::operator delete(m_buffer);

			m_buffer    = other.m_buffer;
			m_length    = other.m_length;
			m_capacity  = other.m_capacity;
			m_allocator = other.m_allocator;
	
			other.m_buffer   = nullptr;
			other.m_length   = 0uz;
			other.m_capacity = 0uz;
		}

		return *this;
	}

	const T& operator[](size_t index) const { assert(index < m_length); return *(m_buffer + index); }
	T&       operator[](size_t index)       { assert(index < m_length); return *(m_buffer + index); }

	vector<T>& operator+=(const T& value) { push(value);            return *this; }
	vector<T>& operator+=(T&& value)      { push(std::move(value)); return *this; }

private:
	void reallocate() { reserve(m_capacity + m_capacity / 2); }

	void call_dtors()
	{
		if constexpr (std::is_trivially_destructible_v<T>)
			return;

		for (size_t i = 0; i < m_length; i++)
			(m_buffer + i)->~T();
	}

private:
	T*         m_buffer    {};
	size_t     m_length    {};
	size_t     m_capacity  {};
	TAllocator m_allocator {};
};

}
