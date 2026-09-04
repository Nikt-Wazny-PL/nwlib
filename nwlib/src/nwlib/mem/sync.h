#pragma once

#include "ref.h"
#include <mutex>
#include <utility>

namespace nw::mem {

template<typename T>
class sync
{
public:
	sync() = default;
	sync(const ref<T>& pointer)
		: m_pointer(pointer) {}
	sync(const sync<T>&) = default;
	sync(sync<T>&&) noexcept = default;
	~sync() = default;

	template<typename TFn> // Ugly af but works (?)
	auto access(TFn&& func) -> decltype(std::declval<TFn>()(std::declval<ref<T>&>()))
	{
		std::scoped_lock<std::mutex> lock(m_mutex);
		return func(m_pointer);
	}

	template<typename... TArgs>
	static sync<T> create(TArgs&&... args) { return sync<T>(ref<T>::create(std::forward<TArgs>(args)...)); }

	sync<T>& operator=(const sync<T>&) = default;
	sync<T>& operator=(sync<T>&&) noexcept = default;

private:
	ref<T>     m_pointer {};
	std::mutex m_mutex   {};
};

}
