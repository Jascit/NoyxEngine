#include <tt_test_detail.hpp>
#include <type_traits/is_constant_evaluated.hpp>

struct TestTypeInvokerIsConstantEvaluated {
  static constexpr void foo() {
    static_assert(xstl::is_constant_evaluated(), "must be true in constexpr");
  }
  void operator()() const {
    NOYX_ASSERT_TRUE_MESSAGE(
      xstl::is_constant_evaluated() == false,
      "is_constant_evaluated() should return false at runtime"
    );
  }
};

NOYX_TEST(IsConstantEvaluated, UnitTest) {
  TestTypeInvokerIsConstantEvaluated{}();
  TestTypeInvokerIsConstantEvaluated::foo();
}
