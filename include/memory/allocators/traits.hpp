/* SPDX-License-Identifier: Apache-2.0 */

/**
 * @file     memory/allocators/traits.hpp
 * @brief    Traits for custom allocators and compile time helpers
 *
 * @author   Jascit (https://github.com/Jascit)
 * @date     07.01.2026
 *
 * @copyright Copyright (c) 2026 The Project Contributors
 */

#pragma once
#include <memory>

namespace noyxcore::memory::allocators {
  template<typename T>
  struct no_alloc {
    using value_type = T;
    using pointer = std::add_pointer_t<T>;
    using reference = std::add_lvalue_reference_t<T>;
  };

  /* 
   * @brief Alias to std::allocator_traits for now.
   * @note Replace with custom traits once allocator API stabilizes.
  */
  template<typename Alloc>
  using allocator_traits = std::allocator_traits<Alloc>;

  namespace details {

    /**
     * @brief SFINAE helper: checks whether Alloc has a member
     *        function `construct(pointer, Args...)`.
     *
     * @tparam Alloc  Allocator type to inspect.
     * @tparam Pointer  Pointer type to construct.
     * @tparam Args   Argument pack for the candidate construct(...) call.
     *
     * By default inherits from std::false_type. A specialization below
     * inherits from std::true_type when the expression
     * `std::declval<Alloc&>().construct(pointer, Args...)` is well-formed.
     */

    template<typename Allocator, typename Pointer, typename... Args>
    struct has_construct_helper {
    private:
      template<typename A>
      static auto test(int) -> decltype(
        std::declval<A &>().construct(
          std::declval<Pointer>(),
          std::declval<Args>()...
        ),
        std::true_type{}
      );

      template<typename>
      static std::false_type test(...);

    public:
      using type = decltype(test<Allocator>(0));
    };

  } // namespace details

  /**
   * @brief Trait: whether Alloc provides construct(pointer, Args...).
   *
   * @tparam Alloc  Allocator type to inspect.
   * @tparam Pointer Pointer type to construct
   * @tparam Args   Argument pack for the candidate construct(...) call.
   */
  template<typename Allocator, typename Pointer, typename... Args>
  struct has_construct : details::has_construct_helper<Allocator, Pointer, Args...>::type {
  };

  /**
   * @brief Bool alias for has_construct.
   *
   * Usage: static_assert(has_construct_v<MyAlloc, T>);
   */
  template<typename Allocator, typename pointer, typename... Args>
  constexpr bool has_construct_v = has_construct<Allocator, pointer, Args...>::value;

  /**
   * @brief Trait: whether Alloc provides destroy(pointer).
   *
   * Defaults to false; a specialization using std::void_t selects true
   * when `Alloc::destroy(pointer)` is a well-formed expression.
   *
   * @tparam Alloc  Allocator type to inspect.
   * @tparam Pointer Pointer type to destroy.
   */
  template<typename Allocator, typename Pointer, typename = void>
  struct has_destroy : std::false_type {};

  template<typename Allocator, typename Pointer>
  struct has_destroy<Allocator, Pointer, std::void_t<decltype(std::declval<Allocator &>().destroy(std::declval<Pointer>()))>> : std::true_type {};

  /**
   * @brief Bool alias for has_destroy.
   *
   * Usage: if constexpr (has_destroy_v<MyAlloc>) { ... }
   */
  template<typename Alloc, typename Pointer>
  inline constexpr bool has_destroy_v = has_destroy<Alloc, Pointer>::value;
} // noyxcore::memory::allocators
