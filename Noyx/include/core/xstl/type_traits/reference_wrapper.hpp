#pragma once
#include "addressof.hpp"      
#include "is_convertible.hpp" 
#include "remove_cvref_t.hpp" 
#include "../core/xstldef.hpp" 
#include "is_object.hpp"      
#include "is_function.hpp"    
#include "forward.hpp"        

namespace xstl {
  namespace detail {

    /// @brief Called when U can bind to T&
    template <typename T>
    constexpr T& _RW_test_bind(T&) noexcept;

    /// @brief Deleted overload: rvalue cannot bind to T&
    template <typename T>
    T& _RW_test_bind(T&&) = delete;

    /// Concept that checks if U can bind to T& without conflict with reference_wrapper
    template <typename T, typename U, typename Wrapper>
    concept _ConceptBindableToRefWrapper =
      !xstl::is_same_v<remove_cvref_t<T>, Wrapper> &&
      requires { detail::_RW_test_bind<T>(xstl::declval<U>()); };
  } // namespace detail

  
  /// @brief A wrapper around a reference, similar to std::reference_wrapper
  /// @tparam T The type to be wrapped, must be an object or function type
  template <typename T>
  class reference_wrapper {
    static_assert(is_object_v<T> || is_function_v<T>,
      "reference_wrapper<T>: T must be an object or function type");
  
  public:
    /// @brief The type being wrapped
    using type = T;

    /// @brief Constructor that wraps a reference
    /// @param ref An lvalue argument to store as a reference
    /// @tparam U A type that can be bound to T&
    template <typename U>
      requires detail::_ConceptBindableToRefWrapper<T, U, reference_wrapper>
    constexpr reference_wrapper(U&& ref) noexcept
      : ptr(xstl::addressof(ref)) {
    }

    /// @brief Deleted constructor for incompatible types
    template <typename U>
      requires (!detail::_ConceptBindableToRefWrapper<T, U, reference_wrapper>)
    constexpr reference_wrapper(U&&) {
      static_assert(false, "reference_wrapper: cannot bind to this type");
    }

    /// @brief Copy constructor
    reference_wrapper(const reference_wrapper&) noexcept = default;

    /// @brief Copy assignment operator
    reference_wrapper& operator=(const reference_wrapper&) noexcept = default;

    /// @brief Implicit conversion to T&
    constexpr operator T& () const noexcept {
      return *ptr;
    }

    /// @brief Explicit getter for the underlying reference
    /// @return A reference to the wrapped object of type T
    constexpr T& get() const noexcept {
      return *ptr;
    }

    /// @brief Invokes the stored callable if T is callable
    /// @tparam Args Argument types
    /// @param args Arguments for the call
    /// @return The result of invoking *ptr(args...)
    /// TODO: change to invoke
    template <typename... Args>
    constexpr auto operator()(Args&&... args) const noexcept(false){}

  private:
    /// @brief Pointer to the wrapped object
    T* ptr{};
  };

} // namespace xstl