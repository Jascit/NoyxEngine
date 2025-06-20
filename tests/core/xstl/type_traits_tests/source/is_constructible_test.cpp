#include <type_traits/is_constructible.hpp>
#include <tt_test_detail.hpp>

template<bool Expected, typename T, typename... Args>
constexpr void tt_is_constructible_test_value() {
  constexpr bool actual1 = xstl::is_constructible<T, Args...>::value;
  constexpr bool actual2 = xstl::is_constructible_v<T, Args...>;

#if TEST_WITH_STATIC_ASSERT
  NOYX_ASSERT_TRUE_MESSAGE(
    actual1 == Expected,
    "is_constructible<" #T ", " #Args "> returned incorrect value"
  );
  NOYX_ASSERT_TRUE_MESSAGE(
    actual2 == Expected,
    "is_constructible_v<" #T ", " #Args "> returned incorrect value"
  );
#else
  NOYX_ASSERT_TRUE_MESSAGE(
    actual1 == Expected,
    "is_constructible<" << typeid(T).name() << ", "
    << (sizeof...(Args) ? typeid(std::tuple<Args...>).name() : "void")
    << "> actual = " << actual1
    << ", expected = " << Expected
  );
  NOYX_ASSERT_TRUE_MESSAGE(
    actual2 == Expected,
    "is_constructible_v<" << typeid(T).name() << ", "
    << (sizeof...(Args) ? typeid(std::tuple<Args...>).name() : "void")
    << "> actual = " << actual2
    << ", expected = " << Expected
  );
#endif
}

struct TestTypeInvokerIsConstructible {
  constexpr void operator()() const {
    tt_is_constructible_test_value<true, int>();           
    tt_is_constructible_test_value<true, int, int>();      
    tt_is_constructible_test_value<true, int, double>();   

    tt_is_constructible_test_value<false, int, const char*>();

    struct A {
      A() = default;
      explicit A(int) {}
      A(double) = delete;
    };
    tt_is_constructible_test_value<true, A>();             
    tt_is_constructible_test_value<true, A, int>();        
    tt_is_constructible_test_value<false, A, double>();    

    struct B {
      B(int) {}
    };
    tt_is_constructible_test_value<false, B>();           
    tt_is_constructible_test_value<true, B, int>();       

    tt_is_constructible_test_value<true, int&, int&>();
    tt_is_constructible_test_value<false, int&, int>();    

    tt_is_constructible_test_value<true, const int&, int>();
    tt_is_constructible_test_value<false, volatile double&, double>();
    
    tt_is_constructible_test_value<true, int[3]>();       
    
    struct C { int x; double y; };
    tt_is_constructible_test_value<true, C>();            
    tt_is_constructible_test_value<true, C, int, double>();

    tt_is_constructible_test_value<true, int*, std::nullptr_t>();
    tt_is_constructible_test_value<true, int*, int*>();
    tt_is_constructible_test_value<false, int*, int&>();

    tt_is_constructible_test_value<true, std::pair<int, double>, int, double>();
    tt_is_constructible_test_value<true, std::pair<int, double>, double, int>();
  }
};

NOYX_TEST(IsConstructible, UnitTest) {
  TestTypeInvokerIsConstructible{}();
}
