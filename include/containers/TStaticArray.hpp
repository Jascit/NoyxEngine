/* SPDX-License-Identifier: Apache-2.0 */
/* 
 * \file   TStaticArray.hpp
 * \brief  Simple fixed-capacity container that stores up to N elements of T
 * 
 * Copyright (c) 2026 Jascit 
 * \author Jascit<https://github.com/Jascit>
 * \date   07.01.2026
 */

#pragma once
#include <platform/assert.hpp>
#include <type_traits>

namespace noyxcore {
  namespace containers {

    // Simple fixed-capacity container that stores up to N elements of T
    // in an aligned char buffer and constructs elements with placement new.
    // - does NOT default-construct the T objects
    // - provides push_back / emplace_back / operator[]
    // - non-copyable, non-movable (policy can be changed)
    template<typename T, size_t N>
    class TStaticArray
    {
    public:
      using value_type = T;
      using pointer = T*;
      using const_pointer = const T*;
      using reference = T&;
      using const_reference = const T&;
      using size_type = size_t;

      TStaticArray() = default;
      TStaticArray(const TStaticArray&) = delete;
      TStaticArray(TStaticArray&&) = delete;
      
      TStaticArray(const T& val) { 
        fill_range(data(), data() + N, val); 
      };
      
      ~TStaticArray() { 
        cleanup(); 
      };
      
      reference operator[](size_type idx) noexcept {
#ifdef NOYX_CORE_DEBUG
        NOYX_CORE_ASSERT_ABORT(idx < size(), "TStaticArray: index out of bounds");
#endif
        return *(data() + idx);
      };

      const_reference operator[](size_type idx) const noexcept {
#ifdef NOYX_CORE_DEBUG
        NOYX_CORE_ASSERT_ABORT(idx < size(), "TStaticArray: index out of bounds");
#endif
        return *(data() + idx);
      };

      CONSTEXPR size_type size() const noexcept { 
        return _last - std::launder(reinterpret_cast<const_pointer>(_data)); 
      };

      CONSTEXPR bool empty() const noexcept { 
        return size() == 0; 
      };
      
      CONSTEXPR pointer data() noexcept { 
        return std::launder(reinterpret_cast<pointer>(_data)); 
      };

      CONSTEXPR const_pointer data() const noexcept { 
        return std::launder(reinterpret_cast<const_pointer>(_data)); 
      };

      CONSTEXPR void push_back(const T& val) noexcept(std::is_nothrow_copy_constructible_v<T>) {
        emplace_back(val); 
      };
      
      CONSTEXPR void push_back(T&&) noexcept(std::is_nothrow_copy_constructible_v<T> || std::is_nothrow_move_assignable_v<T>) { 
        emplace_back(std::forward<T>(val)); 
      };

    private:
      CONSTEXPR void cleanup() noexcept(std::is_nothrow_destructible_v<T>) { 
        if (_last != data()) destroy_range(data(), _last); 
      };

      CONSTEXPR void destroy_range(pointer first, pointer last) {
      
      };

      CONSTEXPR void fill_range(pointer first, pointer last, const T& val) {
      
      };
      
      template<typename Val>
      CONSTEXPR void emplace_back(Val&&) {
      
      };

    private:
      alignas(alignof(T)) char _data[sizeof(T) * N];
      pointer _last;
    };
  }
}
