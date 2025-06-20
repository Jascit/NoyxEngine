#include <type_traits/is_abstract.hpp>
#include <type_traits/is_same.hpp>
#include <tt_test_detail.hpp>
#include <type_traits.hpp>

template<typename T>
constexpr void tt_is_abstract_test_value(bool expected) {
  constexpr bool actual = xstl::is_abstract_v<T>;

#if TEST_WITH_STATIC_ASSERT
  NOYX_ASSERT_TRUE_MESSAGE(
    NOYX_EVAL((actual == expected)),
    "is_abstract<T> returned incorrect value"
  );
#else
  NOYX_ASSERT_TRUE_MESSAGE(
    NOYX_EVAL((actual == expected)),
    "is_abstract<T> returned incorrect value: got different from expected"
  );
#endif
}

struct AbstractBase {
  virtual void foo() = 0;
};

struct ConcreteDerived : AbstractBase {
  void foo() override {}
};

struct NotAbstract {
  void foo() {}
};

struct TestTypeInvokerIsAbstract {
  constexpr void operator()() const {
    tt_is_abstract_test_value<AbstractBase>(true);

    tt_is_abstract_test_value<ConcreteDerived>(false);
    tt_is_abstract_test_value<NotAbstract>(false);
    tt_is_abstract_test_value<int>(false);
    tt_is_abstract_test_value<int[3]>(false);
    tt_is_abstract_test_value<void()>(false);
  }
};

NOYX_TEST(IsAbstract, UnitTest) {
  TestTypeInvokerIsAbstract{}();
}
