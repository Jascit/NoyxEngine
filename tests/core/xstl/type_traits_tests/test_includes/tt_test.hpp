#pragma once

#include <iostream>
#include <is/is_same.hpp>

namespace xstl_test
{
  inline unsigned int failure_count = 0;

#ifdef TEST_WITH_ASSERT

  template <typename A, typename B>
  constexpr void check_different_types()
  {
    static_assert(
      !xstl::is_same<A, B>::value,
      "Type mismatch: expected two different types"
      );
  }

#define VALIDATE_TYPES(A, B)  xstl_test::check_different_types<A, B>()

#else

  template <typename A, typename B>
  constexpr void check_different_types(const char* file, int line)
  {
    if (xstl::is_same<A, B>::value)
    {
      std::cerr
        << file << ":" << line
        << ": Type mismatch: expected different types, got "
        << typeid(A).name() << " and " << typeid(B).name()
        << std::endl;
      ++failure_count;
    }
  }

#define VALIDATE_TYPES(A, B)  xstl_test::check_different_types<A, B>(__FILE__, __LINE__)
#define VALIDATE_TYPES_TYPE_2(A, B, file, line)  xstl_test::check_different_types<A, B>(file, line)

#endif  // TEST_WITH_ASSERT

} // namespace xstl_test
