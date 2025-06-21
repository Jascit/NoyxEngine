#include <tt_test_detail.hpp>
#include <type_traits/is_trivially_move_constructible.hpp>

template<typename T, bool Expected>
constexpr void tt_is_trivially_move_constructible_test_value() {
  constexpr bool actual = std::is_trivially_move_constructible_v<T>;
#if TEST_WITH_STATIC_ASSERT
  NOYX_ASSERT_TRUE_MESSAGE(
    actual == Expected,
    "is_trivially_move_constructible<" #T "> returned incorrect value"
  );
#else
  NOYX_ASSERT_TRUE_MESSAGE(
    actual == Expected,
    "is_trivially_move_constructible<" << typeid(T).name() << "> returned incorrect: "
    "actual = " << actual << ", expected = " << Expected
  );
#endif
}


struct TestTypeInvokerIsTriviallyMoveConstructible {
  void operator()() const {
    struct Trivial {
      Trivial(Trivial&&) = default;
    };

    struct NonTrivial {
      NonTrivial(NonTrivial&&) noexcept {}
    };

    struct DeletedMove {
      DeletedMove(DeletedMove&&) = delete;
    };

    tt_is_trivially_move_constructible_test_value<int, true>();
    tt_is_trivially_move_constructible_test_value<const int, true>();
    tt_is_trivially_move_constructible_test_value<int&, true>();
    tt_is_trivially_move_constructible_test_value<int&&, true>();
    tt_is_trivially_move_constructible_test_value<int[3], false>();
    tt_is_trivially_move_constructible_test_value<int[], false>();
    tt_is_trivially_move_constructible_test_value<int*, true>();

    tt_is_trivially_move_constructible_test_value<Trivial, true>();
    tt_is_trivially_move_constructible_test_value<NonTrivial, false>();
    tt_is_trivially_move_constructible_test_value<DeletedMove, false>();
  }
};

NOYX_TEST(IsTriviallyMoveConstructible, UnitTest) {
  TestTypeInvokerIsTriviallyMoveConstructible{}();
}
