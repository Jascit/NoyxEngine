#pragma once
#include "is_member_function_pointer.hpp"
#include "is_member_object_pointer.hpp"
#include "forward.hpp"
#include "remove_cvref_t.hpp"
#include "is_specialization.hpp"

namespace xstl {
  template<typename T>
  class reference_wrapper;

  namespace details {
    template<typename Callable, typename Tc, typename... Args>
    struct invoke_impl;

    // якщо це простий показник на мембер обєкт з класа то пробуємо найти обєкт класа в першому аргументі, якщо нема його то просто возращаємо
    template<typename Callable, typename Tc, typename... Args>
    requires (!is_member_function_pointer_v<Callable> && is_member_object_pointer_v<Callable>)
    struct invoke_impl<Callable, Tc, Args...> {};

    // якщо callable є показником на функію в класі то пробуємо найти обєкт класа в першому аргументі, якщо нема його, просто визиваємо функцію
    template<typename Callable, typename Tc, typename... Args>
    requires is_member_function_pointer_v<Callable>
    struct invoke_impl<Callable, Tc, Args...> : true_type{};
    
    // якщо нема ніяких звязків до класа і це простий callable, то визиваємо її з простою передачею аргів через форвард
    template<typename Callable, typename Tc, typename... Args>
    requires (!is_member_function_pointer_v<Callable> && !is_member_object_pointer_v<Callable>)
    struct invoke_impl<Callable, Tc, Args...> {};
  
  } // details

  template<typename Callable>
  constexpr decltype(auto) invoke(Callable&& obj) noexcept(noexcept(xstl::forward<Callable>(obj)()))
  {
    return std::forward<Callable>(obj)();
  }

  template<typename Callable, typename Tc, typename... Args>
  constexpr decltype(auto) invoke(Callable&& obj, Tc&& relation, Args&&... types) noexcept(noexcept(xstl::forward<Callable>(obj)()))
  {
    return std::forward<Callable>(obj)();
  }
} // xstl