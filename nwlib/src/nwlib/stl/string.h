#pragma once

#include <memory>
#include <type_traits>

#include "../types.h"

namespace nw::stl {

template<typename T>
struct default_cstr { constexpr static const std::nullptr_t value = {}; };

template<> 
struct default_cstr<chr> { constexpr static const chr value[] = ""; };

template<> 
struct default_cstr<wc>  { constexpr static const wc  value[] = L""; };

template<> 
struct default_cstr<c8>  { constexpr static const c8  value[] = u8""; };

template<> 
struct default_cstr<c16> { constexpr static const c16 value[] = u""; };

template<> 
struct default_cstr<c32> { constexpr static const c32 value[] = U""; };

template<typename T>
concept character 
	=  std::is_same_v<T, chr>
	|| std::is_same_v<T, wc>
	|| std::is_same_v<T, c8>
	|| std::is_same_v<T, c16>
	|| std::is_same_v<T, c32>;

template<character T, typename TAllocator = std::allocator<T>>
class abstract_string
{
private:
	abstract_string() = default;
private:
	T*    m_buffer   {};
	usize m_length   {};
	usize m_capacity {};
};

}