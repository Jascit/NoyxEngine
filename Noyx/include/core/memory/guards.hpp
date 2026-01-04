#pragma once
#include <memory>

template <typename Alloc, typename T>
struct ConstructionGuard {
    Alloc& alloc;
    T* first;
    T*& current;
    bool committed;

    ConstructionGuard(Alloc& a, T* start_ptr, T*& cur_ptr)
        : alloc(a), first(start_ptr), current(cur_ptr), committed(false) {
    }

    ~ConstructionGuard() {
        if (!committed) {
            using traits = std::allocator_traits<Alloc>;
            while (current != first) {
                --current;
                traits::destroy(alloc, current);
            }
        }
    }

    void commit() noexcept { committed = true; }
};


template <typename Alloc>
struct AllocationGuard {
    using allocator_traits = std::allocator_traits<Alloc>;
    using pointer = typename allocator_traits::pointer;
    using size_type = typename allocator_traits::size_type;
    Alloc& alloc;
    pointer ptr;
    size_type n;

    AllocationGuard(Alloc& a, pointer p, size_t count)
        : alloc(a), ptr(p), n(count) {
    }

    ~AllocationGuard() {
        if (ptr != nullptr) {
            using traits = std::allocator_traits<Alloc>;
            traits::deallocate(alloc, ptr, n);
            ptr = nullptr;
        }
    }

    void commit() noexcept { ptr = nullptr; }
};