#include <type_traits/is_trivially_copyable.hpp>
#include <tt_test_detail.hpp>
#include <type_traits.hpp>

template<typename T>
constexpr void tt_is_trivially_copyable_test_value(bool expected) {
  constexpr bool actual = xstl::is_trivially_copyable<T>::value;

  NOYX_ASSERT_TRUE_MESSAGE(
    actual == expected,
    "is_trivially_copyable<T> returned incorrect value"
  );
}

struct IsTriviallyCopyableUnitTest_TriviallyCopyable {
  int x;
  double y;
};

struct IsTriviallyCopyableUnitTest_NonTriviallyCopyableCtor {
  IsTriviallyCopyableUnitTest_NonTriviallyCopyableCtor() {}
  IsTriviallyCopyableUnitTest_NonTriviallyCopyableCtor(const IsTriviallyCopyableUnitTest_NonTriviallyCopyableCtor&) {}
};

struct IsTriviallyCopyableUnitTest_NonTriviallyCopyableDtor {
  ~IsTriviallyCopyableUnitTest_NonTriviallyCopyableDtor() {}
};

struct IsTriviallyCopyableUnitTest_WithVirtual {
  virtual void foo() {}
};

struct TestTypeInvokerIsTriviallyCopyable {
  constexpr void operator()() const {
    tt_is_trivially_copyable_test_value<int>(true);
    tt_is_trivially_copyable_test_value<double>(true);
    tt_is_trivially_copyable_test_value<IsTriviallyCopyableUnitTest_TriviallyCopyable>(true);

    tt_is_trivially_copyable_test_value<IsTriviallyCopyableUnitTest_NonTriviallyCopyableCtor>(false);
    tt_is_trivially_copyable_test_value<IsTriviallyCopyableUnitTest_NonTriviallyCopyableDtor>(false);
    tt_is_trivially_copyable_test_value<IsTriviallyCopyableUnitTest_WithVirtual>(false);

    tt_is_trivially_copyable_test_value<int[5]>(true);
  }
};

NOYX_TEST(IsTriviallyCopyable, UnitTest) {
  TestTypeInvokerIsTriviallyCopyable{}();
}
