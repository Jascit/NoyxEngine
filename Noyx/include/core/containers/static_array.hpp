#pragma once

#include <platform/typedef.hpp>
#include <utility/utility.hpp>
#include <type_traits>
#include <memory>
#include <cstring>
#include "iterators/iterators.hpp"

namespace noyx {
	namespace containers {
        template<typename T, size_t N, typename Alloc = std::allocator<T>>
        class StaticArray {
        private:

            static constexpr bool kTrivial = std::is_trivially_copyable_v<T>;

            T inline_data_[N];

            using pointer = T*;

            noyx::utility::TCompressedPair<T[N], Alloc> storage_;

            size_t size_ = N;

            pointer data_ptr() noexcept { return storage_.first(); }
            const pointer data_ptr() const noexcept { return storage_.first(); }

            Alloc& alloc() noexcept { return storage_.second(); }
            const Alloc& alloc() const noexcept { return storage_.second(); }


        public:
            explicit StaticArray(const Alloc& a = Alloc()) noexcept : storage_({ _FirstZeroSecondArgs{}, a }) {}


            StaticArray(const StaticArray& other) : storage_({ _FirstZeroSecondArgs{}, other.alloc() })                          //copy-constructor
            {
                noyx::utility::_initialized_copy_n(other.data_ptr(), N, data_ptr(), alloc());
            }

            StaticArray(StaticArray&& other) noexcept : storage_({_FirstZeroSecondArgs{}, std::move(other.alloc())})           //move-constructor
            {
                if constexpr (kTrivial) {
                    std::memcpy(data_ptr(), other.data_ptr(), sizeof(T) * N);
                }
                else {
                    for (size_t i = 0; i < N; ++i)
                        new(&data_ptr()[i]) T(std::move(other.data_ptr()[i]));
                }
            }

            ~StaticArray() {
                if constexpr (!std::is_trivially_destructible_v<T>) {
                    for (size_t i = 0; i < N; ++i)
                        alloc().destroy(data_ptr() + i);
                }
            }

        public:
            T& operator[](size_t i) noexcept { return data_ptr()[i]; }
            const T& operator[](size_t i) const noexcept { return data_ptr()[i]; }

            size_t size() const noexcept { return size_; }
            bool empty() const noexcept { return size_ == 0; }

            T* at(size_t i) noexcept { return (i < N) ? &data_ptr()[i] : nullptr; }
            const T* at(size_t i) const noexcept { return (i < N) ? &data_ptr()[i] : nullptr; }

            T& front() noexcept { return data_ptr()[0]; }
            const T& front() const noexcept { return data_ptr()[0]; }

            T& back() noexcept { return data_ptr()[N - 1]; }
            const T& back() const noexcept { return data_ptr()[N - 1]; }
		};
	}
}