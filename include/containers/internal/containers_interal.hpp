/* SPDX-License-Identifier: Apache-2.0 */
/* 
 * \file   containers_interal.hpp
 * \brief  internal helpers for containers
 * 
 * Copyright (c) 2026 Jascit 
 * \author Jascit<https://github.com/Jascit>
 * \date   07.01.2026
 */

#pragma once
#include "containers_traits.hpp"
#include <platform/debug.hpp>
#include <type_traits>
#include <memory>

namespace noyxcore::containers::internal {
  /**
   * @brief RAII helper for constructing objects in uninitialized memory.
   *
   * Manages construction and destruction of elements to ensure exception safety.
   * @tparam Alloc Allocator type.
   */
  template<typename Alloc>
  class ConstructionHelper {
  public:
    using allocator_traits = memory::allocators::allocator_traits<Alloc>;
    using value_type = typename allocator_traits::value_type;
    using pointer = typename allocator_traits::pointer;

    ConstructionHelper(Alloc& alloc, pointer dest) noexcept : alloc_(alloc), current_(dest), first_(dest) {};

    ~ConstructionHelper() {
      if constexpr (!std::is_trivially_destructible_v<value_type>) {
        for (pointer it = first_; it < current_; ++it) {
          if constexpr (memory::allocators::has_destroy_v<Alloc>) {
            alloc_.get().destroy(it);
          }
          else {
            std::destroy_at(it);
          }
        }
      }
    };

    template<typename... Args>
    CONSTEXPR void constructOne(Args&&... args) noexcept(std::is_nothrow_constructible_v<value_type>) {
      if constexpr (memory::allocators::has_construct_v<Alloc>) {
        alloc_.get().construct(current_, std::forward<Args>(args)...);
      }
      else {
        std::construct_at(current_, std::forward<Args>(args)...);
      }
      ++current_;
    };

    CONSTEXPR void release() noexcept { first_ = current_; };

    CONSTEXPR pointer current() const noexcept { return current_; };

  private:
    std::reference_wrapper<Alloc> alloc_; 
    pointer first_;  
    pointer current_; 
  };
  
  template<typename Alloc, typename FwdIt>
  NODISCARD CONSTEXPR alloc_ptr_t<Alloc> uninitialized_fill_n(FwdIt first, size_t count, alloc_val_t<Alloc>& val, Alloc& alloc) noexcept(...);

  template<typename Alloc, typename FwdIt>
  NODISCARD CONSTEXPR alloc_ptr_t<Alloc> uninitialized_copy_n(FwdIt first, size_t count, alloc_ptr_t<Alloc> dest, Alloc& alloc) noexcept(...);

  template<typename Alloc, typename FwdIt>
  NODISCARD CONSTEXPR alloc_ptr_t<Alloc> uninitialized_move_n(FwdIt first, size_t count, alloc_ptr_t<Alloc> dest, Alloc& alloc) noexcept(...);

  template<typename Alloc, typename FwdIt>
  NODISCARD CONSTEXPR alloc_ptr_t<Alloc> uninitialized_copy(FwdIt first, FwdIt last, alloc_ptr_t<Alloc> dest, Alloc& alloc) noexcept(...);

  template<typename Alloc, typename FwdIt>
  NODISCARD CONSTEXPR alloc_ptr_t<Alloc> uninitialized_move(FwdIt first, FwdIt last, alloc_ptr_t<Alloc> dest, Alloc& alloc) noexcept(...);

  template<typename Alloc, typename FwdIt>
  NODISCARD CONSTEXPR alloc_ptr_t<Alloc> uninitialized_fill(FwdIt first, FwdIt last, alloc_ptr_t<Alloc> dest, Alloc& alloc) noexcept(...);

  template<typename Alloc, typename FwdIt>
  NODISCARD CONSTEXPR alloc_ptr_t<Alloc> uninitialized_default_construct(FwdIt first, FwdIt last, Alloc& alloc) noexcept(...);

  template<typename FwdIt>
  NODISCARD CONSTEXPR iter_ptr_t<FwdIt> assign_move_n(FwdIt first, size_t count, iter_ptr_t<FwdIt> dest) noexcept(...);

  template<typename FwdIt>
  NODISCARD CONSTEXPR iter_ptr_t<FwdIt> assign_copy_n(FwdIt first, size_t count, iter_ptr_t<FwdIt> dest) noexcept(...);

  template<typename FwdIt>
  NODISCARD CONSTEXPR iter_ptr_t<FwdIt> assign_move(FwdIt first, FwdIt last, iter_ptr_t<FwdIt> dest) noexcept(...);

  template<typename FwdIt>
  NODISCARD CONSTEXPR iter_ptr_t<FwdIt> assign_copy(FwdIt first, FwdIt last, iter_ptr_t<FwdIt> dest) noexcept(...);

  template<typename Alloc, typename FwdIt>
  constexpr bool is_nothrow_uninitialized_moveable_v = noexcept(internal::uninitialized_move(
    std::declval<FwdIt>(), std::declval<FwdIt>(),
    std::declval<alloc_ptr_t<Alloc>>(), std::declval<Alloc&>()
  ));


  template<typename Alloc, typename FwdIt>
  constexpr bool is_nothrow_uninitialized_copyable_v = noexcept(internal::uninitialized_copy(
    std::declval<FwdIt>(), std::declval<FwdIt>(),
    std::declval<alloc_ptr_t<Alloc>>(), std::declval<Alloc&>()
  ));


  template<typename Alloc, typename FwdIt>
  constexpr bool is_nothrow_uninitialized_fillable_v = noexcept(internal::uninitialized_fill(
    std::declval<FwdIt>(), std::declval<FwdIt>(),
    std::declval<detail::alloc_val_t<Alloc>&>(), std::declval<Alloc&>()
  ));


  template<typename Alloc, typename FwdIt>
  constexpr bool is_nothrow_uninitialized_default_constructible_v = noexcept(internal::uninitialized_default_construct(
    std::declval<FwdIt>(), std::declval<FwdIt>(), std::declval<Alloc&>()
  ));

  template<typename FwdIt>
  constexpr bool is_nothrow_copy_n_assignable_v = noexcept(internal::assign_copy_n(
    std::declval<FwdIt>(), std::declval<size_t>(),
    std::declval<iter_ptr_t<FwdIt>>()
  ));

  template<typename FwdIt>
  constexpr bool is_nothrow_move_n_assignable_v = noexcept(internal::assign_move_n(
    std::declval<FwdIt>(), std::declval<size_t>(),
    std::declval<iter_ptr_t<FwdIt>>()
  ));
} // noyxcore::containers::internal

//#include <containers/internal/inline/containers_internal-inl.hpp>
