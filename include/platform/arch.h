/* SPDX-License-Identifier: Apache-2.0 */

/**
 * @file     arch.h
 * @brief    Architecture detection (prefix: NOYX_ARCH_*)
 *
 * @date     07.01.2026
 *
 * @copyright Copyright (c) 2026 The Project Contributors
 */

#ifndef NOYX_ARCH_H
#define NOYX_ARCH_H

#if defined(__x86_64__) || defined(_M_X64) || defined(_M_AMD64) || defined(ARCH_X64)
#define NOYX_ARCH_X86_64 1
#elif defined(__aarch64__) || defined(_M_ARM64) || defined(ARCH_ARM64)
#define NOYX_ARCH_ARM64 1
#else
#error "NOYX : Unsupported architecture"
#endif
#endif