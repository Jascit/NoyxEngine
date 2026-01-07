/* SPDX-License-Identifier: Apache-2.0 */
/* 
 * \file   memory/allocators/traits.hpp
 * \brief  Traits for custom allocators and compile time helpers
 * 
 * Copyright (c) 2026 Jascit 
 * \author Jascit<https://github.com/Jascit>
 * \date   07.01.2026
 */
#pragma once
#include <memory>

namespace noyxcore::memory::allocators {
  /* 
   * @brief Alias to std::allocator_traits for now.
   * @note Replace with custom traits once allocator API stabilizes.
  */
  template<typename Alloc>
  using allocator_traits = std::allocator_traits<Alloc>;

  namespace detail {

    /**
     * @brief SFINAE helper: checks whether Alloc has a member
     *        function `construct(pointer, Args...)`.
     *
     * @tparam Alloc  Allocator type to inspect.
     * @tparam         SFINAE slot (used with std::void_t).
     * @tparam Args   Argument pack for the candidate construct(...) call.
     *
     * By default inherits from std::false_type. A specialization below
     * inherits from std::true_type when the expression
     * `std::declval<Alloc&>().construct(pointer, Args...)` is well-formed.
     */
    template<typename Alloc, typename = void, typename... Args>
    struct has_construct_helper : std::false_type {};

    /**
     * @brief Specialization selected when Alloc::construct(pointer, Args...) is valid.
     *
     * This specialization uses std::void_t on the decltype of the construct call.
     * The pointer type comes from allocator_traits<Alloc>::pointer.
     */
    template<typename Alloc, typename... Args>
    struct has_construct_helper<
      Alloc,
      std::void_t<
      decltype(
        std::declval<Alloc&>().construct(
          std::declval<typename allocator_traits<Alloc>::pointer>(),
          std::declval<Args>()...
        )
        )
      >,
      Args...
    > : std::true_type{};

  } // namespace detail

  /**
   * @brief Trait: whether Alloc provides construct(pointer, Args...).
   *
   * @tparam Alloc  Allocator type to inspect.
   * @tparam Args   Argument pack for the candidate construct(...) call.
   */
  template<typename Alloc, typename... Args>
  struct has_construct : detail::has_construct_helper<Alloc, void, Args...> {};

  /**
   * @brief Bool alias for has_construct.
   *
   * Usage: static_assert(has_construct_v<MyAlloc, T>);
   */
  template<typename Alloc, typename... Args>
  inline constexpr bool has_construct_v = has_construct<Alloc, Args...>::value;

  /**
   * @brief Trait: whether Alloc provides destroy(pointer).
   *
   * Defaults to false; a specialization using std::void_t selects true
   * when `Alloc::destroy(pointer)` is a well-formed expression.
   *
   * @tparam Alloc  Allocator type to inspect.
   */
  template<typename Alloc, typename = void>
  struct has_destroy : std::false_type {};

  template<typename Alloc>
  struct has_destroy<
    Alloc,
    std::void_t<
    decltype(
      std::declval<Alloc&>().destroy(
        std::declval<typename allocator_traits<Alloc>::pointer>()
      )
      )
    >
  > : std::true_type {
  };

  /**
   * @brief Bool alias for has_destroy.
   *
   * Usage: if constexpr (has_destroy_v<MyAlloc>) { ... }
   */
  template<typename Alloc>
  inline constexpr bool has_destroy_v = has_destroy<Alloc>::value;
} // noyxcore::memory::allocators
