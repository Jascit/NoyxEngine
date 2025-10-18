#pragma once
#include <array>
#include "../../../platform/typedef.hpp"

namespace noyx {
  namespace containers {
    //slotmap init
    struct _IteratorBase {

    };

    template<typename T>
    class _StaticArrayConstIterator {
    public:
      using iterator_category = std::random_access_iterator_tag;
      using value_type = T;
      using difference_type = std::ptrdiff_t;
      using pointer = const T*;
      using reference = const T&;
      using size_type = noyx::size_t;

    private:
      pointer ptr_;
      size_type pos_; // TODO: debug, metadata, add checks
      size_type size_; // TODO: debug, metadata, add checks

    public:
      constexpr _StaticArrayConstIterator() noexcept : ptr_(nullptr), pos_(0), size_(0) {}
      constexpr explicit _StaticArrayConstIterator(pointer p, size_type pos, size_type size) noexcept : ptr_(p), pos_(pos), size_(size) {}

      // copy / assign defaulted
      constexpr _StaticArrayConstIterator(const _StaticArrayConstIterator&) noexcept = default;
      constexpr _StaticArrayConstIterator& operator=(const _StaticArrayConstIterator& o) noexcept {
        if (this != &o)
        {
          ptr_ = o.ptr_;
          pos_ = o.pos_;
          size_ = o.size_;
        }
        return *this;
      };

      constexpr _StaticArrayConstIterator& operator=(_StaticArrayConstIterator&& o) noexcept {
        if (this != &o)
        {
          ptr_ = o.ptr_;
          pos_ = o.pos_;
          size_ = o.size_;
          o.ptr_ = nullptr; // TODO: add checks 
          o.pos_ = 0; // TODO: add checks
          o.size_ = 0;
        }
        return *this;
      };

      // access
      constexpr reference operator*() const noexcept { return *ptr_; }
      constexpr pointer   operator->() const noexcept { return ptr_; }

      // increment / decrement
      constexpr _StaticArrayConstIterator& operator++() noexcept { ++ptr_; return *this; }
      constexpr _StaticArrayConstIterator  operator++(int) noexcept { auto tmp = *this; ++ptr_; return tmp; }

      constexpr _StaticArrayConstIterator& operator--() noexcept { --ptr_; return *this; }
      constexpr _StaticArrayConstIterator  operator--(int) noexcept { auto tmp = *this; --ptr_; return tmp; }

      // arithmetic
      constexpr _StaticArrayConstIterator& operator+=(difference_type n) noexcept { ptr_ += n; return *this; }
      constexpr _StaticArrayConstIterator& operator-=(difference_type n) noexcept { ptr_ -= n; return *this; }

      friend constexpr _StaticArrayConstIterator operator+(const _StaticArrayConstIterator& it, difference_type n) noexcept {
        _StaticArrayConstIterator tmp = it;
        tmp += n;
        return tmp;
      }
      friend constexpr _StaticArrayConstIterator operator+(difference_type n, const _StaticArrayConstIterator& it) noexcept {
        return it + n;
      }
      friend constexpr _StaticArrayConstIterator operator-(const _StaticArrayConstIterator& it, difference_type n) noexcept {
        _StaticArrayConstIterator tmp = it;
        tmp -= n;
        return tmp;
      }

      // difference between iterators
      friend constexpr difference_type operator-(const _StaticArrayConstIterator& a, const _StaticArrayConstIterator& b) noexcept {
        return static_cast<difference_type>(a.ptr_ - b.ptr_);
      }

      // random-access element
      constexpr reference operator[](difference_type n) const noexcept { return ptr_[n]; }

      // comparisons
      constexpr bool operator==(const _StaticArrayConstIterator& o) const noexcept {
        return ptr_ == o.ptr_;
      }
      constexpr bool operator!=(const _StaticArrayConstIterator& o) const noexcept {
        return ptr_ != o.ptr_;
      }
      constexpr bool operator<(const _StaticArrayConstIterator& o) const noexcept {
        return ptr_ < o.ptr_;
      }
      constexpr bool operator>(const _StaticArrayConstIterator& o) const noexcept {
        return ptr_ > o.ptr_;
      }
      constexpr bool operator<=(const _StaticArrayConstIterator& o) const noexcept {
        return ptr_ <= o.ptr_;
      }
      constexpr bool operator>=(const _StaticArrayConstIterator& o) const noexcept {
        return ptr_ >= o.ptr_;
      }

      constexpr pointer unwrap() const noexcept { return ptr_; }
    };

    template<typename T>
    class _StaticArrayIterator : public _StaticArrayConstIterator<T> {
      using _Base = _StaticArrayConstIterator<T>;
    public:
      using value_type = T;
      using difference_type = typename _Base::difference_type;
      using pointer = T*;
      using reference = T&;
      using size_type = noyx::size_t;

      constexpr _StaticArrayIterator() noexcept : _Base() {}
      constexpr explicit _StaticArrayIterator(pointer p, size_type pos, size_type size) noexcept : _Base(p, pos, size) {}

      // access (mutable)
      constexpr reference operator*() const noexcept {
        return const_cast<reference>(_Base::operator*());
      }
      constexpr pointer operator->() const noexcept {
        return const_cast<pointer>(_Base::operator->());
      }

      // increment / decrement reuse base
      constexpr _StaticArrayIterator& operator++() noexcept { _Base::operator++(); return *this; }
      constexpr _StaticArrayIterator  operator++(int) noexcept { _StaticArrayIterator tmp = *this; _Base::operator++(); return tmp; }

      constexpr _StaticArrayIterator& operator--() noexcept { _Base::operator--(); return *this; }
      constexpr _StaticArrayIterator  operator--(int) noexcept { _StaticArrayIterator tmp = *this; _Base::operator--(); return tmp; }

      // arithmetic
      constexpr _StaticArrayIterator& operator+=(difference_type n) noexcept { _Base::operator+=(n); return *this; }
      constexpr _StaticArrayIterator& operator-=(difference_type n) noexcept { _Base::operator-=(n); return *this; }

      friend constexpr _StaticArrayIterator operator+(const _StaticArrayIterator& it, difference_type n) noexcept {
        _StaticArrayIterator tmp = it; tmp += n; return tmp;
      }
      friend constexpr _StaticArrayIterator operator+(difference_type n, const _StaticArrayIterator& it) noexcept {
        return it + n;
      }
      friend constexpr _StaticArrayIterator operator-(const _StaticArrayIterator& it, difference_type n) noexcept {
        _StaticArrayIterator tmp = it; tmp -= n; return tmp;
      }

      friend constexpr difference_type operator-(const _StaticArrayIterator& a, const _StaticArrayIterator& b) noexcept {
        return static_cast<difference_type>(a.unwrap() - b.unwrap());
      }

      constexpr reference operator[](difference_type n) const noexcept {
        return const_cast<reference>(_Base::operator[](n));
      }

      // expose unwrapped pointer as mutable
      constexpr pointer unwrap() const noexcept { return const_cast<pointer>(_Base::unwrap()); }
    };

  }
}