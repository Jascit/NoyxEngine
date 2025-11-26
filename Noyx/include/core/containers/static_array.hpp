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

                uninitialized_default_construct_n(alloc(), data_ptr(), N);
            }

            constexpr StaticArray(const StaticArray& other)
                : storage_({ _FirstZeroSecondArgs{},
                  traits::select_on_container_copy_construction(other.alloc()) })
            {
                allocate_storage();

                uninitialized_copy_n(alloc(), data_ptr(), N, other.data_ptr());
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


        private:
            void reset() noexcept
            {
                destroy_elements();
                if(kUseHeap)
                {
                    deallocate_storage();
				}
            }

            bool try_Steal_Resources(StaticArray&& other) noexcept
            {
                if constexpr (kUseHeap)
                {
                    if (alloc() == other.alloc())
                    {
                        storage_.second() = other.storage_.second();
                        other.storage_.second() = nullptr;
                        return true;
                    }
                }
                return false;
			}

            void prepare_buffer_for_write() {
                if constexpr (kUseHeap) {
                    if (storage_.second() == nullptr) {
                        allocate_storage();
                    }
                }
            }


        public:
            using POCMA = typename traits::propagate_on_container_move_assignment;
            using POCCA = typename traits::propagate_on_container_copy_assignment;
            using POCS = typename traits::propagate_on_container_swap;

            // --- Move Assignment ---
            StaticArray& operator=(StaticArray&& other) noexcept {
                if (this == &other) return *this;

                constexpr bool pocma = POCMA::value;

                bool do_full_reset = pocma;

                if constexpr (kUseHeap) {
                    if (alloc() == other.alloc()) do_full_reset = true;
                }

                if (do_full_reset) reset();
                else destroy_elements();

                if constexpr (pocma) {
                    alloc() = std::move(other.alloc());
                }

                if (try_steal_resources(std::move(other))) {
                    return *this;
                }

                prepare_buffer_for_write();

                uninitialized_move_n(alloc(), data_ptr(), N, other.data_ptr());

                return *this;
            }


            // --- Copy Assignment ---
            StaticArray& operator=(const StaticArray& other) {
                if (this == &other) return *this;

                constexpr bool pocca = POCCA::value;

                bool need_full_reset = false;
                if constexpr (pocca) {
                    if (alloc() != other.alloc()) {
                        need_full_reset = true;
                    }
                }

                if (need_full_reset) {
                    reset(); 
                    alloc() = other.alloc(); 
                }
                else {
                    destroy_elements();
                }

                prepare_buffer_for_write();

                uninitialized_copy_n(alloc(), data_ptr(), N, other.data_ptr());

                return *this;
            }


            // --- Swap ---
            void swap(StaticArray& other) noexcept(
                (traits::propagate_on_container_swap::value || traits::is_always_equal::value) &&
                (!kUseHeap || noexcept(std::swap(std::declval<Alloc&>(), std::declval<Alloc&>())))
                ) {
                if (this == &other) return;

                constexpr bool pocs = traits::propagate_on_container_swap::value;

                if constexpr (pocs) {
                    using std::swap;
                    swap(alloc(), other.alloc());
                }
                else {
                    // assert(alloc() == other.alloc());
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

            friend void swap(StaticArray& lhs, StaticArray& rhs) noexcept(noexcept(lhs.swap(rhs))) {
                lhs.swap(rhs);
            }



        private:
            // Function-helper to destroy elements from first to last;
            static void destroy_range(Alloc& a, T* first, T* last) noexcept
            {
                // Optimization: No need to call destructor for trivial types.
                if constexpr (!std::is_trivially_destructible_v<T>)
                {
                    while (last != first)
                    {
                        --last;
						traits::destroy(a, last);
                    }
                }
            }


            struct ContructionGuard
            {
                Alloc& alloc;
                T* first;
                T* current;
                bool commited;

                ContructionGuard(Alloc& a, T* start_ptr, T*& cur_ptr) : alloc(a), first(start_ptr), current(cur_ptr), commited(false) {}

                ~ContructionGuard()
                {
                    if (!commited)
                    {
                        destroy_range(alloc, first, current);
                    }
                }

                void commit() noexcept
                {
                    commited = true;
				}
            };


            
            private:
                static T* uninitialized_default_construct_n(Alloc& a, T* first, size_t n) {
                    if constexpr (std::is_trivially_default_constructible_v<T> && !std::is_volatile_v<T>) {
                        std::memset(first, 0, n * sizeof(T));
                        return first + n;
                    }
                    else {
                        T* current = first;
                        ConstructionGuard guard(a, first, current);
                        for (; n > 0; --n, ++current) {
                            traits::construct(a, current);
                        }
                        guard.commit();
                        return current;
                    }
                }


                static T* uninitialized_fill_n(Alloc& a, T* first, size_t n, const T& val) {
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
                            ConstructionGuard guard(a, first, current);
                            for (; n > 0; --n, ++current) {
                                traits::construct(a, current, val);
                            }
                            guard.commit();
                            return current;
                        }
                    }
                }


                template <typename InputIter>
                static T* uninitialized_copy_n(Alloc& a, T* dest, size_t n, InputIter src) {
                    constexpr bool kCanMemcpy =
                        std::is_trivially_copy_constructible_v<T> &&
                        std::is_pointer_v<InputIter> &&
                        !std::is_volatile_v<T>;

                    if constexpr (kCanMemcpy) {
                        if (n > 0) {
                            std::memcpy(dest, src, n * sizeof(T));
                        }
                        return dest + n;
                    }
                    else {
                        T* current = dest;
                        ConstructionGuard guard(a, dest, current);
                        for (; n > 0; --n, ++current, ++src) {
                            traits::construct(a, current, *src);
                        }
                        guard.commit();
                        return current;
                    }
                }


                static T* uninitialized_move_n(Alloc& a, T* dest, size_t n, T* src) {
                    constexpr bool kCanMemcpy =
                        std::is_trivially_move_constructible_v<T> &&
                        !std::is_volatile_v<T>;

                    if constexpr (kCanMemcpy) {
                        if (n > 0) std::memcpy(dest, src, n * sizeof(T));
                        return dest + n;
                    }
                    else {
                        T* current = dest;
                        ConstructionGuard guard(a, dest, current);
                        for (; n > 0; --n, ++current, ++src) {
                            traits::construct(a, current, std::move(*src));
                        }
                        guard.commit();
                        return current;
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