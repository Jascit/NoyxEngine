#pragma once
#include "integral_constant.hpp"
#include "remove_reference.hpp"
#include "declval.hpp"
#include "move.hpp"

//TODO: GCC, Clang
namespace xstl {
#if defined(_MSVC_LANG)
  template<typename T>
  struct is_move_constructible : bool_constant<__is_constructible(T, T)>{};

  template <typename T>
  constexpr bool is_move_constructible_v = is_move_constructible<T>::value;

  template <typename T>
  struct is_nothrow_move_constructible : bool_constant<__is_nothrow_constructible(T, T)> {};

  template <typename T>
  constexpr bool is_nothrow_move_constructible_v = is_nothrow_move_constructible<T>::value;

#else 
    namespace details {
        template<typename T>
        concept _ConceptIsMoveConstructible = requires(T && t) {
            T(move(t));
        };

        template<typename T>
        concept _ConceptIsNothrowMoveConstructible = requires(T && t) {
            { T(move(t)) } noexcept;
        };
    }

    template<typename T>
    struct is_move_constructible : false_type {};

    template<typename T>
    requires details::_ConceptIsMoveConstructible<T>
    struct is_move_constructible<T> : true_type {};

    template <typename T>
    constexpr bool is_move_constructible_v = is_move_constructible<T>::value;

    template<typename T>
    struct is_nothrow_move_constructible : false_type {};

    template<typename T>
    requires details::_ConceptIsNothrowMoveConstructible<T>
    struct is_nothrow_move_constructible<T> : true_type {};

    template <typename T>
    constexpr bool is_nothrow_move_constructible_v = is_nothrow_move_constructible<T>::value;
#endif // _MSVC_VER 
}
