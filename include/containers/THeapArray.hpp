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
#include <containers/internal/containers_internal.hpp>
#include <memory>

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

    constexpr THeapArray(const allocator_type& alloc = allocator_type()) : alloc_(alloc), size_(0), capacity_(0), data_(nullptr) {};

    constexpr THeapArray(size_type n, const allocator_type& alloc = allocator_type()) : alloc_(alloc), size_(n), capacity_(n), data_(nullptr)
    {
        data_ = allocator_traits::allocate(alloc_, n);
        uninitialized_fill_n(data(), n, value_type(), alloc_);
    }

    constexpr THeapArray(const THeapArray& other) : alloc_(other.alloc_), size_(other.size_), capacity_(other.capacity_), data_(nullptr)
    {
        data_ = allocator_traits::allocate(alloc_, capacity_);
        uninitialized_copy_n(other.data_, other.size_, data(), alloc_);
    }


    constexpr THeapArray(THeapArray&& other) noexcept : alloc_(std::move(other.alloc_)), size_(other.size_), capacity_(other.capacity_), data_(other.data_)
    {
        other.size_ = 0;
        other.capacity_ = 0;
        other.data_ = nullptr;
    }

    constexpr ~THeapArray()
    {
        if (!empty())
        {
            destroyRange(data(), data_ + size());
        }
        allocator_traits::deallocate(alloc_, data(), capacity_);
    }

    constexpr THeapArray& operator=(const THeapArray& other)
    {
        if (&other == this) return *this;

        if (!empty())
        {
            destroyRange(data(), data_ + size());
        }

        if (other.size_ > capacity_)
        {
            allocator_traits::deallocate(alloc_, data_, capacity_);

            data_ = allocator_traits::allocate(alloc_, other.size_);
            capacity_ = other.size_;
        }
        size_ = other.size_;
        uninitialized_copy_n(other.data_, other.size_, data_, alloc_);

        return *this;
    }


    constexpr THeapArray& operator=(THeapArray&& other)
    {
        if (&other == this) return *this;

        std::swap(alloc_, other.alloc_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
        std::swap(data_, other.data_);

        return *this;
    }

    constexpr reference operator[](size_type index) noexcept { return data_[index]; };
    constexpr const_reference operator[](size_type index) const noexcept { return data_[index]; };
    constexpr bool empty() const noexcept { return size() == 0; };
    constexpr size_type size() const noexcept { return size_; };
    constexpr size_type capacity() const noexcept { return capacity_; };
    constexpr pointer data() noexcept { return data_; };
    constexpr const_pointer data() const noexcept { return data_; };


  private:
      allocator_type alloc_;
      size_type size_;
      size_type capacity_;
      pointer data_;

      constexpr void destroyRange(pointer first_, pointer last_) noexcept(std::is_nothrow_destructible_v<value_type>)
      {
          if constexpr (!std::is_trivially_destructible_v<value_type>)
          {
              for (pointer i = first_; i < last_; ++i)
              {
                  std::destroy_at(i);
              }
          }
      }
  };
}
