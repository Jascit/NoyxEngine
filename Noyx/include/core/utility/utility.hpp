#pragma once
#include <type_traits>
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
      FirstOneSecondArgs() = default;
    };
    struct _FirstZeroSecondArgs {
      FirstZeroSecondArgs() = default;
    };

    template<typename T1, typename T2, bool = std::is_empty_v<T1> && !std::is_final_v<T1>>//fuck T2
    class TCompressedPair final {
      T1 first_;
      T2 second_;

    public:
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

      constexpr T1& GetFirst() noexcept {
        return first_;
      }

      constexpr const T1& GetFirst() const noexcept {
        return first_;
      }

      constexpr T2& GetSecond() noexcept {
        return second_;
      }

      constexpr const T2& GetSecond() const noexcept {
        return second_;
      }
    };

    template<typename T1, typename T2>
    class TCompressedPair<T1, T2, true> final : private T1 {
      T2 second_;

    public:
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

      constexpr T1& GetFirst() noexcept {
        return *this;
      }

      constexpr const T1& GetFirst() const noexcept {
        return *this;
      }

      constexpr T2& GetSecond() noexcept {
        return second_;
      }

      constexpr const T2& GetSecond() const noexcept {
        return second_;
      }
    };
  }
}