#pragma once

#include <iostream>
#include <tuple>
#include <xstl/type_traits/is_same.hpp>
#include <tt_test_sufixes.hpp>
#include <tt_test.hpp>  // VALIDATE_TYPES(A,B)

namespace xstl_test_detail
{
    // Оголошуємо свій список типів в одному місці:
    using all_test_types = std::tuple<
        bool, char, wchar_t, signed char, unsigned char,
        short, unsigned short, int, unsigned int,
        long, unsigned long, float, double, long double
    >;

    // Помічник: по T викликає двічі VALIDATE_TYPES
    template<
        template<typename> class Primary,   // name<T>::type
        template<typename> class Alias = Primary,     // name_t<T>
        typename T
    >
    constexpr void apply_one_type(const char* file, int line)
    {
        VALIDATE_TYPES_TYPE_2(typename Primary<T>::type, T, file, line);

        if constexpr (!xstl::is_same<Alias<T>, Primary<T>>)
        {
            VALIDATE_TYPES_TYPE_2(Alias<T>, T, file, line);
        }
    }
    template<
      template<typename> class Primary,               // name<T>::type
      template<typename> class Alias = Primary,       // name_t<T>
      typename T,
      typename Te                                     // expected
    >
    constexpr void apply_one_type_with_expected(const char* file, int line)
    {
        VALIDATE_TYPES_TYPE_2(typename Primary<T>::type, Te, file, line);

        if constexpr (!xstl::is_same<Alias<T>, Primary<T>>)
        {
            VALIDATE_TYPES_TYPE_2(Alias<T>, Te, file, line);
        }
    }

    // Фолд-виразом розкачуємо весь кортеж типів
    template<
        template<typename> class Primary,
        template<typename> class Alias = Primary
    >
    constexpr void run_transform_tests(const char* test_name, const char* file, int line)
    {
        std::cout << "[ RUN     ] " << test_name << "\n";
        //std::apply x2
        std::apply(
          []<typename... Ta>(std::tuple<Ta...>) {
            [] <typename... Ts>(std::tuple<Ts...>) {
              (xstl_test_detail::apply_one_type<Primary, Alias, Ts>(file, line), ...);
            }, xstl_test_detail::all_suffixes<Ta>{});
          }, xstl_test_detail::all_test_types{}
        );

        std::cout << "[      OK ] " << test_name << "\n";
    }
    template<
      template<typename> class Primary,
      template<typename> class Alias = Primary,
      typename Te                                          //expected
    >
    constexpr void run_transform_tests_with_expected(const char* test_name, const char* file, int line) {
      std::cout << "[ RUN     ] " << test_name << "\n";
      //std::apply x2
      std::apply(
          []<typename... Tt>(std::tuple<Tt...>) {
            [] <typename... Ts>(std::tuple<Ts...>) {
              (xstl_test_detail::apply_one_type_with_expected<Primary, Alias, Ts, Te>(file, line), ...);
            }, xstl_test_detail::all_suffixes<Te>{});
          }, xstl_test_detail::all_test_types{}
        );

        std::cout << "[      OK ] " << test_name << "\n";
    }
} // namespace xstl_test_detail


#define DECL_TRANSFORM_TEST(test_name, Metafunc)                       \
    void test_name##_entry()                                          \
    {                                                                 \
        /* Primary: Metafunc<T>::type, Alias: Metafunc##_t<T> */      \
        xstl_test_detail::run_transform_tests<                        \
            std::remove_reference_t<decltype(Metafunc<int>::type)>::template rebind,>( #test_name );\
    }                                                                                     

#define DECL_TRANSFORM_TEST_SIMPLE(test_name, Metafunc)                \
    void test_name()                                                   \
    {                                                                  \
        xstl_test_detail::run_transform_tests<Metafunc, Metafunc##_t>( \
            #test_name                                                \
        );                                                             \
    }

