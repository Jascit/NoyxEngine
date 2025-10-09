#pragma once
#include "Platform.hpp"

namespace NOYX {
  namespace TYPES {
#if defined(PLATFORM_WINDOWS)
    using size_t = unsigned int;
    using ptrdiff = decltype(static_cast<int*>(nullptr) - static_cast<int*>(nullptr));
    using nullptr_t = decltype(nullptr);

    using int8 = char;
    using int16 = short int;
    using int32 = int;
    using int64 = long long;

    using uint8 = unsigned char;
    using uint16 = unsigned short int;
    using uint32 = unsigned int;
    using uint64 = unsigned long long;

    using max_align = double;
    using intmax = long long;
    using uintmax = unsigned long long;
#else
#endif
  } // xstl
}
