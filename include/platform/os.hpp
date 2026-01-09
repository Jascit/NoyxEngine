/* SPDX-License-Identifier: Apache-2.0 */
/* 
 * \file   os.hpp
 * \brief  OS detection and small OS-specific helpers
 *
 * Copyright (c) 2026 Jascit
 * \author Jascit<https://github.com/Jascit>
 * \date   07.01.2026
 */

#pragma once

#if defined(_WIN64) || defined(_WIN32) || defined(WINDOWS)
#  ifndef NOYX_CORE_WINDOWS
#    define NOYX_CORE_WINDOWS 1
#  endif
#elif defined(__APPLE__) && defined(__MACH__)
#  include <TargetConditionals.h>
#  if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
#    define NOYX_CORE_IOS 1
#  elif defined(TARGET_OS_MAC) && TARGET_OS_MAC
#    define NOYX_CORE_MACOS 1
#  else
#    define NOYX_CORE_APPLE 1
#  endif
#elif defined(__ANDROID__)
#  define NOYX_CORE_ANDROID 1
#elif defined(__linux__) || defined(__gnu_linux__) || defined(LINUX)
#  define NOYX_CORE_LINUX 1
#else
#  error "NOYX_CORE: Unknown platform"
#endif
