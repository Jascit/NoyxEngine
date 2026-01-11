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
    using value_type = T;
    using size_type = std::size_t;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using reference = value_type&;
    using const_reference = const value_type&;

    struct Storage
    {
      pointer first_;
      pointer last_;
    };

    constexpr THeapArray(const allocator_type& alloc = allocator_type())
      : storage_{ nullptr, nullptr }
      , capacity_(0)
      , alloc_(alloc)
    {
    };

    constexpr THeapArray(size_type n, const allocator_type& alloc = allocator_type())
      : storage_{ nullptr, nullptr }
      , capacity_(n)
      , alloc_(alloc)
    {
      if (n > 0)
      {
        storage_.first_ = alloc_.allocate(n);
        noyxcore::containers::internal::uninitialized_fill_n(storage_.first_, n, value_type());
        storage_.last_ = storage_.first_ + n;
      }
    }

    constexpr THeapArray(const THeapArray& other)
      : storage_{ nullptr, nullptr }
      , capacity_(other.capacity_) 
      , alloc_(other.alloc_)
    {
      if (capacity_ > 0)
      {
        storage_.first_ = alloc_.allocate(capacity_);
        storage_.last_ = noyxcore::containers::internal::uninitialized_copy(other.begin(), other.end(), storage_.first_);
      }
    }

    constexpr THeapArray(THeapArray&& other) noexcept
      : storage_{ other.storage_.first_, other.storage_.last_ } 
      , capacity_(other.capacity_)
      , alloc_(std::move(other.alloc_))
    {
      other.storage_.first_ = nullptr;
      other.storage_.last_ = nullptr;
      other.capacity_ = 0;
    }

    constexpr ~THeapArray()
    {
      clear_and_deallocate();
    }



    constexpr THeapArray& operator=(const THeapArray& other)
    {
      if (&other == this) return *this;

      THeapArray temp(other);
      swap(temp);

      return *this;
    }

    constexpr THeapArray& operator=(THeapArray&& other) noexcept
    {
      if (&other == this) return *this;
      swap(other);

      return *this;
    }

    constexpr reference operator[](size_type index) noexcept
    {
      assert(index < size() && "Index out of range");
      return storage_.first_[index];
    }

    constexpr const_reference operator[](size_type index) const noexcept
    {
      assert(index < size() && "Index out of range");
      return storage_.first_[index];
    }

    constexpr size_type size() const noexcept
    {
      return static_cast<size_type>(storage_.last_ - storage_.first_);
    }

    constexpr size_type capacity() const noexcept { return capacity_; }
    constexpr bool empty() const noexcept { return storage_.first_ == storage_.last_; }
    constexpr pointer data() noexcept { return storage_.first_; }
    constexpr const_pointer data() const noexcept { return storage_.first_; }

    constexpr void swap(THeapArray& other) noexcept
    {
      std::swap(storage_, other.storage_);
      std::swap(capacity_, other.capacity_);
      std::swap(alloc_, other.alloc_);
    }

  private:
    Storage storage_;
    size_type capacity_;
    allocator_type alloc_;

    constexpr void clear_and_deallocate()
    {
      if (!empty())
      {
        if constexpr (!std::is_trivially_destructible_v<value_type>)
        {
          std::destroy(storage_.first_, storage_.last_);
        }
      }

      if (capacity_ > 0)
      {
        alloc_.deallocate(storage_.first_, capacity_);
      }
    }
  };
}
