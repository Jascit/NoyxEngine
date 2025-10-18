#pragma once

namespace noyx {
  namespace memory {
    template<typename Alloc>
    struct TAllocatorTraits {
      using value_type = typename Alloc::value_type;
      using pointer = typename Alloc::pointer;
      using const_pointer = typename Alloc::const_pointer;
      using size_type = typename Alloc::size_type;
      using difference_type = typename Alloc::difference_type;

      template<typename U>
      using rebind_alloc = typename Alloc::template rebind_alloc<U>;

      static pointer allocate(Alloc& a, size_type n) { return a.allocate(n); }
      static void deallocate(Alloc& a, pointer p, size_type n) { a.deallocate(p, n); }

      template<typename... Args>
      static void construct(Alloc& a, pointer p, Args&&... args) { a.construct(p, std::forward<Args>(args)...); }
      static void destroy(Alloc& a, pointer p) { a.destroy(p); }

      //using propagate_on_container_copy_assignment = typename Alloc::propagate_on_container_copy_assignment;
      //using propagate_on_container_move_assignment = typename Alloc::propagate_on_container_move_assignment;
      //using propagate_on_container_swap = typename Alloc::propagate_on_container_swap;
      //using is_always_equal = typename Alloc::is_always_equal;

      //static Alloc select_on_container_copy_construction(const Alloc& a) { return a.select_on_container_copy_construction(); }
    };

    template<typename Alloc, typename T>
    struct _rebind_alloc {
      using type = TAllocatorTraits<Alloc>::template rebind_alloc<T>;
    };

    template<typename Alloc, typename T>
    using _rebind_alloc_t = _rebind_alloc<Alloc, T>::type;
  }
}