#pragma once
#include "integral_constant.hpp"
#include "add_lvalue_reference.hpp"

//TODO: Clang, GCC
namespace xstl {
    template <typename T>
    struct is_move_assignable : bool_constant<__is_assignable(add_lvalue_reference_t<T>, T)> {};

    template <typename T>
    constexpr bool is_move_assignable_v = is_move_assignable<T>::value; 

    template <typename T>
    struct is_nothrow_move_assignable : bool_constant<__is_nothrow_assignable(add_lvalue_reference_t<T>, T)> {};

    template <typename T>
    constexpr bool is_nothrow_move_assignable_v = is_nothrow_move_assignable<T>::value;
}
