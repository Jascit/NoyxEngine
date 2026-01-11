/* SPDX-License-Identifier: Apache-2.0 */
/* 
 * \file   assert.hpp
 * \brief  ABORT & ASSERT helpers.
 * 
 * Copyright (c) 2026 Project Contributors
 * \author Jascit<https://github.com/NOYX_CORE>
 * \date   07.01.2026
 * \note   
 */
#pragma once

#include <platform/os.hpp>
#include <platform/debug.hpp>
#include <platform/utils.hpp>
#include <cstdlib>
#include <csignal>
#include <cstdio>

#if defined(_MSC_VER)
#  include <intrin.h>
#endif

// Debug break
#if defined(_MSC_VER) && defined(NOYX_CORE_DEBUG)
#  define NOYX_CORE_DEBUG_BREAK() __debugbreak()
#elif defined(NOYX_CORE_LINUX) || defined(NOYX_CORE_ANDROID) || defined(NOYX_CORE_MACOS)
#  include <signal.h>
#  define NOYX_CORE_DEBUG_BREAK() std::raise(SIGTRAP)
#else
#  define NOYX_CORE_DEBUG_BREAK() ((void)0)
#endif

// Abort with core when possible
#if defined(_MSC_VER)
static inline void noyx_core_abort_immediately() { __fastfail(5); }
#elif defined(NOYX_CORE_LINUX) && !defined(NOYX_CORE_ANDROID)
#  include <unistd.h>
#  include <sys/syscall.h>
static inline void noyx_core_abort_immediately() {
  pid_t pid = getpid();
  pid_t tid = (pid_t)syscall(SYS_gettid);
  syscall(SYS_tgkill, pid, tid, SIGABRT);
  std::abort();
}
#else
static inline void noyx_core_abort_immediately() {
  std::abort();
}
#endif
#define NOYX_CORE_ABORT_IMMEDIATELY() noyx_core_abort_immediately()

// Simple reporter (MSVC CRT or fallback fprintf)
#if defined(_MSC_VER) && defined(_DEBUG)
#  include <crtdbg.h>
#  define NOYX_CORE_REPORT_DEBUG(level, file, line, fmt, ...) \
       _CrtDbgReport(level, file, line, fmt, __VA_ARGS__)
#else
#  define NOYX_CORE_REPORT_DEBUG(level, file, line, fmt, ...) do { \
      std::fprintf(stderr, "%s(%d): " fmt "\n", file, line, ##__VA_ARGS__); \
  } while (0)
#endif

// Assert macro
#define NOYX_CORE_ASSERT_ABORT(cond, msg) do {                     \
    if (NOYX_CORE_UNLIKELY(!(cond))) {                             \
        NOYX_CORE_REPORT_DEBUG(2, __FILE__, __LINE__, "%s", (msg));\
        NOYX_CORE_DEBUG_BREAK();                                   \
        NOYX_CORE_ABORT_IMMEDIATELY();                             \
    }                                                           \
} while (0)
