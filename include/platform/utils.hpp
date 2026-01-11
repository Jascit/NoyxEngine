/* SPDX-License-Identifier: Apache-2.0 */
/* 
 * \file   util.hpp
 * \brief  Small utilities: pause, likely/unlikely, location macro
 *
 * Copyright (c) 2026 Project Contributors
 * \author Jascit<https://github.com/Jascit>
 * \date   07.01.2026
 */

#pragma once
#include <platform/arch.hpp>

#define NOYX_CORE_LOCATION __FILE__, __LINE__

#if defined(NOYX_CORE_ARCH_X86_64)
#  include <immintrin.h>
#  define NOYX_CORE_PAUSE() _mm_pause()
#elif defined(NOYX_CORE_ARCH_ARM64)
#  include <utility>
static inline void NOYX_CORE_PAUSE() { __asm__ volatile("yield" ::: "memory"); }
#else
#  define NOYX_CORE_PAUSE() ((void)0)
#endif

#if defined(__GNUC__) || defined(__clang__)
#  define NOYX_CORE_LIKELY(x)   __builtin_expect(!!(x), 1)
#  define NOYX_CORE_UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
#  define NOYX_CORE_LIKELY(x)   (x)
#  define NOYX_CORE_UNLIKELY(x) (x)
#endif