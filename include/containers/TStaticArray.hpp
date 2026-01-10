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
#include <containers/internal/containers_internal.hpp>
#include <platform/assert.hpp>
#include <type_traits>
#include <memory>

namespace noyxcore::containers {

  // Simple fixed-capacity container that stores up to N elements of T
  // in an aligned char buffer and constructs elements with placement new.
  // - does NOT default-construct the T objects
  // - provides pushBack / emplaceBack / operator[]
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

    constexpr TStaticArray() : last_(data()) {};
    TStaticArray(const TStaticArray&) = delete;
    TStaticArray(TStaticArray&&) = delete;

    constexpr TStaticArray(const T& val) {
      last_ = internal::uninitialized_fill_n(data(), N, val, memory::allocators::no_alloc<T>{});
    };

    constexpr ~TStaticArray() {
      cleanup();
    };

    constexpr reference operator[](size_type idx) noexcept {
#ifdef NOYX_CORE_DEBUG
      NOYX_CORE_ASSERT_ABORT(idx < size(), "TStaticArray: index out of bounds");
#endif
      return data()[idx];
    };

    constexpr const_reference operator[](size_type idx) const noexcept {
#ifdef NOYX_CORE_DEBUG
      NOYX_CORE_ASSERT_ABORT(idx < size(), "TStaticArray: index out of bounds");
#endif
      return data()[idx];
    };

    [[nodiscard]] constexpr size_type size() const noexcept {
      return last_ - std::launder(reinterpret_cast<const_pointer>(data_));
    };

    [[nodiscard]] constexpr bool empty() const noexcept {
      return size() == 0;
    };

    constexpr pointer data() noexcept {
      return std::launder(reinterpret_cast<pointer>(data_));
    };

    constexpr const_pointer data() const noexcept {
      return std::launder(reinterpret_cast<const_pointer>(data_));
    };

    constexpr void pushBack(const value_type& val) noexcept(std::is_nothrow_copy_constructible_v<value_type>) {
      emplaceBack(val);
    };

    constexpr void pushBack(value_type&& val) noexcept(std::is_nothrow_copy_constructible_v<value_type> || std::is_nothrow_move_assignable_v<value_type>) {
      emplaceBack(std::forward<value_type>(val));
    };

    template<typename... Args>
    constexpr reference emplaceBack(Args&&... args) {
      size_type current_size = size();
      NOYX_CORE_ASSERT_ABORT(current_size < N, "TStaticArray::emplaceBack: capacity exceeded");
      new (static_cast<void*>(last_)) T(std::forward<Args>(args)...);
      ++last_;
      return *(last_ - 1);
    };

  private:
    constexpr void cleanup() noexcept(std::is_nothrow_destructible_v<value_type>) {
      if (last_ != data()) destroyRange(data(), last_);
    };

    constexpr void destroyRange(pointer first, pointer last) noexcept(std::is_nothrow_destructible_v<value_type>) {
      if constexpr (!std::is_trivially_destructible_v<value_type>) {
        for (pointer it = first; it < last; it++)
        {
          std::destroy_at(it);
        }
      }
    };
    

  private:
    alignas(alignof(value_type)) char data_[sizeof(value_type) * N];
    pointer last_;
  };
}
