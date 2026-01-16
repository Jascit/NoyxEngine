/* SPDX-License-Identifier: Apache-2.0 /
/*
 * \file   THeapArray.hpp
 * \brief
 *
 * Copyright (c) 2026 Project Contributors
 * \author MaksymRbkh <https://github.com/MaksymRbkh>
 * \date   09.01.2026
 */

#include <containers/internal/containers_internal.hpp>
#include <cassert>
#include <memory/allocators/traits.hpp>
#include <memory>

namespace noyxcore::containers {
  template <typename T, typename Alloc>
  class THeapArray
  {
  public:
    using allocator_type = Alloc;
    using value_type = T;
    using size_type = std::size_t;
    using pointer = value_type*;
    using traits = std::allocator_traits<allocator_type>;
    using const_pointer = const value_type*;
    using reference = value_type&;
    using const_reference = const value_type&;
    using iterator = pointer;
    using const_iterator = const_pointer;

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


  // Copy Assignment
    constexpr THeapArray& operator=(const THeapArray& other)
    {
      if (this == &other) return *this;
      auto& my_alloc_ = alloc_;
      const auto& other_alloc_ = other.alloc_;

      constexpr bool POCCA = traits::propagate_on_container_copy_assignment::value;
      if constexpr (POCCA)
      {
        if (alloc_ != other_alloc_)
        {
          clear_and_deallocate();
          alloc_ = other_alloc_;
        }
      }

      if (capacity_ < other.size())
      {
        clear_and_deallocate();
        storage_.first_ = alloc_.allocate(other.size());
        capacity_ = other.size();
      }
      else
      {
        clean_up();
      }

      storage_.last = noyxcore::containers::internal::uninitialized_copy(other.begin(), other.end(), storage_.first_, alloc_);
      return *this;
    }

    // Move Assignment
    constexpr THeapArray& operator=(THeapArray&& other) noexcept
    {
      if (&other == this) return *this;
      auto& my_alloc_ = alloc_;
      auto& other_alloc_ = other.alloc_;
      constexpr bool POCMA = traits::propagate_on_container_move_assignment::value;

      if constexpr (POCMA)
      {
        clear_and_deallocate();
        alloc_ = std::move(other_alloc_);
        steal_from(&other);
        return *this;
      }
      else if constexpr (traits::is_always_equal::value)
      {
        clear_and_deallocate();
        steal_from((&other));
        return *this;
      }
      else if (my_alloc_ == other_alloc_)
      {
        clear_and_deallocate();
        steal_from((&other));
        return *this;
      }

      pointer new_mem_ = alloc_.allocate(other.size());
      pointer new_last_ = noyxcore::containers::internal::uninitialized_move_n(other.begin(), other.size(), new_mem_, alloc_);
      clear_and_deallocate();

      storage_.first = new_mem_;
      storage_.last = new_last_;
      capacity_ = other.size();

      other.storage_.last_ = other.storage_.first_;
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

    // Iterators
    constexpr iterator begin() noexcept {return storage_.first_;}
    constexpr iterator end() noexcept {return storage_.last_;}
    constexpr const_iterator begin() const noexcept {return storage_.first_;}
    constexpr const_iterator end() const noexcept {return storage_.last_;}
    constexpr const_iterator cbegin() const noexcept {return storage_.first_;}
    constexpr const_iterator cend() const noexcept {return storage_.last_;}


  private:
    Storage storage_;
    size_type capacity_;
    allocator_type alloc_;

    constexpr void clean_up()
    {
      if (!empty())
      {
        if constexpr (!std::is_trivially_destructible_v<value_type>)
        {
          std::destroy(storage_.first_, storage_.last_);
        }
      }
    }

    constexpr void steal_from(THeapArray& other)
    {
      storage_ = other.storage_;
      capacity_ = std::exchange(other.capacity_, 0);
      other.storage_ = {nullptr, nullptr};
    }

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
