/* SPDX-License-Identifier: Apache-2.0 */
/* 
 * \file   arch.hpp
 * \brief  Architecture detection (prefix: NOYX_CORE_ARCH_*)
 * 
 * Copyright (c) 2026 Jascit 
 * \author Jascit<https://github.com/Jascit>
 * \date   07.01.2026
 */
#pragma once
 
#if defined(__x86_64__) || defined(_M_X64) || defined(_M_AMD64) || defined(ARCH_X64)
#  define NOYX_CORE_ARCH_X86_64 1
#elif defined(__aarch64__) || defined(_M_ARM64) || defined(ARCH_ARM64)
#  define NOYX_CORE_ARCH_ARM64 1
#else
#  error "NOYX_CORE : Unsupported architecture"
#endif