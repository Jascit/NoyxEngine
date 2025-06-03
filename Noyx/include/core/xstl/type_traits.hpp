template<typename integral>
struct is_integral {
  static constexpr bool value = false;
};

struct is_integral<char8_t> {
  static constexpr bool value = true;
};

struct is_integral<char16_t> {
  static constexpr bool value = true;
};

struct is_integral<char32_t> {
  static constexpr bool value = true;
};

struct is_integral<wchar_t> {
  static constexpr bool value = true;
};

struct is_integral<signed char> {
  static constexpr bool value = true;
};

struct is_integral<unsigned char> {
  static constexpr bool value = true;
};

struct is_integral<short> {
  static constexpr bool value = true;
};

struct is_integral<unsigned short> {
  static constexpr bool value = true;
};

struct is_integral<int> {
  static constexpr bool value = true;
};

struct is_integral<unsigned int> {
  static constexpr bool value = true;
};

struct is_integral<long> {
  static constexpr bool value = true;
};

struct is_integral<unsigned long > {
  static constexpr bool value = true;
};

struct is_integral<long long> {
  static constexpr bool value = true;
};

struct is_integral<unsigned long long> {
  static constexpr bool value = true;
};

#if defined(_WIN32)
#define NOYX_PLATFORM_WINDOWS
#elif defined(__APPLE__) && defined(__MACH__)
#include <TargetConditionals.h>
#if TARGET_OS_IPHONE
#define NOYX_PLATFORM_IOS
#else
#define NOYX_PLATFORM_MACOS
#endif
#elif defined(__ANDROID__)
#define NOYX_PLATFORM_ANDROID
#elif defined(__linux__)
#define NOYX_PLATFORM_LINUX
#elif defined(__unix__)
#define NOYX_PLATFORM_UNIX
#else
#error "Невідома платформа!"
#endif
