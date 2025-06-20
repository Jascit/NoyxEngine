#include <type_traits/is_trivial.hpp>
#include <tt_test_detail.hpp>
#include <type_traits.hpp>

template<typename T>
constexpr void tt_is_trivial_test_value(bool expected) {
  constexpr bool actual = xstl::is_trivial<T>::value;

  NOYX_ASSERT_TRUE_MESSAGE(
    actual == expected,
    "is_trivial<T> returned incorrect value"
  );
}

struct TrivialStruct {
  int x;
  double y;
};

struct NonTrivialCtor {
  NonTrivialCtor() {}
};

struct NonTrivialDtor {
  ~NonTrivialDtor() {}
};

struct WithVirtual {
  virtual void foo() {}
};

struct TestTypeInvokerIsTrivial {
  constexpr void operator()() const {
    tt_is_trivial_test_value<int>(true);
    tt_is_trivial_test_value<double>(true);
    tt_is_trivial_test_value<TrivialStruct>(true);

    tt_is_trivial_test_value<NonTrivialCtor>(false);
    tt_is_trivial_test_value<NonTrivialDtor>(false);
    tt_is_trivial_test_value<WithVirtual>(false);

    tt_is_trivial_test_value<int[5]>(true);
  }
};

NOYX_TEST(IsTrivial, UnitTest) {
  TestTypeInvokerIsTrivial{}();
}
