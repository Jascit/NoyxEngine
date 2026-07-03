/* SPDX-License-Identifier: Apache-2.0 /
/*
 * \file   THeapArray.hpp
 * \brief
 *
 * Copyright (c) 2026 Project Contributors
 * \author MaksymRbkh <https://github.com/MaksymRbkh>
 * \date   09.01.2026
 */

#include <algorithm>
#include <cassert>
#include <compare>
#include <containers/internal/containers_internal.hpp>
#include <memory/allocators/traits.hpp>
#include <memory>
#include <platform/assert.hpp>
#include <stdexcept>
#include <utility>

#pragma warning(push)
#pragma warning(disable : 4702)

namespace noyxcore::containers
{
template <typename T, typename Alloc = std::allocator<T>>
class THeapArray
{
public:
    using allocator_type         = Alloc;
    using value_type             = T;
    using size_type              = std::size_t;
    using pointer                = value_type *;
    using const_pointer          = const value_type *;
    using traits                 = std::allocator_traits<allocator_type>;
    using reference              = value_type &;
    using const_reference        = const value_type &;
    using iterator               = pointer;
    using const_iterator         = const_pointer;
    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using allocation_guard       = internal::AllocationGuard<T, Alloc>;
    friend allocation_guard;

    struct Storage
    {
        pointer m_first;
        pointer m_last;
    };

    constexpr THeapArray(const allocator_type &alloc = allocator_type())
        : m_storage {nullptr, nullptr},
          m_capacity(0),
          m_alloc(alloc) {};

    constexpr THeapArray(std::initializer_list<value_type> list,
                         const allocator_type &alloc = allocator_type())
        : THeapArray(alloc)
    {
        size_type n = list.size();
        if (n > 0)
        {
            pointer new_mem = m_alloc.allocate(n);
            allocation_guard guard(new_mem, n, m_alloc);
            internal::uninitialized_copy(list.begin(), list.end(), new_mem, m_alloc);
            guard.release();

            m_storage.m_first = new_mem;
            m_storage.m_last  = new_mem + n;
            m_capacity        = n;
        }
    }

    explicit THeapArray(size_type n, const allocator_type &alloc = allocator_type())
        : m_storage {nullptr, nullptr},
          m_capacity(n),
          m_alloc(alloc)
    {
        if (n > 0)
        {
            pointer new_mem = m_alloc.allocate(n);
            allocation_guard guard(new_mem, n, m_alloc);
            internal::uninitialized_default_construct(new_mem, new_mem + n, m_alloc);
            guard.release();

            m_storage.m_first = new_mem;
            m_storage.m_last  = new_mem + n;
        }
    }

    explicit THeapArray(size_type n, const_reference value,
                        const allocator_type &alloc = allocator_type())
        : m_storage {nullptr, nullptr},
          m_capacity(n),
          m_alloc(alloc)
    {
        if (n > 0)
        {
            pointer new_mem = m_alloc.allocate(n);
            allocation_guard guard(new_mem, n, m_alloc);
            internal::uninitialized_fill_n(new_mem, n, value, m_alloc);
            guard.release();

            m_storage.m_first = new_mem;
            m_storage.m_last  = new_mem + n;
        }
    }

    constexpr THeapArray(const THeapArray &other)
        : m_storage {nullptr, nullptr},
          m_capacity(other.m_capacity),
          m_alloc(traits::select_on_container_copy_construction(other.m_alloc))
    {
        if (m_capacity > 0)
        {
            pointer new_mem = m_alloc.allocate(m_capacity);
            allocation_guard guard(new_mem, m_capacity, m_alloc);
            auto new_end = internal::uninitialized_copy(other.m_storage.m_first,
                                                        other.m_storage.m_last, new_mem, m_alloc);
            guard.release();

            m_storage.m_first = new_mem;
            m_storage.m_last  = new_end;
        }
    }

    constexpr THeapArray(THeapArray &&other) noexcept
        : m_storage {other.m_storage.m_first, other.m_storage.m_last},
          m_capacity(other.m_capacity),
          m_alloc(std::move(other.m_alloc))
    {
        other.m_storage.m_first = nullptr;
        other.m_storage.m_last  = nullptr;
        other.m_capacity        = 0;
    }

    constexpr ~THeapArray() { cleanup(); }

    constexpr THeapArray &operator=(std::initializer_list<value_type> list)
    {
        THeapArray tmp(list, m_alloc);
        *this = std::move(tmp);
        return *this;
    }

    friend constexpr bool operator==(const THeapArray &a, const THeapArray &b)
    {
        if (a.size() != b.size())
            return false;
        return std::equal(a.begin(), a.end(), b.begin());
    }

    friend constexpr auto operator<=>(const THeapArray &a, const THeapArray &b)
    {
        return std::lexicographical_compare_three_way(a.begin(), a.end(), b.begin(), b.end());
    }

    // Copy Assignment
    constexpr THeapArray &operator=(const THeapArray &other)
    {
        if (this == &other)
            return *this;
        auto &my_alloc          = m_alloc;
        const auto &other_alloc = other.m_alloc;

        constexpr bool POCCA = traits::propagate_on_container_copy_assignment::value;
        if constexpr (POCCA)
        {
            if (m_alloc != other_alloc)
            {
                cleanup();
                m_alloc = other_alloc;
            }
        }

        if (m_capacity < other.size())
        {
            pointer new_mem = m_alloc.allocate(other.size());

            allocation_guard guard(new_mem, other.size(), m_alloc);
            internal::uninitialized_copy_n(other.m_storage.m_first, other.size(), new_mem, m_alloc);
            guard.release();
            cleanup();

            m_storage.m_first = new_mem;
            m_storage.m_last  = new_mem + other.size();
            m_capacity        = other.size();
        }
        else
        {
            if (other.size() > size())
            {
                std::copy(other.m_storage.m_first, other.m_storage.m_first + size(),
                          m_storage.m_first);
                internal::uninitialized_copy_n(other.m_storage.m_first + size(),
                                               other.size() - size(), m_storage.m_last, m_alloc);
            }
            else
            {
                std::copy(other.m_storage.m_first, other.m_storage.m_first + other.size(),
                          m_storage.m_first);
                destroy_range(m_storage.m_first + other.size(), m_storage.m_last);
            }
            m_storage.m_last = m_storage.m_first + other.size();
        }
        return *this;
    }

    // Move Assignment
    constexpr THeapArray &operator=(THeapArray &&other) noexcept
    {
        if (&other == this)
            return *this;
        auto &my_alloc       = m_alloc;
        auto &other_alloc    = other.m_alloc;
        constexpr bool POCMA = traits::propagate_on_container_move_assignment::value;

        if constexpr (POCMA)
        {
            cleanup();
            m_alloc = std::move(other_alloc);
            steal_from(other);
            return *this;
        }
        else if constexpr (traits::is_always_equal::value)
        {
            cleanup();
            steal_from(other);
            return *this;
        }
        else if (my_alloc == other_alloc)
        {
            cleanup();
            steal_from(other);
            return *this;
        }
        // POCMA = false && Allocators are not equal.
        if (m_capacity < other.size())
        {
            pointer new_mem = m_alloc.allocate(other.size());
            allocation_guard guard(new_mem, other.size(), m_alloc);
            internal::uninitialized_move_n(other.m_storage.m_first, other.size(), new_mem, m_alloc);
            guard.release();
            cleanup();

            m_storage.m_first = new_mem;
            m_storage.m_last  = new_mem + other.size();
            m_capacity        = other.size();
        }
        else
        {
            if (other.size() > size())
            {
                std::move(other.m_storage.m_first, other.m_storage.m_first + size(),
                          m_storage.m_first);
                internal::uninitialized_move_n(other.m_storage.m_first + size(),
                                               other.size() - size(), m_storage.m_last, m_alloc);
            }
            else
            {
                std::move(other.m_storage.m_first, other.m_storage.m_last, m_storage.m_first);
                destroy_range(m_storage.m_first + other.size(), m_storage.m_last);
            }
            m_storage.m_last = m_storage.m_first + other.size();
        }

        other.m_storage.m_last = other.m_storage.m_first;
        return *this;
    }

    constexpr void swap(THeapArray &other) noexcept
    {
        constexpr bool POCS = traits::propagate_on_container_swap::value;
        if constexpr (POCS)
        {
            std::swap(m_alloc, other.m_alloc);
        }
        else
        {
            NOYX_ASSERT_ABORT(m_alloc == other.m_alloc,
                              "Cannot swap containers with different allocators if POCS is false");
        }
        std::swap(m_storage, other.m_storage);
        std::swap(m_capacity, other.m_capacity);
    }

    friend constexpr void swap(THeapArray &lhs, THeapArray &rhs) noexcept { lhs.swap(rhs); }

    // Element access
    constexpr reference operator[](size_type index) noexcept
    {
        NOYX_ASSERT_ABORT(index < size(), "Index out of range");
        return m_storage.m_first[index];
    }

    constexpr const_reference operator[](size_type index) const noexcept
    {
        NOYX_ASSERT_ABORT(index < size(), "Index out of range");
        return m_storage.m_first[index];
    }

    constexpr reference at(size_type index)
    {
        if (index >= size())
        {
            throw std::out_of_range("Index out of range");
        }
        return m_storage.m_first[index];
    }

    constexpr const_reference at(size_type index) const
    {
        if (index >= size())
        {
            throw std::out_of_range("Index out of range");
        }
        return m_storage.m_first[index];
    }

    constexpr reference front() noexcept
    {
        NOYX_ASSERT_ABORT(!empty(), "Array is empty");
        return *m_storage.m_first;
    }

    constexpr const_reference front() const noexcept
    {
        NOYX_ASSERT_ABORT(!empty(), "Array is empty");
        return *m_storage.m_first;
    }

    constexpr reference back() noexcept
    {
        NOYX_ASSERT_ABORT(!empty(), "Array is empty");
        return *(m_storage.m_last - 1);
    }

    constexpr const_reference back() const noexcept
    {
        NOYX_ASSERT_ABORT(!empty(), "Array is empty");
        return *(m_storage.m_last - 1);
    }

    constexpr size_type size() const noexcept
    {
        return static_cast<size_type>(m_storage.m_last - m_storage.m_first);
    }

    constexpr size_type capacity() const noexcept { return m_capacity; }

    [[nodiscard]] constexpr bool empty() const noexcept { return this->begin() == this->end(); }

    constexpr pointer data() noexcept { return this->begin(); }

    constexpr const_pointer data() const noexcept { return this->begin(); }

    constexpr allocator_type get_allocator() const noexcept { return this->m_alloc; }

    // Iterators
    constexpr iterator begin() noexcept { return m_storage.m_first; }

    constexpr const_iterator begin() const noexcept { return m_storage.m_first; }

    constexpr iterator end() noexcept { return m_storage.m_last; }

    constexpr const_iterator end() const noexcept { return m_storage.m_last; }

    constexpr reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }

    constexpr const_reverse_iterator rbegin() const noexcept
    {
        return const_reverse_iterator(end());
    }

    constexpr reverse_iterator rend() noexcept { return reverse_iterator(begin()); }

    constexpr const_reverse_iterator rend() const noexcept
    {
        return const_reverse_iterator(begin());
    }

    constexpr const_iterator cbegin() const noexcept { return m_storage.m_first; }

    constexpr const_iterator cend() const noexcept { return m_storage.m_last; }

    constexpr const_reverse_iterator crbegin() const noexcept
    {
        return const_reverse_iterator(rbegin());
    }

    constexpr const_reverse_iterator crend() const noexcept
    {
        return const_reverse_iterator(rend());
    }

    // Modifiers
    constexpr iterator erase(const_iterator pos)
    {
        size_type index = pos - cbegin();
        pointer p       = m_storage.m_first + index;
        std::move(p + 1, m_storage.m_last, p);
        traits::destroy(m_alloc, m_storage.m_last - 1);
        --m_storage.m_last;
        return make_iter(p);
    }

    constexpr iterator erase(const_iterator first, const_iterator last)
    {
        NOYX_ASSERT_ABORT(first <= last, "vector::erase(first, last) called with invalid range");
        if (first == last)
        {
            return iterator(m_storage.m_first + (first - cbegin()));
        }
        size_type index_first = first - cbegin();
        pointer p_first       = m_storage.m_first + index_first;
        size_type index_last  = last - cbegin();
        pointer p_last        = m_storage.m_first + index_last;

        pointer new_end = std::move(p_last, m_storage.m_last, p_first);
        destroy_range(new_end, m_storage.m_last);
        m_storage.m_last = new_end;
        return make_iter(p_first);
    }

    constexpr iterator insert(const_iterator pos, const_reference value)
    {
        size_type index = pos - cbegin();
        if (size() == m_capacity)
        {
            reserve(m_capacity == 0 ? 1 : m_capacity * 2);
        }
        pointer p = m_storage.m_first + index;
        if (p == m_storage.m_last)
        {
            push_back(value);
        }
        else
        {
            traits::construct(m_alloc, m_storage.m_last, std::move(*(m_storage.m_last - 1)));
            std::move_backward(p, m_storage.m_last - 1, m_storage.m_last);
            *p = value;
            ++m_storage.m_last;
        }
        return make_iter(p);
    }

    constexpr void assign(size_type n, const_reference value)
    {
        if (n <= m_capacity)
        {
            if (n <= size())
            {
                std::fill_n(begin(), n, value);
                destroy_range(m_storage.m_first + n, m_storage.m_last);
            }
            else
            {
                std::fill(begin(), end(), value);
                internal::uninitialized_fill_n(m_storage.m_last, n - size(), value, m_alloc);
            }
            m_storage.m_last = m_storage.m_first + n;
        }
        else
        {
            pointer new_mem = m_alloc.allocate(n);

            allocation_guard guard(new_mem, n, m_alloc);
            internal::uninitialized_fill_n(new_mem, n, value, m_alloc);
            guard.release();
            cleanup();

            m_storage.m_first = new_mem;
            m_storage.m_last  = new_mem + n;
            m_capacity        = n;
        }
    }

    constexpr void push_back(const_reference n) { emplace_back(n); }

    constexpr void push_back(value_type &&n) { emplace_back(std::move(n)); }

    template <class... Args>
    constexpr iterator emplace(const_iterator pos, Args &&...args)
    {
        size_type index = pos - cbegin();
        if (size() == m_capacity)
        {
            reserve(m_capacity == 0 ? 1 : m_capacity * 2);
        }
        pointer p = m_storage.m_first + index;
        if (p == m_storage.m_last)
        {
            emplace_back(std::forward<Args>(args)...);
        }
        else
        {
            traits::construct(m_alloc, m_storage.m_last, std::move(*(m_storage.m_last - 1)));
            std::move_backward(p, m_storage.m_last - 1, m_storage.m_last);
            traits::destroy(m_alloc, p);
            traits::construct(m_alloc, p, std::forward<Args>(args)...);
            ++m_storage.m_last;
        }
        return make_iter(p);
    }

    template <class... Args>
    constexpr void emplace_back(Args &&...args)
    {
        if (size() == m_capacity)
        {
            reserve(m_capacity == 0 ? 1 : m_capacity * 2);
        }
        traits::construct(m_alloc, m_storage.m_last, std::forward<Args>(args)...);
        ++m_storage.m_last;
    }

    constexpr void pop_back()
    {
        NOYX_ASSERT_ABORT(!empty(), "Array is empty");
        --m_storage.m_last;
        traits::destroy(m_alloc, m_storage.m_last);
    }

    constexpr void resize(size_type new_size)
    {
        if (new_size < size())
        {
            destroy_range(m_storage.m_first + new_size, m_storage.m_last);
            m_storage.m_last = m_storage.m_first + new_size;
        }
        else if (new_size > size())
        {
            size_type count = new_size - size();
            if (new_size > m_capacity)
            {
                pointer new_mem = m_alloc.allocate(new_size);
                allocation_guard guard(new_mem, new_size, m_alloc);

                internal::uninitialized_move_n(m_storage.m_first, size(), new_mem, m_alloc);
                internal::uninitialized_default_construct(new_mem + size(),
                                                          new_mem + size() + count, m_alloc);
                guard.release();
                cleanup();

                m_storage.m_first = new_mem;
                m_storage.m_last  = new_mem + new_size;
                m_capacity        = new_size;
            }
            else
            {
                internal::uninitialized_default_construct(m_storage.m_first + size(),
                                                          m_storage.m_last + count, m_alloc);
                m_storage.m_last = m_storage.m_first + new_size;
            }
        }
    }

    void reserve(size_type new_capacity)
    {
        if (new_capacity <= m_capacity)
            return;

        pointer new_mem   = m_alloc.allocate(new_capacity);
        size_type my_size = size();

        allocation_guard guard(new_mem, new_capacity, m_alloc);
        internal::uninitialized_move_n(m_storage.m_first, my_size, new_mem, m_alloc);
        guard.release();
        cleanup();

        m_storage.m_first = new_mem;
        m_storage.m_last  = new_mem + my_size;
        m_capacity        = new_capacity;
    }

    constexpr void shrink_to_fit()
    {
        if (m_capacity > size())
        {
            THeapArray temp(*this);
            swap(temp);
        }
    }

    constexpr void clear()
    {
        destroy_range(m_storage.m_first, m_storage.m_last);
        m_storage.m_last = m_storage.m_first;
    }

private:
    constexpr void destroy_range(pointer first, pointer last) noexcept
    {
        for (; first != last; ++first)
        {
            traits::destroy(m_alloc, first);
        }
    }

    constexpr void steal_from(THeapArray &other)
    {
        m_storage       = other.m_storage;
        m_capacity      = std::exchange(other.m_capacity, 0);
        other.m_storage = {nullptr, nullptr};
    }

    constexpr void cleanup()
    {
        if (!empty())
        {
            destroy_range(m_storage.m_first, m_storage.m_last);
        }
        if (m_storage.m_first != nullptr)
        {
            m_alloc.deallocate(m_storage.m_first, m_capacity);
        }
        m_storage.m_first = nullptr;
        m_storage.m_last  = nullptr;
        m_capacity        = 0;
    }

    constexpr iterator make_iter(pointer p) noexcept { return iterator(p); }

    constexpr const_iterator make_iter(const_pointer p) noexcept { return const_iterator(p); }

private:
    Storage m_storage;
    size_type m_capacity;
    allocator_type m_alloc;
};
} // namespace noyxcore::containers

#pragma warning(pop)