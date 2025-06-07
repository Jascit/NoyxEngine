#pragma once

#include <iostream>
#include <tuple>
#include <is/is_same.hpp>
#include <tt_test_sufixes.hpp>
#include <tt_test.hpp>  // VALIDATE_TYPES(A,B)

namespace xstl_test_detail
{
    using all_test_types = std::tuple<
        bool, char, wchar_t, signed char, unsigned char,
        short, unsigned short, int, unsigned int,
        long, unsigned long, float, double, long double
    >;

    template <typename Tuple, template<typename> class Fn, std::size_t... Is>
    constexpr void for_each_type_impl(std::index_sequence<Is...>) {
      (Fn<std::tuple_element_t<Is, Tuple>>{}(), ...);
    }

    template <typename Tuple, template<typename> class Fn>
    constexpr void for_each_type() {
      xstl_test_detail::for_each_type_impl<Tuple, Fn>(
        std::make_index_sequence<std::tuple_size_v<Tuple>>{}
      );
    }

} // namespace xstl_test_detail

