/* SPDX-License-Identifier: Apache-2.0 */

/**
 * @file     TRingBuffer.cpp
 * @brief     
 *
 * @author   Jascit (https://github.com/Jascit)
 * @date     25.03.2026
 *
 * @copyright Copyright (c) 2026 The Project Contributors
 */

#include <memory/allocators/traits.hpp>

namespace noyxcore::containers {
  template <typename T, typename Alloc>
  class TRingBuffer {
  public:
    using size_type = size_t;
    using value_type = T;
    using allocator_type = Alloc;
    using allocator_traits = memory::allocators::allocator_traits<allocator_type>;
    using iterator = typename allocator_traits::iterator;
    using const_iterator = typename allocator_traits::const_iterator;

    constexpr TRingBuffer();
    constexpr ~TRingBuffer();

    constexpr TRingBuffer(size_type capacity);
    constexpr TRingBuffer(const TRingBuffer& ring_buffer) = delete;
    constexpr TRingBuffer(TRingBuffer&& ring_buffer) noexcept;

    constexpr TRingBuffer& operator=(const TRingBuffer& ring_buffer) = delete;
    constexpr TRingBuffer& operator=(TRingBuffer&& ring_buffer) noexcept;

    constexpr void push_back(T&& value);
    constexpr void push_back(T& value);

    constexpr T& pop_back();
    constexpr T& pop_front();

    constexpr iterator begin();
    constexpr iterator end();
    constexpr const_iterator begin() const;
    constexpr const_iterator end() const;
    constexpr const_iterator cbegin() const;
    constexpr const_iterator cend() const;

    constexpr bool empty() const;
    constexpr size_type size() const;
    constexpr size_type capacity() const;

    constexpr allocator_type& get_allocator() const;

  private:
    template<typename U>
    constexpr void emplace_back(U&& val);

  private:
    struct storage {
      constexpr storage() = default;
      constexpr storage(size_type capacity, T* buffer) noexcept;

      constexpr void swap_with(storage& other) noexcept;
      constexpr void steal_from(storage& other) noexcept;

      size_type capacity;
      size_type size;
      T* buffer;
      T* first;
      T* last;
      T* end;
    };
    storage internal_storage_;
    [[no_unique_address]] allocator_type allocator_;
  };
}
