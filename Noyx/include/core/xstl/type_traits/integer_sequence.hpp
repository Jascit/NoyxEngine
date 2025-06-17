#pragma once
#include <core/xstldef.hpp>

namespace xstl {
	template<typename T, T... Args>
	struct integer_sequence{
		using value_type = T;
		constexpr static xstl::size_t size() {
			return sizeof...(Args);
		}
	};
}