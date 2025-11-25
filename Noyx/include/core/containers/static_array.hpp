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
            using allocator_type = Alloc;
            using traits = std::allocator_traits<allocator_type>;
            using pointer = typename traits::pointer;
            using const_pointer = typename traits::const_pointer;
            using value_type = T;

            static constexpr size_t kStackThresholdBytes = 1024;
            static constexpr size_t kTotalBytes = sizeof(T) * N;
            static constexpr bool kUseHeap = kTotalBytes > kStackThresholdBytes;

            static constexpr bool kTrivialCopyMove =
                std::is_trivially_copyable_v<T> &&
                std::is_trivially_move_constructible_v<T>;

            static constexpr bool kTrivialDestroy = std::is_trivially_destructible_v<T>;

            struct StackStorage {
                alignas(T) std::byte buffer[kTotalBytes];
            };

            using StorageType = std::conditional_t<kUseHeap, T*, StackStorage>;

            noyx::utility::TCompressedPair<Alloc, StorageType> storage_;

            Alloc& alloc() noexcept { return storage_.first(); }
            const Alloc& alloc() const noexcept { return storage_.first(); }

            T* data_ptr() noexcept {
                if constexpr (kUseHeap) {
                    return storage_.second();
                }
                else {
                    return reinterpret_cast<T*>(storage_.second().buffer);
                }
            }

            const T* data_ptr() const noexcept {
                if constexpr (kUseHeap) {
                    return storage_.second();
                }
                else {
                    return reinterpret_cast<const T*>(storage_.second().buffer);
                }
            }

            void allocate_storage() {
                if constexpr (kUseHeap) {
                    storage_.second() = traits::allocate(alloc(), N);
                }
            }

            void deallocate_storage() {
                if constexpr (kUseHeap) {
                    if (storage_.second() != nullptr) {
                        traits::deallocate(alloc(), storage_.second(), N);
                    }
                }
            }

            void destroy_elements() {
                if constexpr (!kTrivialDestroy) {
                    T* ptr = data_ptr();
                    for (size_t i = 0; i < N; ++i) {
                        traits::destroy(alloc(), ptr + (N - 1 - i));
                    }
				}
            }

        public:
            explicit constexpr StaticArray(const allocator_type& a = allocator_type())
                : storage_({ _FirstZeroSecondArgs{}, a })
            {
                allocate_storage();

                if constexpr (!std::is_trivially_default_constructible_v<T>) {
                    T* ptr = data_ptr();
                    for (size_t i = 0; i < N; ++i) {
                        traits::construct(alloc(), ptr + i);
                    }
                }
                else {
                    T* ptr = data_ptr();
                    for (size_t i = 0; i < N; ++i) {
                        traits::construct(alloc(), ptr + i);
                    }
                }
            }

            constexpr StaticArray(const StaticArray& other)
                : storage_({ _FirstZeroSecondArgs{},
                  traits::select_on_container_copy_construction(other.alloc()) })
            {
                allocate_storage();

                T* dest = data_ptr();
                const T* src = other.data_ptr();

                if constexpr (kTrivialCopyMove) {
                    std::memcpy(dest, src, kTotalBytes);
                }
                else {
                    for (size_t i = 0; i < N; ++i) {
                        traits::construct(alloc(), dest + i, src[i]);
                    }
                }
            }

            StaticArray(StaticArray&& other) noexcept
                : storage_({ _FirstZeroSecondArgs{}, std::move(other.alloc()) })
            {
                if constexpr (kUseHeap) {
                    storage_.second() = other.storage_.second();
                    other.storage_.second() = nullptr;
                }
                else {
                    T* dest = data_ptr();
                    T* src = other.data_ptr();

                    if constexpr (kTrivialCopyMove) {
                        std::memcpy(dest, src, kTotalBytes);
                    }
                    else {
                        for (size_t i = 0; i < N; ++i) {
                            traits::construct(alloc(), dest + i, std::move(src[i]));
                        }
                    }
                }
            }

            ~StaticArray() {
                T* ptr = data_ptr();

                if constexpr (!kTrivialDestroy) {
                    for (size_t i = 0; i < N; ++i) {
                        traits::destroy(alloc(), ptr + (N - 1 - i));
                    }
                }

                deallocate_storage();
            }

        public:
            using POCMA = typename traits::propagate_on_container_move_assignment;
            using POCCA = typename traits::propagate_on_container_copy_assignment;
            using POCS = typename traits::propagate_on_container_swap;

            // --- Move Assignment ---
            StaticArray& operator=(StaticArray&& other) noexcept 
            {
                if(this == other) return *this;

                if constexpr (POCMA::value)
                {
                    if constexpr (kUseHeap)
                    {
                        destroy_elements();
						deallocate_storage();
                    }
                    else
                    {
                        destroy_elements();
                    }
					alloc() = std::move(other.alloc());
                }

                else
                {
                    destroy_elements();
                    if constexpr (kUseHeap) {
                        if (alloc() != other.alloc())
                        {
                            deallocate_storage();
                            allocate_storage();
                        }
                    }
                }

                if constexpr (kUseHeap)
                {
                    if (alloc() == other.alloc())
                    {
                        storage_.second() = other.storage_.second();
                        other.storage_.second() = nullptr;
                        return *this;
                    }
                }
                T* dest = data_ptr();
                T* src = other.data_ptr();
                for (size_t i = 0; i < N; ++i) {
                    traits::construct(alloc(), dest + i, std::move(src[i]));
                }
                return *this;
            }



			// --- Copy Assignment ---
            StaticArray& operator=(const StaticArray& other)
            {
                if (this == &other) return *this;

                if constexpr (POCCA::value)
                {
                    if constexpr (alloc() != other.alloc())
                    {
                        destroy_elements();
                        if constexpr (kUseHeap)
                        {
                            deallocate_storage();
                        }
                        alloc() = other.alloc();

                        if constexpr (kUseHeap)
                        {
                            allocate_storage();
                        }
                    }
                    else
                    {
                        destroy_elements();
                    }
                }
                else
                {
                    destroy_elements();
                }

				T* dest = data_ptr();
				const T* src = other.data_ptr();
                if constexpr (kTrivialCopyMove) {
                    std::memcpy(dest, src, kTotalBytes);
                }
                else {
                    for (size_t i = 0; i < N; ++i) {
                        traits::construct(alloc(), dest + i, src[i]);
                    }
				}
                return *this;
            }


            void swap(StaticArray& other) noexcept {
                if (this == &other) return;

                if constexpr (POCS::value) {
                    using std::swap;
                    swap(alloc(), other.alloc());
                }
                else {

                }

                if constexpr (kUseHeap) {
                    using std::swap;
                    swap(storage_.second(), other.storage_.second());
                }
                else {
                    T* a = data_ptr();
                    T* b = other.data_ptr();
                    for (size_t i = 0; i < N; ++i) {
                        using std::swap;
                        swap(a[i], b[i]);
                    }
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

            constexpr T* data() noexcept { return data_ptr(); }
            constexpr const T* data() const noexcept { return data_ptr(); }
        };
    }
}