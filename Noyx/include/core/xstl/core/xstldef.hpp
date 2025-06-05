#pragma once

namespace xstl {
#if defined(NOYX_PLATFORM_WINDOWS)//64-bit windows
  using size_t = unsigned long long;
  using ptrdiff_t = decltype(static_cast<int*>(nullptr) - static_cast<int*>(nullptr));
  using nullptr_t = decltype(nullptr);
  
  using int8_t = char;
  using int16_t = short int;
  using int32_t = int;
  using int64_t = long long;
 
  using uint8_t = unsigned char;
  using uint16_t = unsigned short int;
  using uint32_t = unsigned int;
  using uint64_t = unsigned long long;
  
  using max_align_t = double;
  using intmax_t = long long;  
  using uintmax_t = unsigned long long;

#endif
}
