#pragma once
#include <memory>
#include <cstring>
#include <type_traits>
#include "guards.hpp"

namespace noyx::memory {
    template <typename Alloc, typename T>
    void destroy_range(Alloc& a, T* first, T* last) noexcept {
        if constexpr (!std::is_trivially_destructible_v<T>) {
            using traits = std::allocator_traits<Alloc>;
            while (last != first) {
                --last;
                traits::destroy(a, last);
            }
        }
    }

    // Default Construct
    template <typename Alloc, typename T>
    T* uninitialized_default_construct_n(Alloc& a, T* first, size_t n) {
        using traits = std::allocator_traits<Alloc>;

        if constexpr (std::is_trivially_default_constructible_v<T> && !std::is_volatile_v<T>) {
            std::memset(first, 0, n * sizeof(T));
            return first + n;
        }
        else {
            T* current = first;
            ConstructionGuard<Alloc, T> guard(a, first, current);
            for (; n > 0; --n, ++current) {
                traits::construct(a, current);
            }
            guard.commit();
            return current;
        }
    }

    // Fill
    template <typename Alloc, typename T>
    T* uninitialized_fill_n(Alloc& a, T* first, size_t n, const T& val) {
        using traits = std::allocator_traits<Alloc>;

        constexpr bool kCanUseMemset =
            std::is_trivially_copy_constructible_v<T> &&
            !std::is_volatile_v<T> &&
            (sizeof(T) == 1);

        if constexpr (kCanUseMemset) {
            std::memset(first, static_cast<unsigned char>(val), n);
            return first + n;
        }
        else {
            if constexpr (std::is_trivially_copy_constructible_v<T>) {
                for (size_t i = 0; i < n; ++i) first[i] = val;
                return first + n;
            }
            else {
                T* current = first;
                ConstructionGuard<Alloc, T> guard(a, first, current);
                for (; n > 0; --n, ++current) {
                    traits::construct(a, current, val);
                }
                guard.commit();
                return current;
            }
        }
    }

    // Copy
    template <typename Alloc, typename T, typename InputIter>
    T* uninitialized_copy_n(Alloc& a, T* dest, size_t n, InputIter src) {
        using traits = std::allocator_traits<Alloc>;

        constexpr bool kCanMemcpy =
            std::is_trivially_copy_constructible_v<T> &&
            std::is_pointer_v<InputIter> &&
            !std::is_volatile_v<T>;

        if constexpr (kCanMemcpy) {
            if (n > 0) std::memcpy(dest, src, n * sizeof(T));
            return dest + n;
        }
        else {
            T* current = dest;
            ConstructionGuard<Alloc, T> guard(a, dest, current);
            for (; n > 0; --n, ++current, ++src) {
                traits::construct(a, current, *src);
            }
            guard.commit();
            return current;
        }
    }

    // Move
    template <typename Alloc, typename T>
    T* uninitialized_move_n(Alloc& a, T* dest, size_t n, T* src) {
        using traits = std::allocator_traits<Alloc>;

        constexpr bool kCanMemcpy =
            std::is_trivially_move_constructible_v<T> &&
            !std::is_volatile_v<T>;

        if constexpr (kCanMemcpy) {
            if (n > 0) std::memcpy(dest, src, n * sizeof(T));
            return dest + n;
        }
        else {
            T* current = dest;
            ConstructionGuard<Alloc, T> guard(a, dest, current);
            for (; n > 0; --n, ++current, ++src) {
                traits::construct(a, current, std::move(*src));
            }
            guard.commit();
            return current;
        }
    }

} // namespace noyx::memory