#pragma once

#include <type_traits>
namespace nw::geo {

template<typename T> concept is_number = std::is_arithmetic_v<T>;

}