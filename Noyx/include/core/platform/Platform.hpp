#pragma once

// --- Platform detection (CMake OR compiler macros) ---

// Windows
#if (defined(NOYX_PLATFORM_WINDOWS) || defined(_WIN32) || defined(_WIN64)) && !defined(PLATFORM_WINDOWS)
#define PLATFORM_WINDOWS
#endif

// Linux/Unix
#if (defined(NOYX_PLATFORM_LINUX) || defined(__linux__)) && !defined(PLATFORM_LINUX)
#define PLATFORM_LINUX
#endif

#if (defined(NOYX_PLATFORM_UNIX) || defined(__unix__)) && !defined(PLATFORM_UNIX)
#define PLATFORM_UNIX
#endif

// Microsoft-specific
#if defined(NOYX_PLATFORM_MICROSOFT) && !defined(PLATFORM_MICROSOFT)
#define PLATFORM_MICROSOFT
#elif defined(_MSC_VER) && !defined(PLATFORM_MICROSOFT)
#define PLATFORM_MICROSOFT
#endif

// Apple platforms
#if (defined(NOYX_PLATFORM_APPLE) || defined(__APPLE__)) && !defined(PLATFORM_APPLE)
#define PLATFORM_APPLE
#endif

#if (defined(NOYX_PLATFORM_MAC) || (defined(__APPLE__) && defined(TARGET_OS_MAC) && TARGET_OS_MAC)) && !defined(PLATFORM_MAC)
#define PLATFORM_MAC
#endif

#if (defined(NOYX_PLATFORM_IOS) || (defined(__APPLE__) && defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE)) && !defined(PLATFORM_IOS)
#define PLATFORM_IOS
#endif

#if (defined(NOYX_PLATFORM_TVOS)) && !defined(PLATFORM_TVOS)
#define PLATFORM_TVOS
#endif

#if (defined(NOYX_PLATFORM_VISIONOS)) && !defined(PLATFORM_VISIONOS)
#define PLATFORM_VISIONOS
#endif

// Android
#if (defined(NOYX_PLATFORM_ANDROID) || defined(__ANDROID__)) && !defined(PLATFORM_ANDROID)
#define PLATFORM_ANDROID
#endif

// Nintendo Switch
#if defined(NOYX_PLATFORM_SWITCH) && !defined(PLATFORM_SWITCH)
#define PLATFORM_SWITCH
#endif

// FreeBSD
#if (defined(NOYX_PLATFORM_FREEBSD) || defined(__FreeBSD__)) && !defined(PLATFORM_FREEBSD)
#define PLATFORM_FREEBSD
#endif

// --- CPU / Architecture detection ---
#if (defined(NOYX_ARCH_X64) || defined(_M_X64) || defined(__x86_64__)) && !defined(PLATFORM_CPU_X86_FAMILY)
#define PLATFORM_CPU_X86_64_FAMILY
#endif

#if ((defined(NOYX_ARCH_X86) || defined(_M_IX86) || defined(__i386__)))
#define PLATFORM_CPU_X86_FAMILY
#endif

#if (defined(NOYX_ARCH_ARM64) || defined(_M_ARM64) || defined(__aarch64__)) && !defined(PLATFORM_CPU_ARM_FAMILY)
#define PLATFORM_CPU_ARM_FAMILY
#endif

#if (defined(NOYX_ARCH_ARM) || defined(_M_ARM) || defined(__arm__)) && !defined(PLATFORM_CPU_ARM_FAMILY)
#define PLATFORM_CPU_ARM_FAMILY
#endif

#if defined(_MSC_VER)
#  define FORCE_INLINE __forceinline
#elif defined(__GNUC__) || defined(__clang__)
#  define FORCE_INLINE inline __attribute__((always_inline))
#else
#  define FORCE_INLINE inline
#endif


#pragma once

#if defined(_WIN64)
  // Windows 64-bit
#include <windows.h>
#define WINDOWS
#elif defined(__APPLE__) && defined(__MACH__)
#include <TargetConditionals.h>
#if TARGET_OS_IPHONE && TARGET_IPHONE_SIMULATOR
// iOS Simulator
#elif TARGET_OS_IPHONE
// iOS device
#elif TARGET_OS_MAC
#define macOS
#else
// other Apple platform
#endif

#elif defined(__linux__)
#define LINUX
#else
#error "Unknown platform"
#endif

#if defined(__x86_64__) || defined(_M_X64) || defined(_M_AMD64)
#define X64
#elif defined(__aarch64__) || defined(_M_ARM64)
#define X64_ARM
#else
#error "unsupported architecture"
#endif
#pragma once

#include <cstdlib>
#include <cstdio>

#define LOCATION_FILE_LINE __FILE__, __LINE__

#if defined(_MSC_VER) && defined(_DEBUG)
#include <intrin.h>
#define DEBUG_BREAK() __debugbreak()
#elif defined(_DEBUG) || defined(DEBUG)
#include <csignal>
#define DEBUG_BREAK() std::raise(SIGTRAP)
#else
#define DEBUG_BREAK() ((void)0)
#endif


#if defined(_MSC_VER)
#include <csignal>
#define ABORT_IMMEDIATELY() __fastfail(FAST_FAIL_INVALID_ARG)
#elif defined(LINUX) && !defined(__ANDROID__)
#include <unistd.h>
#include <sys/syscall.h>
static inline void linux_abort_with_core() {
  pid_t pid = getpid();
  pid_t tid = (pid_t)syscall(SYS_gettid);
  syscall(SYS_tgkill, pid, tid, SIGABRT);
  std::abort();
}
#define ABORT_IMMEDIATELY() linux_abort_with_core()
#elif defined(__ANDROID__)
#include <csignal>
#define ABORT_IMMEDIATELY() std::raise(SIGABRT)
#else
#include <csignal>
#define ABORT_IMMEDIATELY() std::abort()
#endif


#if defined(_MSC_VER) && defined(_DEBUG)
#include <crtdbg.h>

#define report_debug(level, file, line, fmt, ...) \
      _CrtDbgReport(level, file, line, fmt, __VA_ARGS__)
#else
#include <cstdio>
#define report_debug(level, file, line, fmt, ...) do { \
      std::fprintf(stderr, "%s(%d): " fmt "\n", file, line, ##__VA_ARGS__); \
  } while (0)
#endif


#define ASSERT_ABORT(cond, msg) do {                                    \
    if (!(cond)) {                                                      \
        report_debug(2, LOCATION_FILE_LINE, "%s", (msg));               \
        DEBUG_BREAK();                                                  \
        ABORT_IMMEDIATELY();                                            \
    }                                                                   \
} while (0)

#if defined(_MSC_VER)
#  define FORCE_INLINE __forceinline
#elif defined(__GNUC__) || defined(__clang__)
#  define FORCE_INLINE inline __attribute__((always_inline))
#else
#  define FORCE_INLINE inline
#endif

#define CONSTEXPR constexpr
#define NODISCARD [[nodiscard]]
