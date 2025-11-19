#pragma once

#include <platform/typedef.hpp>
#include <utility/utility.hpp>
#include <type_traits>
#include <xmemory>
#include <memory>
#include <cstring>
#include "iterators/iterators.hpp"

namespace noyx {
	namespace containers {
        template<typename T, size_t N, typename Alloc = std::allocator<T>>
        class StaticArray {
        private:

            static constexpr bool kTrivialCopyMove =
                std::is_trivially_copyable_v<T> &&
                std::is_trivially_move_constructible_v<T>;


            static constexpr bool kTrivialDestroy = std::is_trivially_destructible_v<T>;

            using pointer = T*;
            using allocator_type = Alloc;
            using traits = std::allocator_traits<allocator_type>;

            noyx::utility::TCompressedPair<Alloc, T[N]> storage_;

            pointer data_ptr() noexcept { return storage_.second(); }
            const pointer data_ptr() const noexcept { return storage_.second(); }

            Alloc& alloc() noexcept { return storage_.first(); }
            const Alloc& alloc() const noexcept { return storage_.first(); }


        public:
                                                                                               // default ctor
            explicit StaticArray(const allocator_type& a = allocator_type()) noexcept
                : storage_({ _FirstZeroSecondArgs{}, a })
            {
                if constexpr (!std::is_trivially_default_constructible_v<T>) {
                    for (size_t i = 0; i < N; ++i)
                        ::new (data_ptr() + i) T();
                }
            }


                                                                                               // copy ctor
            StaticArray(const StaticArray& other)
                : storage_({ _FirstZeroSecondArgs{},
                traits::select_on_container_copy_construction(other.alloc()) })
            {
                if constexpr (kTrivialCopyMove) {
                    std::memcpy(data_ptr(), other.data_ptr(), sizeof(T) * N);
                }
                else {
                    for (size_t i = 0; i < N; ++i)
                        ::new (data_ptr() + i) T(other.data_ptr()[i]);
                }
            }


                                                                                                // move ctor
            StaticArray(StaticArray&& other) noexcept
                : storage_({ _FirstZeroSecondArgs{}, std::move(other.alloc()) })
            {
                if constexpr (kTrivialCopyMove) {
                    std::memcpy(data_ptr(), other.data_ptr(), sizeof(T) * N);
                }
                else {
                    for (size_t i = 0; i < N; ++i)
                        ::new (data_ptr() + i) T(std::move(other.data_ptr()[i]));
                }
            }


            ~StaticArray() {
                if constexpr (!kTrivialDestroy) {
                    for (size_t i = 0; i < N; ++i)
                        data_ptr()[i].~T();
                }
            }

        public:
            constexpr T& operator[](size_t i) noexcept { return data_ptr()[i]; }
            constexpr const T& operator[](size_t i) const noexcept { return data_ptr()[i]; }

            constexpr size_t size() const noexcept { return N; }

            constexpr T* at(size_t i) noexcept { return (i < N) ? &data_ptr()[i] : nullptr; }
            constexpr const T* at(size_t i) const noexcept { return (i < N) ? &data_ptr()[i] : nullptr; }

            constexpr T& front() noexcept { return data_ptr()[0]; }
            constexpr const T& front() const noexcept { return data_ptr()[0]; }

            constexpr T& back() noexcept { return data_ptr()[N - 1]; }
            constexpr const T& back() const noexcept { return data_ptr()[N - 1]; }
		};
	}
}