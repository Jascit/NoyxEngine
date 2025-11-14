#pragma once

#include <platform/typedef.hpp>
#include <utility/utility.hpp>
#include <type_traits>
#include <memory>
#include <cstring>
#include "iterators/iterators.hpp"

namespace noyx {
	namespace containers {
        template <typename First, typename Second>
        class compressed_pair : private First, private Second
        {
        public:
            compressed_pair(const First& f, const Second& s)
                : First(f), Second(s) {
            }

            compressed_pair(First&& f, Second&& s)
                : First(std::move(f)), Second(std::move(s)) {
            }

            First& first() noexcept { return static_cast<First&>(*this); }
            const First& first() const noexcept { return static_cast<const First&>(*this); }

            Second& second() noexcept { return static_cast<Second&>(*this); }
            const Second& second() const noexcept { return static_cast<const Second&>(*this); }
        };


        template<typename T, size_t N, typename Alloc = std::allocator<T>>
        class StaticArray {
        private:

            static constexpr bool kTrivial = std::is_trivially_copyable_v<T>;

            T inline_data_[N];

            using pointer = T*;

            compressed_pair<pointer, Alloc> storage_;

            size_t size_ = N;

            pointer data_ptr() noexcept { return storage_.first(); }
            const pointer data_ptr() const noexcept { return storage_.first(); }

            Alloc& alloc() noexcept { return storage_.second(); }
            const Alloc& alloc() const noexcept { return storage_.second(); }


        public:
            explicit StaticArray(const Alloc& alloc = Alloc()) noexcept : storage_(inline_data_, alloc) {} 


            StaticArray(const StaticArray& other) : storage_(inline_data_, other.alloc())                          //copy-constructor
            {
                if constexpr (kTrivial) {
                    std::memcpy(inline_data_, other.inline_data_, sizeof(T) * N);
                }
                else {
                    for (size_t i = 0; i < N; ++i) {
                        new (&inline_data_[i]) T(other.inline_data_[i]);
                    }
                }
            }

            StaticArray(StaticArray&& other) noexcept : storage_(inline_data_, std::move(other.alloc()))           //move-constructor
            {
                if constexpr (kTrivial) {
                    std::memcpy(inline_data_, other.inline_data_, sizeof(T) * N);
                }
                else {
                    for (size_t i = 0; i < N; ++i) {
                        new (&inline_data_[i]) T(std::move(other.inline_data_[i]));
                    }
                }
            }

            ~StaticArray() noexcept {                                                                              //destructor
                if constexpr (!std::is_trivially_destructible_v<T>) {
                    for (size_t i = 0; i < size_; ++i) {
                        inline_data_[i].~T();
                    }
                }
            }

        public:
            T& operator[](size_t i) noexcept { return data_ptr()[i]; }
            const T& operator[](size_t i) const noexcept { return data_ptr()[i]; }

            size_t size() const noexcept { return size_; }
            bool empty() const noexcept { return size_ == 0; }

            T& front() noexcept { return data_ptr()[0]; }
            const T& front() const noexcept { return data_ptr()[0]; }

            T& back() noexcept { return data_ptr()[N - 1]; }
            const T& back() const noexcept { return data_ptr()[N - 1]; }

            T* at(size_t i) noexcept {
                return (i < N) ? &data_ptr()[i] : nullptr;
            }

            const T* at(size_t i) const noexcept {
                return (i < N) ? &data_ptr()[i] : nullptr;
            }
		};
	}
}