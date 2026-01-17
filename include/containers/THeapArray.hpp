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
        pointer new_mem_ = alloc_.allocate(other.size());
        //+Guard1k
        internal::uninitialized_copy_n(other.begin(), other.size(), new_mem_, alloc_);
        clear_and_deallocate();
        storage_.first_ = new_mem_;
        storage_.last_ = new_mem_ + other.size();
        capacity_ = other.size();
      }
      else
      {
        if (other.size() > size())
        {
          std::copy(other.begin(), other.begin() + size(), storage_.first_);
          uninitialized_copy_n(other.begin() + size(), other.size() - size(), storage_.last_, alloc_);
        }
        else
        {
          std::copy(other.begin(), other.begin() + other.size(), storage_.first_);
          cleanUp(storage_.first_ + other.size(), storage_.last_);
        }
        storage_.last_ = storage_.first_ + other.size();
      }
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
        steal_from((other));
        return *this;
      }
      else if (my_alloc_ == other_alloc_)
      {
        clear_and_deallocate();
        steal_from((other));
        return *this;
      }

      //POCMA = false && Allocators are not equal.
      if (capacity_ < other.size())
      {
        pointer new_mem_ = alloc_.allocate(other.size());
        //Добавити гуард777
        internal::uninitialized_move_n(other.begin(), other.size(), new_mem_, alloc_);
        clear_and_deallocate();

        storage_.first_ = new_mem_;
        storage_.last_ = new_mem_ + other.size();
        capacity_ = other.size();
      }
      else
      {
        if (other.size() > size())
        {
           std::move(other.storage_.first_, other.storage_.first_ + size(), storage_.first_);
           internal::uninitialized_move_n(other.storage_.first_ + size(), other.size() - size(), storage_.last_, alloc_);
        }
        else
        {
          std::move(other.storage_.first_, other.storage_.last_, storage_.first_);
          cleanUp(storage_.first_ + other.size(), storage_.last_);
        }
        storage_.last_ = storage_.first_ + other.size();
      }

      other.storage_.last_ = other.storage_.first_;
      return *this;
    }

    // Element access
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

    constexpr void cleanUp(pointer first, pointer last) noexcept
    {
      for (; first != last; ++first)
      {
        traits::destroy(alloc_, first);
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
        cleanUp(storage_.first_, storage_.last_);
        alloc_.deallocate(storage_.first_, capacity_);
      }
      storage_.first_ = nullptr;
      storage_.last_ = nullptr;
      capacity_ = 0;
    }

    constexpr void resize(size_type new_size)
    {
      if (new_size < size())
      {
        cleanUp(storage_.first + new_size, storage_.last_);
        storage_.last_ = storage_.first_ + new_size;
      }

      else if (new_size > size())
      {
        size_type count = new_size - size();
        if (new_size > capacity_)
        {
          pointer new_mem_ = alloc_.allocate(new_size);
          //+Guard1k
          internal::uninitialized_move_n(storage_.first_, size(), new_mem_, alloc_);
          internal::uninitialized_default_construct(new_mem_ + size(), new_mem_ + size() + count, alloc_);
          clear_and_deallocate();

          storage_.first_ = new_mem_;
          storage_.last_ = new_mem_ + new_size;
          capacity_ = new_size;
        }
        else
        {
          internal::uninitialized_default_construct(storage_.first_ + size(), storage_.last_ + count, alloc_);
          storage_.last_ = storage_.first_ + new_size;
        }
      }
    }

    void reserve(size_type new_capacity)
    {
      if (new_capacity <= capacity_) return;

      pointer new_mem_ = alloc_.allocate(new_capacity);
      size_type my_size_ = size();
      //Guard1k;
      internal::uninitialized_move_n(storage_.first_, my_size_, new_mem_, alloc_);
      clear_and_deallocate();
      storage_.first_ = new_mem_;
      storage_.last_ = new_mem_ + my_size_;
      capacity_ = new_capacity;
    }
  };
}
