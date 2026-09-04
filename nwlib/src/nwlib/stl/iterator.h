#pragma once

#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace nw::stl {

enum class iterator_type : uint8_t { forward, reverse };

namespace detail {

template<typename T, typename = void>
struct value_type
{
	using type = typename T::value_t;
};

template<typename T>
struct value_type<T, std::void_t<typename T::value_type>>
{
	using type = typename T::value_type; 
};

template<typename T>
using value_type_t = typename value_type<T>::type;

using offset_t = std::make_signed_t<size_t>;

constexpr offset_t sign(iterator_type type) { return type == iterator_type::forward ? +1 : -1; }

}

template<typename TIterable, iterator_type Type>
class iterator
{
public:
	using container_t = TIterable;

	using self_t   = iterator<TIterable, Type>;
	using offset_t = detail::offset_t;

	using value_t           = detail::value_type_t<container_t>;
	using const_value_t     = const value_t;
	using pointer_t         = value_t*;
	using const_pointer_t   = const value_t*;
	using reference_t       = value_t&;
	using const_reference_t = const value_t&;
	using temporary_t       = value_t&&;
	using const_temporary_t = const value_t&&;

public:
	iterator() = delete;
	explicit iterator(pointer_t pointer)
		: m_pointer(pointer) {}
	iterator(const self_t& other) = default;
	iterator(self_t&& other) noexcept = default;

	self_t& operator+=(offset_t offset)       { m_pointer += offset * detail::sign(Type); return *this; }
	self_t  operator+ (offset_t offset) const { return self_t(m_pointer) += offset; }
	self_t& operator++()                      { return *this += 1z; }
	self_t  operator++(int)                   { return self_t(m_pointer++); }

	self_t& operator-=(offset_t offset)       { m_pointer -= offset * detail::sign(Type); return *this; }
	self_t  operator- (offset_t offset) const { return self_t(m_pointer) -= offset; }
	self_t& operator--()                      { return *this -= 1z; }
	self_t  operator--(int)                   { return self_t(m_pointer--); }

	self_t& operator=(const self_t& other) = default;
	self_t& operator=(self_t&& other) noexcept = default;

	auto operator<=>(const self_t& other) const = default;

	const_pointer_t operator->() const { return m_pointer; }
	pointer_t       operator->()       { return m_pointer; }

	const_reference_t operator*() const { return *m_pointer; }
	reference_t       operator*()       { return *m_pointer; }
	
private:
	pointer_t m_pointer {};
};

template<typename TIterable> 
using forward_iterator = iterator<TIterable, iterator_type::forward>;

template<typename TIterable> 
using reverse_iterator = iterator<TIterable, iterator_type::reverse>;

}
