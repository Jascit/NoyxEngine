/* SPDX-License-Identifier: Apache-2.0 */

/**
 * @file     util.hpp
 * @brief    Small utilities: pause, likely/unlikely, location macro
 *
 * @author   Jascit (https://github.com/Jascit)
 * @date     07.01.2026
 *
 * @copyright Copyright (c) 2026 The Project Contributors
 */

#ifndef NOYX_UTILS_H
#define NOYX_UTILS_H
#include <platform/arch.h>

#define NOYX_LOCATION __FILE__, __LINE__

#if defined(NOYX_ARCH_X86_64)
#  include <immintrin.h>
#  define NOYX_PAUSE() _mm_pause()
#elif defined(NOYX_ARCH_ARM64)
#  include <utility>
static inline void NOYX_PAUSE() { __asm__ volatile("yield" ::: "memory"); }
#else
#  define NOYX_PAUSE() ((void)0)
#endif

#if defined(__GNUC__) || defined(__clang__)
#  define NOYX_LIKELY(x)   __builtin_expect(!!(x), 1)
#  define NOYX_UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
#  define NOYX_LIKELY(x)   (x)
#  define NOYX_UNLIKELY(x) (x)
#endif
#endif
