#pragma once
#include <type_traits/integer_sequence.hpp>

namespace xstl {
	namespace details {
		template<typename T, T N, T I = 0, T... Args>
		struct _make_integer_sequence_impl : _make_integer_sequence_impl<T, N, I + 1, Args..., I> {};

		template<typename T, T N, T... Args>
		struct _make_integer_sequence_impl<T, N, N, Args...> {
			using type = integer_sequence<T, Args...>;
		};
	}

	template<typename T, T N>
	using make_integer_sequence = typename details::_make_integer_sequence_impl<T, N>::type;

	template<xstl::size_t N>
	using make_index_sequence = make_integer_sequence<xstl::size_t, N>;

	template<typename... Args>
	using make_sequence_for = make_index_sequence<sizeof...(Args)>;
}