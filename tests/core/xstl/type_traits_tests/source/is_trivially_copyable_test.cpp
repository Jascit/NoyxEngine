#include <type_traits/is_trivially_copyable.hpp>
#include <tt_test_detail.hpp>
#include <type_traits.hpp>

template<typename T>
constexpr void tt_is_trivially_copyable_test_value(bool expected) {
  constexpr bool actual = xstl::is_trivially_copyable<T>::value;

#if TEST_WITH_STATIC_ASSERT
  NOYX_ASSERT_TRUE_MESSAGE(
    actual == expected,
    "is_trivially_copyable<T> returned incorrect value"
  );
#else
  NOYX_ASSERT_TRUE_MESSAGE(
    actual == expected,
    "is_trivially_copyable<T> returned incorrect value: actual = " << actual
    << ", expected = " << expected
    << ", T = " << typeid(T).name()
  );
#endif
}

struct TestTypeInvokerIsTriviallyCopyable {
  constexpr void operator()() const {
    struct TriviallyCopyable {
      int x;
      double y;
    };
    struct NonTriviallyCopyableCtor {
      NonTriviallyCopyableCtor() {}
      NonTriviallyCopyableCtor(const NonTriviallyCopyableCtor&) {}
    };
    struct NonTriviallyCopyableDtor {
      ~NonTriviallyCopyableDtor() {}
    };
    struct WithVirtual {
      virtual void foo() {}
    };

    tt_is_trivially_copyable_test_value<int>(true);
    tt_is_trivially_copyable_test_value<double>(true);
    tt_is_trivially_copyable_test_value<TriviallyCopyable>(true);

    tt_is_trivially_copyable_test_value<NonTriviallyCopyableCtor>(false);
    tt_is_trivially_copyable_test_value<NonTriviallyCopyableDtor>(false);
    tt_is_trivially_copyable_test_value<WithVirtual>(false);

    tt_is_trivially_copyable_test_value<int[5]>(true);
  }
};

NOYX_TEST(IsTriviallyCopyable, UnitTest) {
  TestTypeInvokerIsTriviallyCopyable{}();
}
