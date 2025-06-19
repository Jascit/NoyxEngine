#pragma once
#include <type_traits/integral_constant.hpp>

namespace xstl {
		template<typename T>
		struct is_polymorphic : bool_constant<__is_polymorphic(T)>{};

		template<typename T>
		constexpr bool is_polymorphic_v = is_polymorphic<T>::value;
}