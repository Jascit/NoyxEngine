#pragma once
#include "addressof.hpp"
#include "is_convertible.hpp"
#include "remove_cvref_t.hpp"
#include "is_object.hpp"
#include "is_function.hpp"
std::reference_wrapper<int>;

namespace xstl {
  template<typename T>
  class reference_wrapper {
    static_assert(xstl::is_function_v<T> || xstl::is_object_v<T>, "T must be a function or an object type");
  public:
    using type = T;

    template<typename U>
    requires xstl::is_convertible_v<U&, T&> && !xstl::is_same_v<remove_cvref_t<U>, reference_wrapper>
    reference_wrapper(U& ref) noexcept : ptr(xstl::addressof(ref)) {};

    template<typename U>
    requires !(is_convertible_v<U&, T&>&& !is_same_v<remove_cvref_t<U>, reference_wrapper>)
    reference_wrapper(U&) {
      static_assert(false, "Cannot bind reference_wrapper to this type");
    };

    reference_wrapper(const reference_wrapper&) noexcept = default;
    reference_wrapper& operator=(const reference_wrapper&) noexcept = default;

    constexpr operator T& () const noexcept { return *ptr; }
    
    constexpr [[nodiscard]] T& Get() const noexcept { return *ptr; }

    constexpr auto operator()() const noexcept() -> (void/*change in future*/) {}
  private:
    T* ptr{};
  };
} // xstl