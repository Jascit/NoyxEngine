/* SPDX-License-Identifier: Apache-2.0 */

/**
 * @file   containers_internal.hpp
 * @brief  Internal helpers for containers.
 *
 * @date   07.01.2026
 *
 * @copyright Copyright (c) 2026 The Project Contributors
 */

#pragma once
#include "containers_traits.hpp"
#include <iterator>
#include <memory>
#include <platform/debug.h>
#include <type_traits>

namespace noyxcore::containers::internal
{
/**
 * @brief RAII helper for constructing objects in uninitialized memory.
 *
 * Manages construction and destruction of elements to ensure exception safety.
 * @tparam Alloc Allocator type.
 */
template <typename Alloc>
class ConstructionHelper
{
public:
    using allocator_traits = memory::allocators::allocator_traits<Alloc>;
    using value_type       = typename allocator_traits::value_type;
    using pointer          = typename allocator_traits::pointer;
    using raw_pointer      = decltype(std::to_address(std::declval<pointer>()));
    static_assert(std::is_same_v<pointer, raw_pointer>,
                  "ConstructionHelper doesn't support fancy pointers");
    constexpr ConstructionHelper(Alloc &alloc, pointer dest) noexcept
        : m_alloc(alloc),
          m_current(dest),
          m_first(dest) {};

    constexpr ~ConstructionHelper()
    {
        if constexpr (!std::is_trivially_destructible_v<value_type>)
        {
            for (raw_pointer it = m_first; it < m_current; ++it)
            {
                if constexpr (memory::allocators::has_destroy_v<Alloc, pointer>)
                {
                    m_alloc.get().destroy(it);
                }
                else
                {
                    std::destroy_at(it);
                }
            }
        }
    };

    template <typename... Args>
    constexpr void
    construct_one(Args &&...args) noexcept(std::is_nothrow_constructible_v<value_type, Args...>)
    {
        if constexpr (memory::allocators::has_construct_v<Alloc, pointer>)
        {
            m_alloc.get().construct(m_current, std::forward<Args>(args)...);
        }
        else
        {
            std::construct_at(m_current, std::forward<Args>(args)...);
        }
        ++m_current;
    };

    constexpr void release() noexcept { m_first = m_current; };

    constexpr pointer current() const noexcept { return m_current; };

private:
    std::reference_wrapper<Alloc> m_alloc;
    pointer m_first;
    pointer m_current;
};

template <typename T>
class CleanupGuard
{
public:
    CleanupGuard(T *obj)
        : m_obj(obj) {};

    ~CleanupGuard()
    {
        if (m_obj)
        {
            m_obj->cleanup_();
        }
    };

    void release() { m_obj = nullptr; };

private:
    T *m_obj;
};

template <typename T, typename Alloc>
struct AllocationGuard
{
    using traits    = std::allocator_traits<Alloc>;
    using pointer   = typename traits::pointer;
    using size_type = typename traits::size_type;

    pointer m_ptr;
    size_type m_n;
    Alloc &m_alloc;

    constexpr AllocationGuard(pointer p, size_type n, Alloc &a)
        : m_ptr(p),
          m_n(n),
          m_alloc(a)
    {
    }

    ~AllocationGuard()
    {
        if (m_ptr)
        {
            traits::deallocate(m_alloc, m_ptr, m_n);
        }
    }

    void release() { m_ptr = nullptr; }
};

template <typename Alloc, typename FwdIt>
constexpr alloc_raw_ptr_t<Alloc> uninitialized_fill_n(
    FwdIt first, size_t count, const alloc_val_t<Alloc> &val,
    Alloc &alloc) noexcept(std::is_nothrow_copy_constructible_v<alloc_val_t<Alloc>>);

template <typename Alloc, typename FwdIt>
constexpr alloc_raw_ptr_t<Alloc> uninitialized_copy_n(
    FwdIt first, size_t count, alloc_raw_ptr_t<Alloc> dest,
    Alloc &alloc) noexcept(std::is_nothrow_copy_constructible_v<alloc_val_t<Alloc>>);

template <typename Alloc, typename FwdIt>
constexpr alloc_raw_ptr_t<Alloc> uninitialized_move_n(
    FwdIt first, size_t count, alloc_raw_ptr_t<Alloc> dest,
    Alloc &alloc) noexcept(std::is_nothrow_move_constructible_v<alloc_val_t<Alloc>>);

template <typename Alloc, typename FwdIt>
constexpr alloc_raw_ptr_t<Alloc>
uninitialized_copy(FwdIt first, FwdIt last, alloc_raw_ptr_t<Alloc> dest,
                   Alloc &alloc) noexcept(std::is_nothrow_copy_constructible_v<alloc_val_t<Alloc>>);

template <typename Alloc, typename FwdIt>
constexpr alloc_raw_ptr_t<Alloc>
uninitialized_move(FwdIt first, FwdIt last, alloc_raw_ptr_t<Alloc> dest,
                   Alloc &alloc) noexcept(std::is_nothrow_move_constructible_v<alloc_val_t<Alloc>>);

template <typename Alloc, typename FwdIt>
constexpr alloc_raw_ptr_t<Alloc>
uninitialized_fill(FwdIt first, FwdIt last, const alloc_val_t<Alloc> &val,
                   Alloc &alloc) noexcept(std::is_nothrow_copy_constructible_v<alloc_val_t<Alloc>>);

template <typename Alloc, typename FwdIt>
constexpr alloc_raw_ptr_t<Alloc> uninitialized_default_construct(
    FwdIt first, FwdIt last,
    Alloc &alloc) noexcept(std::is_nothrow_default_constructible_v<alloc_val_t<Alloc>>);

template <typename FwdIt>
constexpr iter_ptr_t<FwdIt>
assign_move_n(FwdIt first, size_t count,
              FwdIt dest) noexcept(std::is_nothrow_move_assignable_v<iter_val_t<FwdIt>>);

template <typename FwdIt>
constexpr iter_ptr_t<FwdIt>
assign_copy_n(FwdIt first, size_t count,
              FwdIt dest) noexcept(std::is_nothrow_copy_assignable_v<iter_val_t<FwdIt>>);

template <typename FwdIt>
constexpr iter_ptr_t<FwdIt>
assign_move(FwdIt first, FwdIt last,
            FwdIt dest) noexcept(std::is_nothrow_move_assignable_v<iter_val_t<FwdIt>>);

template <typename FwdIt>
constexpr iter_ptr_t<FwdIt>
assign_copy(FwdIt first, FwdIt last,
            FwdIt dest) noexcept(std::is_nothrow_copy_assignable_v<iter_val_t<FwdIt>>);

template <typename Alloc, typename FwdIt>
constexpr bool is_nothrow_uninitialized_moveable_v = noexcept(
    internal::uninitialized_move(std::declval<FwdIt>(), std::declval<FwdIt>(),
                                 std::declval<alloc_raw_ptr_t<Alloc>>(), std::declval<Alloc &>()));

template <typename Alloc, typename FwdIt>
constexpr bool is_nothrow_uninitialized_copyable_v =
    noexcept(uninitialized_copy(std::declval<FwdIt>(), std::declval<FwdIt>(),
                                std::declval<alloc_raw_ptr_t<Alloc>>(), std::declval<Alloc &>()));

template <typename Alloc, typename FwdIt>
constexpr bool is_nothrow_uninitialized_fillable_v =
    noexcept(uninitialized_fill(std::declval<FwdIt>(), std::declval<FwdIt>(),
                                std::declval<alloc_val_t<Alloc> &>(), std::declval<Alloc &>()));

template <typename Alloc, typename FwdIt>
constexpr bool is_nothrow_uninitialized_default_constructible_v =
    noexcept(uninitialized_default_construct(std::declval<FwdIt>(), std::declval<FwdIt>(),
                                             std::declval<Alloc &>()));

template <typename FwdIt>
constexpr bool is_nothrow_copy_n_assignable_v = noexcept(assign_copy_n(
    std::declval<FwdIt>(), std::declval<size_t>(), std::declval<iter_ptr_t<FwdIt>>()));

template <typename FwdIt>
constexpr bool is_nothrow_move_n_assignable_v = noexcept(assign_move_n(
    std::declval<FwdIt>(), std::declval<size_t>(), std::declval<iter_ptr_t<FwdIt>>()));
} // namespace noyxcore::containers::internal

#include "containers_internal-inl.hpp"
