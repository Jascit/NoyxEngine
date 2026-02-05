/* SPDX-License-Identifier: Apache-2.0 */
/* 
 * \file   assert.hpp
 * \brief  ABORT & ASSERT helpers.
 * 
 * Copyright (c) 2026 Project Contributors
 * \author Jascit<https://github.com/NOYX>
 * \date   07.01.2026
 * \note   
 */

#pragma once

#include "os/os_detect.h"
#include <platform/debug.h>
#include <platform/utils.hpp>
#include <cstdlib>
#include <csignal>
#include <cstdio>

#if defined(_MSC_VER)
#  include <intrin.h>
#endif

// Debug break
#if defined(_MSC_VER) && defined(NOYX_DEBUG)
#  define NOYX_DEBUG_BREAK() __debugbreak()
#elif defined(NOYX_LINUX) || defined(NOYX_ANDROID) || defined(NOYX_MACOS) && defined(NOYX_DEBUG)
#  include <signal.h>
#  define NOYX_DEBUG_BREAK() std::raise(SIGTRAP)
#else
#  define NOYX_DEBUG_BREAK() ((void)0)
#endif

// Abort with core when possible
#if defined(_MSC_VER)
static inline void NOYX_abort_immediately() { __fastfail(5); }
#elif defined(NOYX_LINUX) && !defined(NOYX_ANDROID)
#  include <unistd.h>
#  include <sys/syscall.h>
static inline void NOYX_abort_immediately() {
  pid_t pid = getpid();
  pid_t tid = (pid_t)syscall(SYS_gettid);
  syscall(SYS_tgkill, pid, tid, SIGABRT);
  std::abort();
}
#else
static inline void NOYX_abort_immediately() {
  std::abort();
}
#endif
#define NOYX_ABORT_IMMEDIATELY() NOYX_abort_immediately()

// Simple reporter (MSVC CRT or fallback fprintf)
#if defined(_MSC_VER) && defined(_DEBUG)
#  include <crtdbg.h>
#  define NOYX_REPORT_DEBUG(level, file, line, fmt, ...) \
       _CrtDbgReport(level, file, line, fmt, __VA_ARGS__)
#else
#  define NOYX_REPORT_DEBUG(level, file, line, fmt, ...) do { \
      std::fprintf(stderr, "%s(%d): " fmt "\n", file, line, ##__VA_ARGS__); \
  } while (0)
#endif

// Assert macro
#define NOYX_ASSERT_ABORT(cond, msg) do {                     \
    if (NOYX_UNLIKELY(!(cond))) {                             \
        NOYX_REPORT_DEBUG(2, __FILE__, __LINE__, "%s", (msg));\
        NOYX_DEBUG_BREAK();                                   \
        NOYX_ABORT_IMMEDIATELY();                             \
    }                                                           \
} while (0)

