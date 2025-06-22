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
    template <typename T>
    constexpr T& _RW_test_bind(T&) noexcept;
    template <typename T>
    void _RW_test_bind(T&&) = delete;

    template <typename T, typename U, typename Wrapper>
    concept _ConceptBindableToRefWrapper = 
      xstl::is_same_v<remove_cvref_t<T>, Wrapper> && requires { _RW_test_bind<T>(xstl::declval<U>()); };

  } // namespace detail

  template <typename T>
  class reference_wrapper {
    static_assert(is_object_v<T> || is_function_v<T>, "reference_wrapper<T>: T must be an object or function type");

  public:
    using type = T;

    template <typename U>
    requires detail::_ConceptBindableToRefWrapper<T, U, reference_wrapper>
    constexpr reference_wrapper(U&& ref) noexcept : ptr(xstl::addressof(ref)) {}

    template <typename U>
    requires (!detail::_ConceptBindableToRefWrapper<T, U, reference_wrapper>)
    constexpr reference_wrapper(U&&) {
      static_assert(false, "reference_wrapper: cannot bind to this type");
    }

    reference_wrapper(const reference_wrapper&) noexcept = default;
    reference_wrapper& operator=(const reference_wrapper&) noexcept = default;

    constexpr operator T& () const noexcept {
      return *ptr;
    }
    constexpr T& get() const noexcept {
      return *ptr;
    }

    //temporary TODO: change on invoker
    template <typename... Args>
    constexpr auto operator()(Args&&... args) const
      noexcept(noexcept((*ptr)(xstl::forward<Args>(args)...)))
      -> decltype((*ptr)(xstl::forward<Args>(args)...))
    {
      return (*ptr)(xstl::forward<Args>(args)...);
    }

  private:
    T* ptr = xstl::nullptr_t;
  };

} // namespace xstl
