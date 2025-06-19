#pragma once
#include <type_traits/integral_constant.hpp>

namespace xstl {
		template<typename T>
		struct is_final : bool_constant<__is_final(T)>{};

		template<typename T>
		constexpr bool is_final_v = is_final<T>::value;
}