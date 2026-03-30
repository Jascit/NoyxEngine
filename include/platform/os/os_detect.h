/* SPDX-License-Identifier: Apache-2.0 */

/**
 * @file     os_detect.h
 * @brief    OS detection and small OS-specific helpers
 *
 * @author   Jascit (https://github.com/Jascit)
 * @date     07.01.2026
 *
 * @copyright Copyright (c) 2026 The Project Contributors
 */

#ifndef NOYX_OS_DETECT_H
#define NOYX_OS_DETECT_H

#if defined(_WIN64) || defined(_WIN32) || defined(WINDOWS)
#  ifndef NOYX_WINDOWS
#    define NOYX_WINDOWS 1
#  endif
#elif defined(__APPLE__) && defined(__MACH__)
#define NOYX_APPLE 1
#  include <TargetConditionals.h>
#  if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
#    define NOYX_IOS 1
#  elif defined(TARGET_OS_MAC) && TARGET_OS_MAC
#    define NOYX_MACOS 1
#  else
#    define NOYX_APPLE 1
#  endif
#elif defined(__ANDROID__)
#  define NOYX_ANDROID 1
#elif defined(__linux__) || defined(__gnu_linux__) || defined(LINUX)
#  define NOYX_LINUX 1
#else
#  error "NOYX: Unknown platform"
#endif

#endif
