#pragma once
#include <type_traits>
#include <xmemory>

namespace noyx {
  namespace utility {
    template<typename T>
    constexpr void swap(T& first, T& second)
      noexcept(std::is_nothrow_move_constructible_v<T>&& std::is_nothrow_move_assignable_v<T>) {
      T temp_ = std::move(first);
      first = std::move(second);
      second = std::move(temp_);
    }

    struct _FirstOneSecondArgs {
      _FirstOneSecondArgs() = default;
    };
    struct _FirstZeroSecondArgs {
      _FirstZeroSecondArgs() = default;
    };

    template<typename T1, typename T2, bool = std::is_empty_v<T1> && !std::is_final_v<T1>>//fuck T2
    class TCompressedPair final {
      T1 first_;
      T2 second_;

    public:
      // need to have overload move operator=
      constexpr void swap(TCompressedPair<T1, T2, false>&& other) noexcept(std::is_nothrow_copy_assignable_v<T1>&& std::is_nothrow_copy_assignable_v<T2>) {
        T2 temp1_ = std::move(first_);
        T2 temp2_ = std::move(second_);
        second_ = std::move(other.second());
        first_ = std::move(other.first());
        other.second_ = std::move(temp1_);
        other.second_ = std::move(temp2_);
      }
      // need to have overload copy operator=
      constexpr void copy(const TCompressedPair<T1, T2, false>& other) noexcept(std::is_nothrow_copy_assignable_v<T1>&& std::is_nothrow_copy_assignable_v<T2>) {
        second_ = other.second();
        first_ = other.first();
      }
      TCompressedPair() = default;
      template<typename... Args>
      constexpr TCompressedPair(_FirstZeroSecondArgs, Args&&... args) noexcept(
        std::conjunction_v<
        std::is_nothrow_constructible<T1>,
        std::is_nothrow_constructible<T2, Args&&...>
        >)
        : first_(), second_(std::forward<Args>(args)...) {
      }

      template<typename U1, typename... Args>
      constexpr TCompressedPair(_FirstOneSecondArgs, U1&& f, Args&&... args) noexcept(
        std::conjunction_v<
        std::is_nothrow_constructible<T1, U1&&>,
        std::is_nothrow_constructible<T2, Args&&...>
        >)
        : first_(std::forward<U1>(f)), second_(std::forward<Args>(args)...) {
      }

      template<typename U1, typename U2>
      constexpr TCompressedPair(U1&& f, U2&& s) noexcept(
        std::conjunction_v<
        std::is_nothrow_constructible<T1, U1&&>,
        std::is_nothrow_constructible<T2, U2&&>
        >)
        : first_(std::forward<U1>(f)), second_(std::forward<U2>(s)) {
      }

      constexpr T1& first() noexcept {
        return first_;
      }

      constexpr const T1& first() const noexcept {
        return first_;
      }

      constexpr T2& second() noexcept {
        return second_;
      }

      constexpr const T2& second() const noexcept {
        return second_;
      }
    };

    template<typename T1, typename T2>
    class TCompressedPair<T1, T2, true> final : private T1 {
      T2 second_;

    public:
      // need to have overload move operator=
      constexpr void swap(TCompressedPair<T1, T2>&& other) noexcept(
        std::is_nothrow_move_assignable_v<T1>&& std::is_nothrow_move_assignable_v<T2>)
      {
        T2 tmp2 = std::move(second_);
        second_ = std::move(other.second_);
        other.second_ = std::move(tmp2);
      }

      // need to have overload copy operator=
      constexpr void copy(const TCompressedPair<T1, T2>& other) noexcept(std::is_nothrow_copy_assignable_v<T1>&& std::is_nothrow_copy_assignable_v<T2>) {
        second_ = other.second();
      }
      TCompressedPair() : T1() {};
      template<typename... Args>
      constexpr TCompressedPair(_FirstZeroSecondArgs, Args&&... args) noexcept(
        std::conjunction_v<
        std::is_nothrow_constructible<T1>,
        std::is_nothrow_constructible<T2, Args&&...>
        >)
        : T1(), second_(std::forward<Args>(args)...) {
      }

      template<typename U1, typename... Args>
      constexpr TCompressedPair(_FirstOneSecondArgs, U1&& f, Args&&... args) noexcept(
        std::conjunction_v<
        std::is_nothrow_constructible<T1, U1&&>,
        std::is_nothrow_constructible<T2, Args&&...>
        >)
        : T1(std::forward<U1>(f)), second_(std::forward<Args>(args)...) {
      }

      template<typename U1, typename U2>
      constexpr TCompressedPair(U1&& f, U2&& s) noexcept(
        std::conjunction_v<
        std::is_nothrow_constructible<T1, U1&&>,
        std::is_nothrow_constructible<T2, U2&&>
        >)
        : T1(std::forward<U1>(f)), second_(std::forward<U2>(s)) {
      }

      inline constexpr T1& first() noexcept {
        return static_cast<T1&>(*this);
      }

      inline constexpr const T1& first() const noexcept {
        return static_cast<T1&>(*this);
      }

      inline constexpr T2& second() noexcept {
        return second_;
      }

      inline constexpr const T2& second() const noexcept {
        return second_;
      }
    };

    template<typename Alloc, typename InputPtr, typename DestPtr, typename SizeType>
    DestPtr _initialized_copy_n(InputPtr src, SizeType count, DestPtr dest, Alloc& alloc) {
      using traits = std::allocator_traits<Alloc>;
      using value_type = typename traits::value_type;
      using raw_ptr = typename Alloc::value_type*;

      if constexpr (std::is_trivially_copyable_v<value_type>) {
        memcpy(static_cast<void*>(dest), static_cast<const void*>(src), static_cast<size_t>(count) * sizeof(value_type));
        return dest + count;
      }
      else {
        // Загальний шлях: поелементно construct + rollback при виключенні
        size_t constructed = 0;
        try {
          for (SizeType i = 0; i < count; ++i) {
            traits::construct(alloc, dest + i, src[i]); // placement new через аллокатор
            ++constructed;
          }
          return dest + count;
        }
        catch (...) {
          // знищити те, що вже сконструйовано
          for (size_t j = 0; j < constructed; ++j) {
            traits::destroy(alloc, dest + (constructed - 1 - j));
          }
          throw;
        }
      }
    }
  }
}