#pragma once
#include "is_move_constructible.hpp"
#include "is_move_assignable.hpp"
#include "move.hpp"

namespace xstl {
	namespace details {
			template <typename T>
			concept _ConceptIsSwapable = is_move_constructible_v<T> && is_move_assignable_v<T>;
	}

  template<typename T>
  requires (!details::_ConceptIsSwapable<T>)
  constexpr void swap(T&, T&)
  {
    static_assert(details::_ConceptIsSwapable<T>, "xstl::swap<T> requires T to be MoveConstructible and MoveAssignable");
  }

	template<typename T>
  requires details::_ConceptIsSwapable<T>
	constexpr void swap(T& a, T& b) noexcept(is_nothrow_move_constructible_v<T> && is_nothrow_move_assignable_v<T>) {
			T temp = xstl::move(a);
			a = xstl::move(b);
			b = xstl::move(temp);
	}
}
