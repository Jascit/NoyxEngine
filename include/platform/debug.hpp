/* SPDX-License-Identifier: Apache-2.0 */
/* 
 * \file   debug.hpp
 * \brief  Debug/Release detection and common attributes
 * 
 * Copyright (c) 2026 Project Contributors
 * \author Jascit<https://github.com/Jascit>
 * \date   07.01.2026
 */

#pragma once

#if defined(FORCE_DISABLE_DEBUG)
#  undef DEBUG
#  undef NOYX_CORE_DEBUG
#elif defined(FORCE_ENABLE_DEBUG)
#  ifndef DEBUG
#    define DEBUG
#  endif
#  ifndef NOYX_CORE_DEBUG
#    define NOYX_CORE_DEBUG 1
#  endif
#endif

#if !defined(NDEBUG) && !defined(DEBUG) && !defined(FORCE_DISABLE_DEBUG)
#  define NOYX_CORE_DEBUG 1
#endif


#if defined(_MSC_VER)
#  define FORCE_INLINE __forceinline
#elif defined(__GNUC__) || defined(__clang__)
#  define FORCE_INLINE inline __attribute__((always_inline))
#else
#  define FORCE_INLINE inline
#endif
