#pragma once
#include <type_traits/is_move_constructible.hpp>
#include <type_traits/is_move_assignable.hpp>
#include <type_traits/move.hpp>

namespace xstl {
	namespace details {
			template <typename T>
			concept _ConceptIsSwapable = is_move_constructible_v<T> && is_move_assignable_v<T>;
	}
	template<details::_ConceptIsSwapable T>
	constexpr void swap(T& a, T& b) noexcept(is_nothrow_move_constructible_v<T> && is_nothrow_move_assignable_v<T>) {
			T temp = move(a);
			a = move(b);
			b = move(temp);
	}
}