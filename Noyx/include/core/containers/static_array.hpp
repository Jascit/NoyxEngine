#pragma once
#include <platform/typedef.hpp>
#include <utility/utility.hpp>
#include <utility/macros.hpp>
#include <type_traits>
#include <memory>
#include "memory/guards.hpp"
#include "memory/uninitialized.hpp"
#include <stdexcept>
#include <cstring>
#include "iterators/iterators.hpp"


namespace noyx {
    namespace containers {
        template<typename T, size_t N>
        class StaticArray {
			using _FirstZeroSecondArgs = noyx::utility::_FirstZeroSecondArgs;
            using _FirstOneSecondArgs = noyx::utility::_FirstOneSecondArgs;
        private:
            using allocator_type = std::allocator<T>;
            using traits = std::allocator_traits<allocator_type>;
            using pointer = typename traits::pointer;
            using const_pointer = typename traits::const_pointer;
            using value_type = T;
            using iterator = MyIterator<T, false>;
            using const_iterator = MyIterator<T, true>;
            using reverse_iterator = std::reverse_iterator<iterator>;
            using const_reverse_iterator = std::reverse_iterator<const_iterator>;

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

            noyx::utility::TCompressedPair<allocator_type, StorageType> storage_;

            allocator_type& alloc() noexcept { return storage_.first(); }
            const allocator_type& alloc() const noexcept { return storage_.first(); }

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
            static_assert(N > 0, "Static array must be > 0");

			// --- Default Constructor ---
            explicit StaticArray(const allocator_type& a = allocator_type())
                : storage_( _FirstOneSecondArgs{}, a)
            {
                allocate_storage();
                if constexpr (kUseHeap)
                {
                    AllocationGuard<allocator_type> mem_guard(alloc(), data_ptr(), N);
                    noyx::memory::uninitialized_default_construct_n(alloc(), data_ptr(), N);
                    mem_guard.commit();
                }
                else
                {
                    noyx::memory::uninitialized_default_construct_n(alloc(), data_ptr(), N);
                }
            }


			// --- Copy Constructor ---
            StaticArray(const StaticArray& other)
                : storage_( _FirstOneSecondArgs{},
                  traits::select_on_container_copy_construction(other.alloc()))
            {
                allocate_storage();

                noyx::memory::uninitialized_copy_n(alloc(), data_ptr(), N, other.data_ptr());
            }


			// --- Move Constructor ---
            StaticArray(StaticArray&& other) noexcept
                : storage_(_FirstOneSecondArgs{}, std::move(other.alloc()))
            {
                if constexpr (kUseHeap) {
                    storage_.second() = other.storage_.second();
                    other.storage_.second() = nullptr;
                }
                else {
                    noyx::memory::uninitialized_move_n(alloc(), data_ptr(), N, other.data_ptr());
                }
            }


			// --- Destructor ---
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

                if (try_steal_resources(std::move(other))) {
                    return *this;
                }

                prepare_buffer_for_write();

                if constexpr (kUseHeap) {
                    AllocationGuard mem_guard(alloc(), storage_.second(), N);
                    noyx::memory::uninitialized_move_n(alloc(), data_ptr(), N, other.data_ptr());
                    mem_guard.commit();
                }
                else {
                    noyx::memory::uninitialized_move_n(alloc(), data_ptr(), N, other.data_ptr());
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
                    reset();
                    alloc() = other.alloc();
                }
                else {
                    noyx::memory::destroy_range(alloc(), data_ptr(), data_ptr() + N);
                }

                prepare_buffer_for_write();

                if constexpr (kUseHeap) {
                    AllocationGuard mem_guard(alloc(), storage_.second(), N);

                    noyx::memory::uninitialized_copy_n(alloc(), data_ptr(), N, other.data_ptr());

                    mem_guard.commit();
                }
                else {
                    noyx::memory::uninitialized_copy_n(alloc(), data_ptr(), N, other.data_ptr());
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
			// --- Element Access ---
            constexpr T& operator[](size_t i) noexcept { return data_ptr()[i]; }
            constexpr const T& operator[](size_t i) const noexcept { return data_ptr()[i]; }

            constexpr size_t size() const noexcept { return N; }

            constexpr T& at(size_t i) { return (i < N) ? data_ptr()[i] : throw std::out_of_range("StaticArray::at: index out of bounds"); }
            constexpr const T& at(size_t i) const { return (i < N) ? data_ptr()[i] : throw std::out_of_range("StaticArray::at: index out of bounds"); }

            constexpr T& front() noexcept { return data_ptr()[0]; }
            constexpr const T& front() const noexcept { return data_ptr()[0]; }

            constexpr T& back() noexcept { return data_ptr()[N - 1]; }
            constexpr const T& back() const noexcept { return data_ptr()[N - 1]; }

            constexpr T* data() noexcept { return data_ptr(); }
            constexpr const T* data() const noexcept { return data_ptr(); }


			// --- Iterators ---
            constexpr iterator begin() noexcept { return iterator(data_ptr()); }
            constexpr const_iterator begin() const noexcept { return iterator(data_ptr()); }
            constexpr const_iterator cbegin() const noexcept { return iterator(data_ptr()); }

            constexpr iterator end() noexcept { return iterator(data_ptr() + N); }
            constexpr const_iterator end() const noexcept { return iterator(data_ptr() + N); }
            constexpr const_iterator cend() const noexcept { return iterator(data_ptr() + N); }


			// --- Reverse Iterators ---
            constexpr reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
            constexpr const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
            constexpr reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
            constexpr const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }
            constexpr const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(end()); }
            constexpr const_reverse_iterator crend() const noexcept { return const_reverse_iterator(begin()); }


        public:
            
            template <typename Iterator>
            void fill(Iterator first, Iterator last, const T& value)
            {
                while (first != last)
                {
                    *first = value;
                    ++first;
                }
            }

            void assign(size_t count, const T& value)
            {
                if (count > N) {
                    throw std::length_error("StaticArray::assign: count > N");
                }

                for (size_t i = 0; i < count; ++i) {
                    data_ptr()[i] = value;
                }
            }

            template <typename... Args>
            T& replace_at(size_t index, Args&&... args)
            {
                if (index >= N) {
                    throw std::out_of_range("StaticArray::replace_at: index out of bounds");
                }

                T temp(std::forward<Args>(args)...); 

                T* ptr = data_ptr() + index;
                traits::destroy(alloc(), ptr);

                traits::construct(alloc(), ptr, std::move(temp));
                return *ptr;
            }
        };
    }
}