/* SPDX-License-Identifier: Apache-2.0 */

/**
 * @file     TStaticArray.hpp
 * @brief    Simple fixed-capacity container that stores up to N elements of T
 *
 * @date     07.01.2026
 *
 * @copyright Copyright (c) 2026 The Project Contributors
 */

#pragma once
#include <containers/internal/containers_internal.hpp>
#include <memory>
#include <platform/assert.hpp>
#include <type_traits>

namespace noyxcore::containers
{

// Simple fixed-capacity container that stores up to N elements of T
// in an aligned char buffer and constructs elements with placement new.
// - does NOT default-construct the T objects
// - provides pushBack / emplace_back / operator[]
// - non-copyable, non-movable (policy can be changed)
template <typename T, size_t N>
class TStaticArray
{
public:
    using value_type      = T;
    using pointer         = T *;
    using const_pointer   = const T *;
    using reference       = T &;
    using const_reference = const T &;
    using size_type       = size_t;

    constexpr TStaticArray()
        : m_data {},
          m_last(data()) {};
    TStaticArray(const TStaticArray &) = delete;
    TStaticArray(TStaticArray &&)      = delete;

    constexpr TStaticArray(const T &val)
    {
        m_last = internal::uninitialized_fill_n(data(), N, val, memory::allocators::NoAlloc<T> {});
    };

    constexpr ~TStaticArray() { cleanup(); };

    constexpr reference operator[](size_type idx) noexcept
    {
#ifdef NOYX_CORE_DEBUG
        NOYX_CORE_ASSERT_ABORT(idx < size(), "TStaticArray: index out of bounds");
#endif
        return data()[idx];
    };

    constexpr const_reference operator[](size_type idx) const noexcept
    {
#ifdef NOYX_CORE_DEBUG
        NOYX_CORE_ASSERT_ABORT(idx < size(), "TStaticArray: index out of bounds");
#endif
        return data()[idx];
    };

    [[nodiscard]] constexpr size_type size() const noexcept
    {
        return m_last - std::launder(reinterpret_cast<const_pointer>(m_data));
    };

    [[nodiscard]] constexpr bool empty() const noexcept { return size() == 0; };

    constexpr pointer data() noexcept { return std::launder(reinterpret_cast<pointer>(m_data)); };

    constexpr const_pointer data() const noexcept
    {
        return std::launder(reinterpret_cast<const_pointer>(m_data));
    };

    constexpr void
    push_back(const value_type &val) noexcept(std::is_nothrow_copy_constructible_v<value_type>)
    {
        emplace_back(val);
    };

    constexpr void
    push_back(value_type &&val) noexcept(std::is_nothrow_copy_constructible_v<value_type> ||
                                         std::is_nothrow_move_assignable_v<value_type>)
    {
        emplace_back(std::forward<value_type>(val));
    };

    template <typename... Args>
    constexpr reference emplace_back(Args &&...args)
    {
        size_type current_size = size();
        NOYX_ASSERT_ABORT(current_size < N, "TStaticArray::emplace_back: capacity exceeded");
        new (static_cast<void *>(m_last)) T(std::forward<Args>(args)...);
        ++m_last;
        return *(m_last - 1);
    };

private:
    constexpr void cleanup() noexcept(std::is_nothrow_destructible_v<value_type>)
    {
        if (m_last != data())
            destroy_range(data(), m_last);
    };

    constexpr void destroy_range(pointer first,
                                 pointer last) noexcept(std::is_nothrow_destructible_v<value_type>)
    {
        if constexpr (!std::is_trivially_destructible_v<value_type>)
        {
            for (pointer it = first; it < last; it++)
            {
                std::destroy_at(it);
            }
        }
    };

private:
    alignas(alignof(value_type)) char m_data[sizeof(value_type) * N];
    pointer m_last;
};
} // namespace noyxcore::containers
