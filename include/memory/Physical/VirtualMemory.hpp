/* SPDX-License-Identifier: Apache-2.0 */

/**
 * @file     VirtualMemory.hpp
 * @brief
 *
 * @date     15.02.2026
 *
 * @copyright Copyright (c) 2026 The Project Contributors
 */

#pragma once
#include <cstddef>
#include <platform/os/os_detect.h>

namespace OS {
  bool  Commit(void* addr, std::size_t bytes);
  void  Decommit(void* addr, std::size_t bytes);
  void* Reserve(std::size_t bytes);
  void  Release(void* addr, std::size_t bytes);
}

#if defined(NOYX_WINDOWS)
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

namespace OS {
  inline bool Commit(void* addr, std::size_t bytes) {
    return VirtualAlloc(addr, bytes, MEM_COMMIT, PAGE_READWRITE) != nullptr;
  }
  inline void Decommit(void* addr, std::size_t bytes) {
    VirtualFree(addr, bytes, MEM_DECOMMIT);
  }
  inline void* Reserve(std::size_t bytes) {
    return VirtualAlloc(nullptr, bytes, MEM_RESERVE, PAGE_NOACCESS);
  }
  inline void Release(void* addr, std::size_t bytes) {
    (void)bytes; // unused on Windows
    VirtualFree(addr, 0, MEM_RELEASE);
  }
  inline std::size_t pageSize() {
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    return si.dwPageSize;
  }
}

#elif defined(NOYX_LINUX) || defined(NOYX_MACOS)
#include <sys/mman.h>
#include <unistd.h>
namespace OS {
  inline bool Commit(void* addr, std::size_t bytes) {
    return mprotect(addr, bytes, PROT_READ | PROT_WRITE) == 0;
  }
  inline void Decommit(void* addr, std::size_t bytes) {
    madvise(addr, bytes, MADV_FREE);
    mprotect(addr, bytes, PROT_NONE);
  }
  inline void* Reserve(std::size_t bytes) {
    void* ptr = mmap(nullptr, bytes, PROT_NONE,
                     MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    return ptr == MAP_FAILED ? nullptr : ptr;
  }

  inline void Release(void* addr, std::size_t bytes) {
    munmap(addr, bytes);
  }
  inline std::size_t pageSize() {
    return sysconf(_SC_PAGESIZE);
  }
}

#else
#error "NOYX: VirtualMemory not implemented for this platform"
#endif