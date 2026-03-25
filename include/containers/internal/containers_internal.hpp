/* SPDX-License-Identifier: Apache-2.0 */
/* 
 * \file   containers_interal.hpp
 * \brief  Internal helpers for containers.
 * 
 * Copyright (c) 2026 Project Contributors
 * \author Jasc<https://github.com/Jascit>
 * \date   07.01.2026
 */

#pragma once
#include "containers_traits.hpp"
#include <platform/debug.h>
#include <type_traits>
#include <memory>
#include <iterator>

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
    using raw_pointer = decltype(std::to_address(std::declval<pointer>()));
    static_assert(std::is_same_v<pointer, raw_pointer>, "ConstructionHelper doesn't support fancy pointers");
    constexpr ConstructionHelper(Alloc& alloc, pointer dest) noexcept : alloc_(alloc), current_(dest), first_(dest) {};

    constexpr ~ConstructionHelper() {
      if constexpr (!std::is_trivially_destructible_v<value_type>) {
        for (raw_pointer it = first_; it < current_; ++it) {
          if constexpr (memory::allocators::has_destroy_v<Alloc, pointer>) {
            alloc_.get().destroy(it);
          } else {
            std::destroy_at(it);
          }
        }
      }
    };

    template<typename... Args>
    constexpr void constructOne(Args&&... args) noexcept(std::is_nothrow_constructible_v<value_type, Args...>) {
      if constexpr (memory::allocators::has_construct_v<Alloc, pointer>) {
        alloc_.get().construct(current_, std::forward<Args>(args)...);
      } else {
        std::construct_at(current_, std::forward<Args>(args)...);
      }
      ++current_;
    };

    constexpr void release() noexcept { first_ = current_; };

    constexpr pointer current() const noexcept { return current_; };

  private:
    std::reference_wrapper<Alloc> alloc_;
    pointer first_;
    pointer current_;
  };

  template<typename T>
    class CleanupGuard {
  public:
    CleanupGuard(T* obj) : obj_(obj) {};

    ~CleanupGuard() {
      if (obj_) {
        obj_->cleanup_();
      }
    };

    void release() {
      obj_ = nullptr;
    };

  private:
    T* obj_;
  };

  template<typename T, typename Alloc>
  struct AllocationGuard {
    using traits = std::allocator_traits<Alloc>;
    using pointer = typename traits::pointer;
    using size_type = typename traits::size_type;

    pointer ptr_;
    size_type n_;
    Alloc& alloc_;

    constexpr AllocationGuard(pointer p, size_type n, Alloc& a)
      : ptr_(p), n_(n), alloc_(a) {}

    ~AllocationGuard() {
      if (ptr_) {
        traits::deallocate(alloc_, ptr_, n_);
      }
    }
    void release() { ptr_ = nullptr; }
  };


  template<typename Alloc, typename FwdIt>
  constexpr alloc_raw_ptr_t<Alloc> uninitialized_fill_n(FwdIt first, size_t count, const alloc_val_t<Alloc>& val,
                                                        Alloc& alloc)
    noexcept(std::is_nothrow_copy_constructible_v<alloc_val_t<Alloc>>);

  template<typename Alloc, typename FwdIt>
  constexpr alloc_raw_ptr_t<Alloc> uninitialized_copy_n(FwdIt first, size_t count, alloc_raw_ptr_t<Alloc> dest,
                                                        Alloc& alloc)
    noexcept(std::is_nothrow_copy_constructible_v<alloc_val_t<Alloc>>);

  template<typename Alloc, typename FwdIt>
  constexpr alloc_raw_ptr_t<Alloc> uninitialized_move_n(FwdIt first, size_t count, alloc_raw_ptr_t<Alloc> dest,
                                                        Alloc& alloc)
    noexcept(std::is_nothrow_move_constructible_v<alloc_val_t<Alloc>>);

  template<typename Alloc, typename FwdIt>
  constexpr alloc_raw_ptr_t<Alloc> uninitialized_copy(FwdIt first, FwdIt last, alloc_raw_ptr_t<Alloc> dest,
                                                      Alloc& alloc)
    noexcept(std::is_nothrow_copy_constructible_v<alloc_val_t<Alloc>>);

  template<typename Alloc, typename FwdIt>
  constexpr alloc_raw_ptr_t<Alloc> uninitialized_move(FwdIt first, FwdIt last, alloc_raw_ptr_t<Alloc> dest,
                                                      Alloc& alloc)
    noexcept(std::is_nothrow_move_constructible_v<alloc_val_t<Alloc>>);

  template<typename Alloc, typename FwdIt>
  constexpr alloc_raw_ptr_t<Alloc> uninitialized_fill(FwdIt first, FwdIt last, const alloc_val_t<Alloc>& val
                                                      , Alloc& alloc)
    noexcept(std::is_nothrow_copy_constructible_v<alloc_val_t<Alloc>>);

  template<typename Alloc, typename FwdIt>
  constexpr alloc_raw_ptr_t<Alloc> uninitialized_default_construct(FwdIt first, FwdIt last, Alloc& alloc)
    noexcept(std::is_nothrow_default_constructible_v<alloc_val_t<Alloc>>);

  template<typename FwdIt>
  constexpr iter_ptr_t<FwdIt> assign_move_n(FwdIt first, size_t count, FwdIt dest)
    noexcept(std::is_nothrow_move_assignable_v<iter_val_t<FwdIt>>);

  template<typename FwdIt>
  constexpr iter_ptr_t<FwdIt> assign_copy_n(FwdIt first, size_t count, FwdIt dest)
    noexcept(std::is_nothrow_copy_assignable_v<iter_val_t<FwdIt>>);

  template<typename FwdIt>
  constexpr iter_ptr_t<FwdIt> assign_move(FwdIt first, FwdIt last, FwdIt dest)
    noexcept(std::is_nothrow_move_assignable_v<iter_val_t<FwdIt>>);

  template<typename FwdIt>
  constexpr iter_ptr_t<FwdIt> assign_copy(FwdIt first, FwdIt last, FwdIt dest)
    noexcept(std::is_nothrow_copy_assignable_v<iter_val_t<FwdIt>>);

  template<typename Alloc, typename FwdIt>
  constexpr bool is_nothrow_uninitialized_moveable_v = noexcept(internal::uninitialized_move(
    std::declval<FwdIt>(), std::declval<FwdIt>(),
    std::declval<alloc_raw_ptr_t<Alloc>>(), std::declval<Alloc&>()
  ));


  template<typename Alloc, typename FwdIt>
  constexpr bool is_nothrow_uninitialized_copyable_v = noexcept(uninitialized_copy(
    std::declval<FwdIt>(), std::declval<FwdIt>(),
    std::declval<alloc_raw_ptr_t<Alloc>>(), std::declval<Alloc&>()
  ));


  template<typename Alloc, typename FwdIt>
  constexpr bool is_nothrow_uninitialized_fillable_v = noexcept(uninitialized_fill(
    std::declval<FwdIt>(), std::declval<FwdIt>(),
    std::declval<alloc_val_t<Alloc>&>(), std::declval<Alloc&>()
  ));


  template<typename Alloc, typename FwdIt>
  constexpr bool is_nothrow_uninitialized_default_constructible_v = noexcept(uninitialized_default_construct(
    std::declval<FwdIt>(), std::declval<FwdIt>(), std::declval<Alloc&>()
  ));

  template<typename FwdIt>
  constexpr bool is_nothrow_copy_n_assignable_v = noexcept(assign_copy_n(
    std::declval<FwdIt>(), std::declval<size_t>(),
    std::declval<iter_ptr_t<FwdIt>>()
  ));

  template<typename FwdIt>
  constexpr bool is_nothrow_move_n_assignable_v = noexcept(assign_move_n(
    std::declval<FwdIt>(), std::declval<size_t>(),
    std::declval<iter_ptr_t<FwdIt>>()
  ));
} // noyxcore::containers::internal

#include "containers_internal-inl.hpp"
