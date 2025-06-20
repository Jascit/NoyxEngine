#pragma once
#include "remove_cvref_t.hpp"
#include "is_function.hpp"
#include "is_array.hpp"
#include "conditional.hpp"
#include "add_pointer.hpp"
#include "remove_all_extents.hpp"

namespace xstl {
	namespace details {
		template<typename T>
		struct _details_helper {
			using Tf = conditional_t<is_function_v<T>, add_pointer_t<T>, remove_cv_t<T>>;
			using Tfa = conditional_t<is_array_v<T>, add_pointer_t<remove_all_extents_t<T>>, Tf>;
			using type = Tfa;
		};
	}
	template<typename T>
	struct decay : details::_details_helper<remove_reference_t<T>> {};

	template<typename T>
	using decay_t = decay<T>::type;
}

