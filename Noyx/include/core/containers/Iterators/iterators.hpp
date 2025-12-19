#pragma once
#include <array>
#include <platform/typedef.hpp>

namespace noyx {
  namespace containers {
		
	  template<typename T, bool isConst>
	  class MyIterator
	  {
	  public:
		  using iterator_category = std::random_access_iterator_tag;
		  using iterator_concept = std::contiguous_iterator_tag;
		  using value_type = std::remove_const_t<T>;
		  using difference_type = std::ptrdiff_t;
		  using pointer = std::conditional_t<isConst, const T*, T*>;
		  using reference = std::conditional_t<isConst, const T&, T&>;

	  private:
		  pointer ptr_ = nullptr;

	  public:
		  constexpr MyIterator() noexcept = default;
		  constexpr explicit MyIterator(pointer p) noexcept : ptr_(p) {}

		  template<bool wasConst, typename = std::enable_if_t<IsConst && !wasConst>>
		  constexpr MyIterator(const MyIterator<T, wasConst>& other) noexcept : ptr_(other.base()) {}

		  constexpr pointer base() const noexcept { return ptr_; }

	  public:
		  constexpr reference operator*() const noexcept { return *ptr_; }
		  constexpr pointer operator->() const noexcept { return ptr_; }
		  constexpr reference operator[](difference_type n) const noexcept { return ptr_[n]; }

		  constexpr MyIterator& operator++() noexcept { 
			  ++ptr_; 
			  return *this; 
		  }
		  constexpr MyIterator operator++(int) noexcept {
			  auto temp = *this;
			  ptr_++;
			  return temp;
		  }
		  constexpr MyIterator& operator--() noexcept {
			  --ptr_;
			  return *this;
		  }
		  constexpr MyIterator operator--(int) noexcept {
			  auto temp = *this;
			  ptr_--;
			  return temp;
		  }
		  constexpr MyIterator& operator+=(difference_type n) noexcept {
			  ptr_ += n;
			  return *this;
		  }
		  constexpr MyIterator& operator-=(difference_type n) noexcept {
			  ptr_ -= n;
			  return *this;
		  }

	  public:
		  friend constexpr MyIterator operator+(const MyIterator& it, difference_type n) noexcept {
			  auto temp = it;
			  temp += n; 
			  return temp;
		  }
		  friend constexpr MyIterator operator+(difference_type n, const MyIterator& it) noexcept {
			  return it + n;
		  }
		  friend constexpr MyIterator operator-(const MyIterator& it, difference_type n) noexcept {
			  auto temp = it;
			  it -= n;
			  return temp;
		  }
		  friend constexpr difference_type operator-(const MyIterator& lit, const MyIterator& rit) noexcept {
			  return lit.base() - rit.base();
		  }

		  auto operator<=>(const MyIterator&) const = default;

	  };
  }
}