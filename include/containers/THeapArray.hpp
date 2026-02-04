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
#include <compare>
#include <platform/assert.hpp>
#include <utility>
#include <algorithm>
#include <stdexcept>

namespace noyxcore::containers {
  template <typename T, typename Alloc>
  class THeapArray
  {
  public:
    using allocator_type = Alloc;
    using value_type = T;
    using size_type = std::size_t;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using traits = std::allocator_traits<allocator_type>;
    using reference = value_type&;
    using const_reference = const value_type&;
    using iterator = pointer;
    using const_iterator = const_pointer;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    struct Storage
    {
      pointer first_;
      pointer last_;
    };

    constexpr THeapArray(const allocator_type& alloc = allocator_type())
      : storage_{ nullptr, nullptr }
      , capacity_(0)
      , alloc_(std::move(alloc))
    {
    };

    constexpr THeapArray(std::initializer_list<value_type> list, const allocator_type& alloc = allocator_type())
      : THeapArray(alloc)
    {
      reserve(list.size());
      internal::uninitialized_copy(list.begin(), list.end(), storage_.first_, alloc_);
      storage_.last_ = storage_.first_ + list.size();
    }

    constexpr THeapArray(size_type n, const allocator_type& alloc = allocator_type())
      : storage_{ nullptr, nullptr }
      , capacity_(n)
      , alloc_(std::move(alloc))
    {
      if (n > 0)
      {
        storage_.first_ = alloc_.allocate(n);
        internal::uninitialized_fill_n(storage_.first_, n, value_type());
        storage_.last_ = storage_.first_ + n;
      }
    }

    constexpr THeapArray(const THeapArray& other)
      : storage_{ nullptr, nullptr }
      , capacity_(other.capacity_) 
      , alloc_(std::move(other.alloc_))
    {
      if (capacity_ > 0)
      {
        storage_.first_ = alloc_.allocate(capacity_);
        storage_.last_ = internal::uninitialized_copy(other.begin(), other.end(), storage_.first_);
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

    constexpr THeapArray& operator=(std::initializer_list<value_type> list)
    {
      THeapArray tmp(list, alloc_);
      *this = std::move(tmp);
      return *this;
    }
    friend constexpr bool operator==(const THeapArray& a, const THeapArray& b)
    {
      if (a.size() != b.size()) return false;
      return std::equal(a.begin(), a.end(), b.begin());
    }
    friend constexpr auto operator<=>(const THeapArray& a, const THeapArray& b)
    {
      return std::lexicographical_compare_three_way(a.begin(), a.end(), b.begin(), b.end());
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

    constexpr void swap(THeapArray& other) noexcept
    {
      constexpr bool POCS = traits::propagate_on_container_swap::value;
      if constexpr (POCS)
      {
        std::swap(alloc_, other.alloc_);
      }
      else
      {
        NOYX_CORE_ASSERT_ABORT(alloc_ == other.alloc_, "Cannot swap containers with different allocators if POCS is false");
      }
      std::swap(storage_, other.storage_);
      std::swap(capacity_, other.capacity_);
    }
    friend constexpr void swap(THeapArray& lhs, THeapArray& rhs) noexcept
    {
      lhs.swap(rhs);
    }

    // Element access
    constexpr reference operator[](size_type index) noexcept
    {
      NOYX_CORE_ASSERT_ABORT(index < size(), "Index out of range");
      return storage_.first_[index];
    }
    constexpr const_reference operator[](size_type index) const noexcept
    {
      NOYX_CORE_ASSERT_ABORT(index < size(), "Index out of range");
      return storage_.first_[index];
    }
    constexpr reference at(size_type index)
    {
      if (index >= size())
      {
        throw std::out_of_range("Index out of range");
      }
      return storage_.first_[index];
    }
    constexpr const_reference at(size_type index) const
    {
      if (index >= size())
      {
        throw std::out_of_range("Index out of range");
      }
      return storage_.first_[index];
    }
    constexpr reference front() noexcept
    {
      NOYX_CORE_ASSERT_ABORT(!empty(), "Array is empty");
      return *storage_.first_;
    }
    constexpr const_reference front() const noexcept
    {
      NOYX_CORE_ASSERT_ABORT(!empty(), "Array is empty");
      return *storage_.first_;
    }
    constexpr reference back() noexcept
    {
      NOYX_CORE_ASSERT_ABORT(!empty(), "Array is empty");
      return *(storage_.last_ - 1);
    }
    constexpr const_reference back() const noexcept
    {
      NOYX_CORE_ASSERT_ABORT(!empty(), "Array is empty");
      return *(storage_.last_ - 1);
    }
    constexpr size_type size() const noexcept
    {
      return static_cast<size_type>(storage_.last_ - storage_.first_);
    }
    constexpr size_type capacity() const noexcept
    {
      return capacity_;
    }
    [[nodiscard]] constexpr bool empty() const noexcept { return this->begin() == this->end(); }
    constexpr pointer data() noexcept { return this->begin(); }
    constexpr const_pointer data() const noexcept { return this->begin(); }
    constexpr allocator_type get_allocator() const noexcept { return this->alloc_; }



    // Iterators
    constexpr iterator begin() noexcept {return storage_.first_;}
    constexpr const_iterator begin() const noexcept {return storage_.first_;}
    constexpr iterator end() noexcept {return storage_.last_;}
    constexpr const_iterator end() const noexcept {return storage_.last_;}
    constexpr reverse_iterator rbegin() noexcept {return reverse_iterator(end());}
    constexpr const_reverse_iterator rbegin() const noexcept {return const_reverse_iterator(end());}
    constexpr reverse_iterator rend() noexcept {return reverse_iterator(begin());}
    constexpr const_reverse_iterator rend() const noexcept {return const_reverse_iterator(begin());}
    constexpr const_iterator cbegin() const noexcept {return storage_.first_;}
    constexpr const_iterator cend() const noexcept {return storage_.last_;}
    constexpr const_reverse_iterator crbegin() const noexcept {return const_reverse_iterator(rbegin());}
    constexpr const_reverse_iterator crend() const noexcept {return const_reverse_iterator(rend());}



    // Modifiers
    constexpr iterator erase(const_iterator pos)
    {
      size_type index = pos - cbegin();
      pointer p = storage_.first_ + index;
      std::move(p + 1, storage_.last_, p);
      traits::destroy(alloc_, storage_.last_ - 1);
      --storage_.last_;
      return make_iter(p);
    }
    constexpr iterator erase(const_iterator first, const_iterator last)
    {
      NOYX_CORE_ASSERT_ABORT(first <= last, "vector::erase(first, last) called with invalid range");
      if (first == last)
      {
        return iterator(storage_.first_ + (first - cbegin()));
      }
      size_type index_first = first - cbegin();
      pointer p_first = storage_.first_ + index_first;
      size_type index_last = last - cbegin();
      pointer p_last = storage_.first_ + index_last;

      pointer new_end = std::move(p_last, storage_.last_, p_first);
      cleanUp(new_end, storage_.last_);
      storage_.last_ = new_end;
      return make_iter(p_first);
    }

    constexpr iterator insert(const_iterator pos, const_reference value)
    {
      size_type index = pos - cbegin();
      if (size() == capacity_)
      {
        reserve(capacity_ == 0 ? 1 : capacity_ * 2);
      }
      pointer p = storage_.first_ + index;
      if (p == storage_.last_)
      {
        push_back(value);
      }
      else
      {
        traits::construct(alloc_, storage_.last_, std::move(*(storage_.last_ - 1)));
        std::move_backward(p, storage_.last_ - 1, storage_.last_);
        *p = value;
        ++storage_.last_;
      }
      return make_iter(p);
    }

    constexpr void assign(size_type n, const_reference value)
    {
      if (n <= capacity_)
      {
        if (n <= size())
        {
          std::fill_n(begin(), n, value);
          cleanUp(storage_.first_ + n, storage_.last_);
        }
        else
        {
          std::fill(begin(), end(), value);
          internal::uninitialized_fill_n(storage_.last_, n - size(), value, alloc_);
        }
        storage_.last_ = storage_.first_ + n;
      }
      else
      {
        pointer new_mem_ = alloc_.allocate(n);
        //+guard
        internal::uninitialized_fill_n(new_mem_, n, value, alloc_);
        clear_and_deallocate();
        storage_.first_ = new_mem_;
        storage_.last_ = new_mem_ + n;
        capacity_ = n;
      }
    }

    constexpr void push_back(const_reference n)
    {
      emplace_back(n);
    }

    constexpr void push_back(value_type&& n)
    {
      emplace_back(std::move(n));
    }

    template <class... Args>
    constexpr iterator emplace(const_iterator pos, Args&&... args)
    {
      size_type index = pos - cbegin();
      if (size() == capacity_)
      {
        reserve(capacity_ == 0 ? 1 : capacity_ * 2);
      }
      pointer p = storage_.first_ + index;
      if (p == storage_.last_)
      {
        emplace_back(std::forward<Args>(args)...);
      }
      else
      {
        traits::construct(alloc_, storage_.last_, std::move(*(storage_.last_ - 1)));
        std::move_backward(p, storage_.last_ - 1, storage_.last_);
        traits::destroy(alloc_, p);
        traits::construct(alloc_, p, std::forward<Args>(args)...);
        ++storage_.last_;
      }
      return make_iter(p);
    }

    template <class... Args>
    constexpr void emplace_back(Args&&... args)
    {
      if (size() == capacity_)
      {
        reserve(capacity_ == 0 ? 1 : capacity_ * 2);
      }
      traits::construct(alloc_, storage_.last_, std::forward<Args>(args)...);
      ++storage_.last_;
    }

    constexpr void pop_back()
    {
      NOYX_CORE_ASSERT_ABORT(!empty(), "Array is empty");
      --storage_.last_;
      traits::destroy(alloc_, storage_.last_);
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

    constexpr void shrink_to_fit()
    {
      if (capacity_ > size())
      {
        THeapArray temp(*this);
        swap(temp);
      }
    }

    constexpr void clear()
    {
      cleanUp(storage_.first_, storage_.last_);
      storage_.last_ = storage_.first_;
    }

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
      }
      if (storage_.first != nullptr)
      {
        alloc_.deallocate(storage_.first_, capacity_);
      }
      storage_.first_ = nullptr;
      storage_.last_ = nullptr;
      capacity_ = 0;
    }

    constexpr iterator make_iter(pointer p) noexcept {return iterator(p);}
    constexpr const_iterator make_iter(const_pointer p) noexcept {return const_iterator(p);}
  };
}
