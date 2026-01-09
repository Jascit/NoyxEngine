/* SPDX-License-Identifier: Apache-2.0 /
/*

 * \file   THeapArray.hpp
 * \brief
 *
 * Copyright (c) 2026 MaksymRbkh
 * \author MaksymRbkh <https://github.com/MaksymRbkh>
 * \date   09.01.2026
 */

#include <memory/allocators/traits.hpp>

namespace noyxcore::containters {
  template <typename T, typename Alloc>
  class THeapArray
  {
  public:
    using allocator_type = Alloc;
    using allocator_traits = memory::allocators::allocator_traits<allocator_type>;
    using size_type = typename allocator_traits::size_type;
    using value_type = T;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using reference = value_type&;
    using const_reference = const value_type&;

    constexpr THeapArray(const allocator_type& alloc = allocator_type());
    constexpr THeapArray(size_type n, const allocator_type& alloc = allocator_type());

    constexpr THeapArray(const THeapArray& other);
    constexpr THeapArray(THeapArray&& other) noexcept;

    constexpr ~THeapArray();

    constexpr THeapArray& operator=(const THeapArray& other);
    constexpr THeapArray& operator=(THeapArray&& other);

    constexpr reference operator[](size_type index) noexcept;
    constexpr const_reference operator[](size_type index) const noexcept;
    constexpr bool empty() const noexcept { return size() == 0; };
    constexpr size_type size() const noexcept { return size_; };
    constexpr size_type capacity() const noexcept { return capacity_; };
    constexpr pointer data() noexcept { return data_; };
    constexpr const_pointer data() const noexcept { return data_; };

  };
}
