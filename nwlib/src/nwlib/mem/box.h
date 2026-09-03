#pragma once

#include <cassert>
#include <cstddef>
#include <utility>

namespace nw::mem {

template<typename T>
struct default_box_destroyer
{
	// pointer is guaranteed to not be null

	static void destroy(T* instance) { delete instance; }
};

template<typename T>
struct default_box_destroyer<T[]>
{
	// pointer is guaranteed to not be null

	static void destroy(T* instance) { delete[] instance; }
};


/// @brief A basic, std::unique_pointer-like "container"
/// @tparam T type of the pointer
/// @tparam TDestroyer destroyer needed to destroy the T instance instead of the default `delete #;`
template<typename T, typename TDestroyer = default_box_destroyer<T>>
class box
{
public:
	using destroyer_t = TDestroyer;
	using type_t      = T;
	using self_t      = box<T, TDestroyer>;

public:
	box() = default;
	box(T* pointer)
		: m_pointer(pointer) {}
	box(const self_t& other) = delete;
	box(self_t&& other) noexcept
		: m_pointer(other.get()) { other.m_pointer = nullptr; }
	~box() { if (m_pointer) destroyer_t::destroy(m_pointer); }

	void reset(T* pointer = nullptr)
	{
		if (m_pointer)
			destroyer_t::destroy(m_pointer);
			
		m_pointer = pointer;
	}

	T* get() const { return m_pointer; }

	template<typename... TArgs>
	static self_t create(TArgs&&... args) { return self_t(new T(std::forward<TArgs>(args)...)); }

	self_t& operator=(const self_t& other) = delete;
	self_t& operator=(self_t&& other) noexcept
	{
		if (this != &other)
		{
			reset(other.m_pointer);
			other.m_pointer = nullptr;
		}

		return *this;
	}

	const T& operator*() const { assert(get() && "null pointer dereference"); return *get(); }
	T&       operator*()       { assert(get() && "null pointer dereference"); return *get(); }

	const T* operator->() const { assert(get() && "null pointer access!"); return get(); }
	T*       operator->()       { assert(get() && "null pointer access!"); return get(); }

	operator bool() const { return get() != nullptr; }

private:
	T* m_pointer {};
};

/// @brief A basic, std::unique_pointer-like "container" with no index checking
/// @tparam T type of the pointer
/// @tparam TDestroyer destroyer needed to destroy the T instances instead of the default `delete[] #;`
template<typename T, typename TDestroyer>
class box<T[], TDestroyer>
{
public:
	using destroyer_t = TDestroyer;
	using type_t      = T[];
	using self_t      = box<T[], TDestroyer>;

public:
	box() = default;
	box(T* pointer)
		: m_pointer(pointer) {}
	box(const self_t& other) = delete;
	box(self_t&& other) noexcept
		: m_pointer(other.get()) { other.m_pointer = nullptr; }
	~box() { if (m_pointer) destroyer_t::destroy(m_pointer); }

	void reset(T* pointer = nullptr)
	{
		if (m_pointer)
			destroyer_t::destroy(m_pointer);
			
		m_pointer = pointer;
	}

	T* get() const { return m_pointer; }

	template<typename... TArgs>
	static self_t create(size_t length) { return self_t(new T[length]); }

	self_t& operator=(const self_t& other) = delete;
	self_t& operator=(self_t&& other) noexcept
	{
		if (this != &other)
		{
			reset(other.m_pointer);
			other.m_pointer = nullptr;
		}

		return *this;
	}

	const T& operator[](size_t index) const { assert(get() && "null pointer index"); return *(get() + index); }
	T&       operator[](size_t index)       { assert(get() && "null pointer index"); return *(get() + index); }

	const T& operator*() const { assert(get() && "null pointer dereference"); return *get(); }
	T&       operator*()       { assert(get() && "null pointer dereference"); return *get(); }

	const T* operator->() const { assert(get() && "null pointer access!"); return get(); }
	T*       operator->()       { assert(get() && "null pointer access!"); return get(); }

	operator bool() const { return get() != nullptr; }

private:
	T* m_pointer {};
};

}
