#pragma once
#include <memory>
#include <type_traits>
#include <utility>

namespace noyx {
    namespace detail {
        template <typename Alloc, typename = void>
        struct get_propagate_on_container_copy_assignment : std::false_type {};

        template <typename Alloc>
        struct get_propagate_on_container_copy_assignment<
            Alloc,
            std::void_t<typename Alloc::propagate_on_container_copy_assignment>>
            : Alloc::propagate_on_container_copy_assignment {};

        template <typename Alloc, typename = void>
        struct get_propagate_on_container_move_assignment : std::false_type {};

        template <typename Alloc>
        struct get_propagate_on_container_move_assignment<
            Alloc,
            std::void_t<typename Alloc::propagate_on_container_move_assignment>>
            : Alloc::propagate_on_container_move_assignment {};

        template <typename Alloc, typename = void>
        struct get_propagate_on_container_swap : std::false_type {};

        template <typename Alloc>
        struct get_propagate_on_container_swap<
            Alloc,
            std::void_t<typename Alloc::propagate_on_container_swap>>
            : Alloc::propagate_on_container_swap {};


        template <typename Alloc, typename = void>
        struct get_pointer {
            using type = typename Alloc::value_type*;
        };

        template <typename Alloc>
        struct get_pointer<Alloc, std::void_t<typename Alloc::pointer>> {
            using type = typename Alloc::pointer;
        };

        template <typename Alloc, typename = void>
        struct get_const_pointer {
            using type = const typename Alloc::value_type*;
        };

        template <typename Alloc>
        struct get_const_pointer<Alloc, std::void_t<typename Alloc::const_pointer>> {
            using type = typename Alloc::const_pointer;
        };

        template <typename Alloc, typename = void>
        struct get_void_pointer {
            using type = void*;
        };

        template <typename Alloc>
        struct get_void_pointer<Alloc, std::void_t<typename Alloc::void_pointer>> {
            using type = typename Alloc::void_pointer;
        };

        template <typename Alloc, typename = void>
        struct get_const_void_pointer {
            using type = const void*;
        };

        template <typename Alloc>
        struct get_const_void_pointer<Alloc, std::void_t<typename Alloc::const_void_pointer>> {
            using type = typename Alloc::const_void_pointer;
        };

        template <typename Alloc, typename = void>
        struct get_size_type {
            using type = std::size_t;
        };

        template <typename Alloc>
        struct get_size_type<Alloc, std::void_t<typename Alloc::size_type>> {
            using type = typename Alloc::size_type;
        };

        template <typename Alloc, typename = void>
        struct get_difference_type {
            using type = std::ptrdiff_t;
        };

        template <typename Alloc>
        struct get_difference_type<Alloc, std::void_t<typename Alloc::difference_type>> {
            using type = typename Alloc::difference_type;
        };
    }

    template <typename Alloc>
    class allocator_traits {
    public:
        using allocator_type = Alloc;
        using value_type = typename Alloc::value_type;

        using pointer = typename detail::get_pointer<Alloc>::type;
        using const_pointer = typename detail::get_const_pointer<Alloc>::type;
        using void_pointer = typename detail::get_void_pointer<Alloc>::type;
        using const_void_pointer = typename detail::get_const_void_pointer<Alloc>::type;

        using size_type = typename detail::get_size_type<Alloc>::type;
        using difference_type = typename detail::get_difference_type<Alloc>::type;

        template <typename T> using rebind_alloc = typename Alloc::template rebind<T>;
        template <typename T> using rebind_traits = allocator_traits<rebind_alloc<T>>;

        // propagate_on_*
        using propagate_on_container_copy_assignment = detail::get_propagate_on_container_copy_assignment<Alloc>;
        using propagate_on_container_move_assignment = detail::get_propagate_on_container_move_assignment<Alloc>;
        using propagate_on_container_swap = detail::get_propagate_on_container_swap<Alloc>;
        using is_always_equal = std::is_empty<Alloc>;

        // allocate
        static pointer allocate(Alloc& a, size_type n) noexcept(noexcept(std::declval<Alloc&>().allocate(std::declval<size_type>()))) {
            return a.allocate(n);
        }

        // deallocate
        static void deallocate(Alloc& a, pointer p, size_type n) noexcept {
            a.deallocate(p, n);
        }

        // construct
        template <typename T, typename... Args>
        static void construct(Alloc& a, T* p, Args&&... args) {
            if constexpr (requires { a.construct(p, std::forward<Args>(args)...); })
                a.construct(p, std::forward<Args>(args)...);
            else
                std::construct_at(p, args...);
        }

        // destroy
        template <typename T>
        static void destroy(Alloc& a, T* p) noexcept {
            if constexpr (requires { a.destroy(p); })
                a.destroy(p);
            else
                std::destroy_at(p);
        }

        // max_size
        static size_type max_size(const Alloc& a) noexcept {
            if constexpr (requires { a.max_size(); })
                return a.max_size();
            else
                return static_cast<size_type>(-1) / sizeof(value_type);
        }

        // select_on_container_copy_construction
        static Alloc select_on_container_copy_construction(const Alloc& a) {
            if constexpr (requires { a.select_on_container_copy_construction(); })
                return a.select_on_container_copy_construction();
            else
                return a;
        }
    };
} // namespace noyx