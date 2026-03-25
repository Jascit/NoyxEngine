/* SPDX-License-Identifier: Apache-2.0 */
/*
 * \file   TRingBuffer.cpp
 * \brief  
 *
 * Copyright (c) 2026 Project Contributors
 * \author Jascit <https://github.com/Jascit>
 * \date   25.03.2026
 * \note   
 */

namespace noyxcore::containers {
  template<typename T, typename Alloc>
  class TRingBuffer {
  public:
    constexpr TRingBuffer() {  }
    constexpr ~TRingBuffer() {  }

    constexpr TRingBuffer(const TRingBuffer& ring_buffer) = delete;
    constexpr TRingBuffer(TRingBuffer&& ring_buffer) noexcept {  }

    constexpr TRingBuffer& operator=(const TRingBuffer& ring_buffer) = delete;
    constexpr TRingBuffer& operator=(TRingBuffer&& ring_buffer) noexcept {  }

    constexpr void push_back(T&& value) noexcept {}
    constexpr void push_back(T& value) noexcept {}

  private:
    template<typename U>
    constexpr void emplace_back(U&& val);
  };
}

#include <internal/impl/TRingBuffer-inl.hpp>