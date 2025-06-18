#pragma once
#include <type_traits/void_t.hpp>
#include <type_traits/integral_constant.hpp>
#include <type_traits/remove_reference.hpp>
#include <type_traits/declval.hpp>
#include <type_traits/move.hpp>

//TODO: GCC, Clang
namespace xstl {
#if defined(_MSVC_LANG)
  template<typename T, typename = void>
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
            T(std::move(t));
        };

        template<typename T>
        concept _ConceptIsNothrowMoveConstructible = requires(T && t) {
            { T(std::move(t)) } noexcept;
        };
    }

    template<typename T>
    struct is_move_constructible : std::false_type {};

    template<details::_ConceptIsMoveConstructible T>
    struct is_move_constructible<T> : std::true_type {};

    template<typename T>
    struct is_nothrow_move_constructible : std::false_type {};

    template<details::_ConceptIsNothrowMoveConstructible T>
    struct is_nothrow_move_constructible<T> : std::true_type {};
#endif // _MSVC_VER 
}