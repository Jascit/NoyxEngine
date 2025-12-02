#pragma once

#include <platform/typedef.hpp>
#include <utility/utility.hpp>
#include <type_traits>
#include <memory>
#include "memory/guards.hpp"
#include "memory/uninitialized.hpp"
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
                alignas(T) unsigned char buffer[kTotalBytes];
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


        public:
            explicit StaticArray(const allocator_type& a = allocator_type())
                : storage_({ _FirstZeroSecondArgs{}, a })
            {
                allocate_storage();
                if constexpr (kUseHeap)
                {
                    AllocationGuard mem_guard(alloc(), data_ptr(), N);
                    uninitialized_default_construct_n(alloc(), data_ptr(), N);
                    mem_guard.commit();
                }
                else
                {
                    uninitialized_default_construct_n(alloc(), data_ptr(), N);
                }
            }

            StaticArray(const StaticArray& other)
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
                noyx::memory::destroy_range(alloc(), data_ptr(), data_ptr() + N);
                if (kUseHeap)
                {
                    deallocate_storage();
                }
            }

            bool try_steal_resources(StaticArray&& other) noexcept
            {
                if constexpr (kUseHeap)
                {
                    if (traits::is_always_equal::value || alloc() == other.alloc())
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
                else noyx::memory::destroy_range(alloc(), data_ptr(), data_ptr() + N);

                if constexpr (pocma) {
                    alloc() = std::move(other.alloc());
                }

                if (noyx::memory::try_steal_resources(std::move(other))) {
                    return *this;
                }

                prepare_buffer_for_write();

                if constexpr (kUseHeap) {
                    AllocationGuard mem_guard(alloc(), storage_.second(), N);
                    uninitialized_move_n(alloc(), data_ptr(), N, other.data_ptr());
                    mem_guard.commit();
                }
                else {
                    uninitialized_move_n(alloc(), data_ptr(), N, other.data_ptr());
                }

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
                    noyx::reset();
                    alloc() = other.alloc(); 
                }
                else {
                    destroy_range(alloc(), data_ptr(), data_ptr() + N);
                }

                prepare_buffer_for_write();

                if constexpr (kUseHeap) {
                    AllocationGuard mem_guard(alloc(), storage_.second(), N);

                    uninitialized_copy_n(alloc(), data_ptr(), N, other.data_ptr());

                    mem_guard.commit(); 
                }
                else {
                    uninitialized_copy_n(alloc(), data_ptr(), N, other.data_ptr());
                }

                return *this;
            }


            // --- Swap ---
            void swap(StaticArray& other) noexcept(
                (traits::propagate_on_container_swap::value || traits::is_always_equal::value) && 
                (kUseHeap || std::is_nothrow_swappable_v<T>)
                ) {
                if (this == &other) return;

                constexpr bool pocs = traits::propagate_on_container_swap::value;

                if constexpr (pocs) {
                    using std::swap;
                    swap(alloc(), other.alloc());
                }
                else {
                    static_assert(alloc() == other.alloc());
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

            constexpr T* begin() noexcept { return data_ptr(); }
			constexpr const T* begin() const noexcept { return data_ptr(); }

			constexpr T* end() noexcept { return data_ptr() + N; }
			constexpr const T* end() const noexcept { return data_ptr() + N; }

        public:
            void fill(T* first, T* last, const T& value)
            {
                while (first != last)
                {
                    *first = value;
                    first++;
                }
            }

            void assign(size_t count, const T& value)
            {
                if (count > N) {
                    assert(index > N && "StaticArray::replace_at: index out of bounds");
                }

                for (size_t i = 0; i < count; ++i) {
                    data_ptr()[i] = value;
                }
            }
             
            template <typename... Args>
            T& replace_at(size_t index, Args&&... args)
            {
                assert(index > N && "StaticArray::replace_at: index out of bounds");
				T* ptr = data_ptr() + index;
				traits::destroy(alloc(), ptr);

				traits::construct(alloc(), ptr, std::forward<Args>(args)...);
				return *ptr;
            }

}