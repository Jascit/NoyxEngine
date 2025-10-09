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
