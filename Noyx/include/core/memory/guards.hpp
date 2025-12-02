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


template <typename Alloc, typename T>
struct AllocationGuard {
    Alloc& alloc;
    T*& ptr_ref;
    size_t n;
    bool committed;

    AllocationGuard(Alloc& a, T*& p, size_t count)
        : alloc(a), ptr_ref(p), n(count), committed(false) {
    }

    ~AllocationGuard() {
        if (!committed && ptr_ref != nullptr) {
            using traits = std::allocator_traits<Alloc>;
            traits::deallocate(alloc, ptr_ref, n);
            ptr_ref = nullptr;
        }
    }

    void commit() noexcept { committed = true; }
};

}