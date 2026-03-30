/* SPDX-License-Identifier: Apache-2.0 */

/**
 * @file     TRingBuffer-inl.hpp
 * @brief     
 *
 * @author   Jascit (https://github.com/Jascit)
 * @date     25.03.2026
 *
 * @copyright Copyright (c) 2026 The Project Contributors
 */

#pragma once
#include "TRingBuffer.hpp"

namespace noyxcore::containers {
  template <typename T, typename Alloc>
  constexpr void TRingBuffer<T,Alloc>::push_back(T&& value) {};

  template <typename T, typename Alloc>
  constexpr void TRingBuffer<T,Alloc>::push_back(const T& value) {};

  template<typename T, typename Alloc>
  template<typename U>
  constexpr void TRingBuffer<T, Alloc>::emplace_back(U&& val) {

  }

}
