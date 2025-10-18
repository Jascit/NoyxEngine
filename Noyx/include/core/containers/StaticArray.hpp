#pragma once

#include <../../platform/typedef.hpp>
#include <../../utility/utility.hpp>
#include <type_traits>
#include <memory>
#include <cstring>
#include "Iterators/iterators.hpp"

namespace noyx {
  namespace containers {

    template<typename T>
    class TStaticArray {
      using allocator_type = std::allocator<T>;// TODO: add StaticAllocator
      using rebinded_alloc = typename std::allocator_traits<allocator_type>::template rebind_alloc<T>;
      using alloc_traits = std::allocator_traits<rebinded_alloc>;

    public:
      using value_type = T;
      using pointer = typename alloc_traits::pointer;
      using const_pointer = typename alloc_traits::const_pointer;
      using size_type = typename alloc_traits::size_type;
      using difference_type = typename alloc_traits::difference_type;
      using reference = T&;
      using const_reference = const T&;
      using iterator = _StaticArrayIterator<T>;
      using const_iterator = _StaticArrayConstIterator<T>;

      static_assert(std::is_object_v<T>,
        "T must be an object type (not void, function, reference, or incomplete type)");

    public:
      // ---- ctor
      explicit TStaticArray(size_type size) : size_(size) {
        if (size_ == 0) {
          pair_.second() = nullptr;
          return;
        }

        // allocate may throw, that's fine
        pair_.second() = pair_.first().allocate(size_);

        // if trivially default constructible -> nothing to do
        if constexpr (std::is_trivially_default_constructible_v<T>) {
          return;
        }

        // otherwise construct elements with rollback on exception
        rebinded_alloc& al = pair_.first();
        size_type constructed = 0;
        try {
          for (; constructed < size_; ++constructed) {
            alloc_traits::construct(al, pair_.second() + constructed);
          }
        }
        catch (...) {
          // destroy constructed in reverse order
          for (size_type j = constructed; j > 0; --j) {
            alloc_traits::destroy(al, pair_.second() + (j - 1));
          }
          al.deallocate(pair_.second(), size_);
          pair_.second() = nullptr;
          size_ = 0;
          throw;
        }
      }

      // ---- copy ctor (exception-safe, optimized for trivially_copyable)
      TStaticArray(const TStaticArray& other) : size_(other.size_) {
        if (size_ == 0) {
          pair_.second() = nullptr;
          return;
        }

        pair_.second() = pair_.first().allocate(size_);
        rebinded_alloc& al = pair_.first();
        size_type constructed = 0;
        try {
          if constexpr (std::is_trivially_copyable_v<T>) {
            std::memcpy(pair_.second(), other.pair_.second(), static_cast<size_t>(size_) * sizeof(T));
            constructed = size_;
          }
          else {
            for (; constructed < size_; ++constructed) {
              alloc_traits::construct(al, pair_.second() + constructed, other.pair_.second()[constructed]);
            }
          }
        }
        catch (...) {
          for (size_type j = constructed; j > 0; --j) {
            alloc_traits::destroy(al, pair_.second() + (j - 1));
          }
          al.deallocate(pair_.second(), size_);
          pair_.second() = nullptr;
          size_ = 0;
          throw;
        }
      }

      // ---- move ctor
      TStaticArray(TStaticArray&& other) noexcept {
        // steal resources
        pair_ = std::move(other.pair_);
        size_ = other.size_;
        other.pair_.second() = nullptr;
        other.size_ = 0;
      }

      // ---- dtor
      ~TStaticArray() {
        if (pair_.second() != nullptr) {
          rebinded_alloc& al = pair_.first();
          if constexpr (!std::is_trivially_destructible_v<T>) {
            for (size_type i = 0; i < size_; ++i) {
              alloc_traits::destroy(al, pair_.second() + i);
            }
          }
          al.deallocate(pair_.second(), size_);
          pair_.second() = nullptr;
          size_ = 0;
        }
      }

      // ---- copy-assignment (copy-and-swap = strong exception guarantee)
      TStaticArray& operator=(const TStaticArray& other) {
        if (this == &other) return *this;
        TStaticArray tmp(other); // may throw, but strong guarantee
        swap(tmp);
        return *this;
      }

      // ---- move-assignment
      TStaticArray& operator=(TStaticArray&& other) noexcept {
        if (this == &other) return *this;

        // destroy and deallocate current storage
        _clear_and_deallocate();

        // steal other's resources
        pair_ = std::move(other.pair_);
        size_ = other.size_;
        other.pair_.second() = nullptr;
        other.size_ = 0;
        return *this;
      }

      // ---- accessors
      [[nodiscard]] reference operator[](size_type index) {
        return *(pair_.second() + index);
      }
      [[nodiscard]] const_reference operator[](size_type index) const {
        return *(pair_.second() + index);
      }
      [[nodiscard]] reference at(size_type index) {
        return *(pair_.second() + index);
      }
      [[nodiscard]] const_reference at(size_type index) const {
        return *(pair_.second() + index);
      }
      [[nodiscard]] reference front() {
        return *pair_.second();
      }
      [[nodiscard]] const_reference front() const {
        return *pair_.second();
      }
      [[nodiscard]] reference back() {
        return *(pair_.second() + size_ - 1);
      }
      [[nodiscard]] const_reference back() const {
        return *(pair_.second() + size_ - 1);
      }
      [[nodiscard]] pointer data() {
        return pair_.second();
      };
      [[nodiscard]] const_pointer data() const {
        return pair_.second();
      };

      // ---- iterators
      [[nodiscard]] iterator begin() noexcept { return iterator(pair_.second(), 0, size_); }
      [[nodiscard]] const_iterator begin() const noexcept { return const_iterator(pair_.second(), 0, size_); }
      [[nodiscard]] iterator end() noexcept { return iterator(pair_.second() + size_, size_, size_); }
      [[nodiscard]] const_iterator end() const noexcept { return const_iterator(pair_.second() + size_, size_, size_); }
      [[nodiscard]] const_iterator cbegin() const noexcept { return const_iterator(pair_.second(), 0, size_); }
      [[nodiscard]] const_iterator cend() const noexcept { return const_iterator(pair_.second() + size_, size_, size_); }

      constexpr size_type size() const noexcept { return size_; }

      // ---- fill: only for copy-assignable types
      template<typename U = T>
      std::enable_if_t<std::is_copy_assignable_v<U>, void>
        fill(const T& value) {
        for (size_type i = 0; i < size_; ++i) pair_.second()[i] = value;
      }

      // ---- swap
      void swap(TStaticArray<T>& other) noexcept(
        noexcept(std::declval<noyx::utility::TCompressedPair<allocator_type, T*>>().swap(std::declval<noyx::utility::TCompressedPair<allocator_type, T*>>()))
        )
      {
        size_type tmp2_ = other.size_;
        pair_.swap(std::move(other.pair_));
        other.size_ = size_;
        size_ = tmp2_;
      }

    private:
      // helper to destroy elements and deallocate storage
      void _clear_and_deallocate() noexcept {
        if (pair_.second() == nullptr) return;
        rebinded_alloc& al = pair_.first();
        if constexpr (!std::is_trivially_destructible_v<T>) {
          for (size_type i = 0; i < size_; ++i)
            alloc_traits::destroy(al, pair_.second() + i);
        }
        al.deallocate(pair_.second(), size_);
        pair_.second() = nullptr;
        size_ = 0;
      }

    private:
      noyx::utility::TCompressedPair<allocator_type, T*> pair_;
      size_type size_ = 0;
    };
  } // namespace containers

  namespace utility {
    template<typename T>
    constexpr void swap(containers::TStaticArray<T>& first, containers::TStaticArray<T>& second)
      noexcept(noexcept(first.swap(second))) {
      first.swap(second);
    }
  }
} // namespace noyx

using noyx::containers::TStaticArray;
