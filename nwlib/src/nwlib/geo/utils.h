#pragma once

#include "fwd_matrix.h"
#include "fwd_vector.h"

#include <new>
#include <cassert>

namespace nw::geo {

template<uint32_t C, uint32_t R, typename T>
constexpr uint32_t get_element_count(const matrix<C, R, T>& matrix) { (void)matrix; return C * R; }

template<uint32_t C, uint32_t R, typename T>
constexpr const T* get_value_ptr(const matrix<C, R, T>& matrix) { return std::launder((const T*)&matrix); }

template<uint32_t C, uint32_t R, typename T>
constexpr T* get_value_ptr(matrix<C, R, T>& matrix) { return std::launder((T*)&matrix); }

	

template<uint32_t N, typename T>
constexpr uint32_t get_element_count(const vector<N, T>& vector) { (void)vector; return N; }

template<uint32_t N, typename T>
constexpr const T* get_value_ptr(const vector<N, T>& vector) { return std::launder((const T*)&vector); }

template<uint32_t N, typename T>
constexpr T* get_value_ptr(vector<N, T>& vector) { return std::launder((const T*)&vector); }

}