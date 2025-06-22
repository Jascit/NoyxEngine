#include "common_reference.hpp"    // ��� ��������� � common_reference_t
#include <type_traits/is_same.hpp> // ��� xstl::is_same
#include <tt_test_detail.hpp>      // ��� ��������� NOYX_ASSERT_�

// ������ ������ � ��� ���� ���������
template<typename T, typename U, typename Expected>
constexpr void tt_common_reference_test_value() {
  constexpr bool actual = xstl::is_same<
    xstl::common_reference_t<T, U>,
    Expected
  >::value;

#if TEST_WITH_STATIC_ASSERT
  NOYX_ASSERT_TRUE_MESSAGE(
    actual,
    "common_reference<" #T ", " #U "> returned incorrect value"
  );
#else
  NOYX_ASSERT_TRUE_MESSAGE(
    actual == true,
    "common_reference<" << typeid(T).name()
    << ", " << typeid(U).name()
    << "> returned incorrect: actual = "
    << typeid(xstl::common_reference_t<T, U>).name()
    << ", expected = " << typeid(Expected).name()
  );
#endif
}

// ������ ������ � ��� ����� ��������� (�������)
template<typename T, typename U, typename V, typename Expected>
constexpr void tt_common_reference_test_value() {
  constexpr bool actual = xstl::is_same<
    xstl::common_reference_t<T, U, V>,
    Expected
  >::value;

#if TEST_WITH_STATIC_ASSERT
  NOYX_ASSERT_TRUE_MESSAGE(
    actual,
    "common_reference<" #T ", " #U ", " #V "> returned incorrect value"
  );
#else
  NOYX_ASSERT_TRUE_MESSAGE(
    actual == true,
    "common_reference<" << typeid(T).name()
    << ", " << typeid(U).name()
    << ", " << typeid(V).name()
    << "> returned incorrect: actual = "
    << typeid(xstl::common_reference_t<T, U, V>).name()
    << ", expected = " << typeid(Expected).name()
  );
#endif
}

struct TestTypeInvokerCommonReference {
  constexpr void operator()() const {
    // --- ���� ��������
    tt_common_reference_test_value<int, int, int>();

    // --- ��� ���������, ������ lvalue ? lvalue-ref
    tt_common_reference_test_value<int&, int&, int&>();
    // ������ rvalue ? rvalue-ref
    tt_common_reference_test_value<int&&, int&&, int&&>();
    // ����� ? lvalue-ref
    tt_common_reference_test_value<int&, int&&, int&>();
    tt_common_reference_test_value<int&&, int&, int&>();

    // --- ��� ��������� ����� ���� (std::common_type<int,long> == long)
    tt_common_reference_test_value<int&, long&, long&>();
    tt_common_reference_test_value<int&&, long&&, long&&>();
    tt_common_reference_test_value<int&, long&&, long&>();

    // --- ��� ��������� (�������)
    // �� lvalue ? lvalue-ref
    tt_common_reference_test_value<int&, int&, int&, int&>();
    // �� rvalue ? rvalue-ref
    tt_common_reference_test_value<int&&, int&&, int&&, int&&>();
    // ����� ? lvalue-ref
    tt_common_reference_test_value<int&, int&&, int&, int&>();

    // --- ��������� ������� �� ��������������� ������
    struct A { operator double() const; };
    struct B { operator int() const; };
    // std::common_type_t<A,B> == double
    tt_common_reference_test_value<A&, B&, double&>();
    tt_common_reference_test_value<A&&, B&&, double&&>();
    tt_common_reference_test_value<A&, B&&, double&>();
  }
};

NOYX_TEST(CommonReference, UnitTest) {
  TestTypeInvokerCommonReference{}();
}
