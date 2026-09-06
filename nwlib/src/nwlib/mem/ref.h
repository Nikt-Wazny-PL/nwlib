#pragma once

#include <atomic>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <new>
#include <type_traits>
#include <utility>

namespace nw::mem {

namespace detail {

template<typename T>
struct control_block
{
	alignas(T) uint8_t instance_memory[sizeof(T)] {};

	std::atomic<size_t> weak_count   {};
	std::atomic<size_t> strong_count {};
};

template<typename T, typename... TArgs>
control_block<T>* create_block(T*& instance, TArgs&&... args);

}

class ref_counted
{
	template<typename> friend class ref;
	template<typename T, typename... TArgs> friend detail::control_block<T>* detail::create_block(T*&, TArgs&&...);
public:
	virtual ~ref_counted() {}

private:
	void   increment_count() const {        m_control_block->strong_count.fetch_add(1, std::memory_order_relaxed); }
	bool   decrement_count() const { return m_control_block->strong_count.fetch_sub(1, std::memory_order_acq_rel) == 1; }
	size_t get_count()       const { return m_control_block->strong_count.load(std::memory_order_relaxed); }

private:
	detail::control_block<ref_counted>* m_control_block {};
};

namespace detail {

template<typename T, typename... TArgs>
inline control_block<T>* create_block(T*& instance, TArgs&&... args)
{
	static_assert(std::is_base_of_v<ref_counted, T>);

	control_block<T>* const block = new control_block<T>();
	block->weak_count   = 0uz;
	block->strong_count = 1uz;

	instance = new (block->instance_memory) T(std::forward<TArgs>(args)...);
	instance->m_control_block = std::launder((control_block<ref_counted>*)block);

	return block;
}

template<typename T>
inline void free_strong(T* pointer)
{
	static_assert(std::is_base_of_v<ref_counted, T>);
	if (!pointer)
		return;

	control_block<T>* const block = std::launder((control_block<T>*)pointer->m_control_block);

	if (pointer->decrement_count())
	{
		pointer->~T();

		if (block->weak_count.load(std::memory_order_acquire) == 0)
			delete block;
	}
}

template<typename T>
inline void free_weak(control_block<T>* block)
{
	static_assert(std::is_base_of_v<ref_counted, T>);

	if (block && block->weak_count.fetch_sub(1, std::memory_order_acq_rel) == 1)
		delete block;
}

}

template<typename T>
class ref
{
	static_assert(std::is_base_of_v<ref_counted, T>);
public:
	using type_t = T;
	using self_t = ref<T>;

public:
	ref() = default;
	ref(T* pointer) { reset(pointer); }
	ref(const self_t& other) { reset(other.m_pointer); }
	ref(self_t&& other) noexcept
		: m_pointer(other.m_pointer) { other.m_pointer = nullptr; }
	~ref() { detail::free_strong(m_pointer); }

	template<typename U>
	ref<U> as() const
	{
		static_assert(std::is_base_of_v<T, U>);
		return ref<U>(dynamic_cast<U*>(m_pointer));
	}

	void reset(T* pointer = nullptr)
	{
		if (pointer)
		{
			assert(pointer->m_control_block && "User-provided pointer must be managed by nw::mem::ref!");
			pointer->increment_count();
		}

		detail::free_strong(m_pointer);
		m_pointer = pointer;
	}

	size_t get_count() const { return m_pointer ? m_pointer->get_count() : 0; }

	T* get() const { return m_pointer; }

	template<typename... TArgs>
	static self_t create(TArgs&&... args)
	{
		self_t result {};

		detail::create_block<T>(result.m_pointer, std::forward<TArgs>(args)...);

		return result;
	}

	self_t& operator=(const self_t& other)
	{
		if (this != &other)
			reset(other.m_pointer);

		return *this;
	}
	self_t& operator=(self_t&& other) noexcept
	{
		if (this != &other)
		{
			detail::free_strong(m_pointer);
	
			m_pointer = other.m_pointer;
			other.m_pointer = nullptr;
		}

		return *this;
	}

	const T& operator*() const { assert(get() && "null pointer dereference"); return *get(); }
	T&       operator*()       { assert(get() && "null pointer dereference"); return *get(); }

	const T* operator->() const { assert(get() && "null pointer access!"); return get(); }
	T*       operator->()       { assert(get() && "null pointer access!"); return get(); }

	operator bool() const { return get_count() > 0uz; }

private:
	T* m_pointer {};
};

template<typename T>
class weak
{
private:
	weak() = default;
	weak(const ref<T>& ref)
		: m_control_block(std::launder((detail::control_block<T>*)ref.get()->m_control_block))
	{
		if (m_control_block)
			m_control_block->weak_count++;
	}
	weak(const weak<T>& other)
		: m_control_block(other.m_control_block) { if (m_control_block) m_control_block->weak_count++; }
	weak(weak<T>&& other) noexcept
		: m_control_block(other.m_control_block) { other.m_control_block = nullptr; }
	~weak() { detail::free_weak(m_control_block); }

	ref<T> make_strong() const
	{
		if (!m_control_block)
			return {};

		T* const pointer = std::launder((T*)m_control_block->instance_memory);
		return ref<T>(pointer);
	}

	size_t get_count()      const { return m_control_block ? m_control_block->strong_count : 0; }
	size_t get_weak_count() const { return m_control_block ? m_control_block->weak_count : 0; }

	weak<T>& operator=(const weak<T>& other)
	{
		if (this != &other)
		{
			detail::free_weak(m_control_block);

			m_control_block = other.m_control_block;

			if (m_control_block)
				m_control_block->weak_count++;
		}

		return *this;
	}
	weak<T>& operator=(weak<T>&& other) noexcept
	{
		if (this != &other)
		{
			detail::free_weak(m_control_block);

			m_control_block       = other.m_control_block;
			other.m_control_block = nullptr;
		}

		return *this;
	}

	operator bool() const { return get_count() > 0uz; }

private:
	detail::control_block<T>* m_control_block {};
};

}
