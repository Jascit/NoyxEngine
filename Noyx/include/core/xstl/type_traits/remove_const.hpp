#pragma once
#include <type_traits>
std::remove_const<int>;
namespace xstl {
  template<typename T>
	struct remove_const<T const>
	{
		using T type;
	};
  
	template<typename T>
	struct remove_const<T>
	{
		using T type;
	};

	template <typename T, xstl::size_t N>
	struct remove_const<const T[N]> {
		using type = T[N];
	};

	template <typename T>
	struct remove_const<const T[]> {
		using type = T[];
	};

	template <typename T>
	using remove_const_t = typename remove_const<T>::type;
}
