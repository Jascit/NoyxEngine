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
        template<typename> class Alias,     // name_t<T>
        typename T
    >
    void apply_one_type()
    {
        VALIDATE_TYPES(typename Primary<T>::type, T);

        if constexpr (!xstl::is_same<Alias<T>, void>)
        {
            VALIDATE_TYPES(Alias<T>, T);
        }
    }

    // Фолд-виразом розкачуємо весь кортеж типів
    template<
        template<typename> class Primary,
        template<typename> class Alias = Primary
    >
    void run_transform_tests(const char* test_name)
    {
        std::cout << "[ RUN ] " << test_name << "\n";
        //std::apply x2
        std::apply(
            []<typename... Ts>(std::tuple<Ts...>){
              (apply_one_type<Primary, Alias, Ts>(), ...);
            }, all_test_types{}
        );

        std::cout << "[ OK ] " << test_name << "\n";
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

