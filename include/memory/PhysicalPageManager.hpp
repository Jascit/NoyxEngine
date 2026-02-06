/* SPDX-License-Identifier: Apache-2.0 /
/*
 * \file   PhysicalPageManager.hpp
 * \brief
 *
 * Copyright (c) 2026 Project Contributors
 * \author MaksymRbkh <https://github.com/MaksymRbkh>
 * \date   06.02.2026
 */

#include <cstddef>
#include <sys/mman.h>

#if defined(_WIN32)
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace noyxcore::memory {
  class PhysicalPageManager {
  public:
    static std::size_t GetPageSize() {
#ifdef _WIN32
      SYSTEM_INFO si;
      GetSystemInfo(&si);
      return si.dwPageSize;
#else
      return static_cast<std::size_t>(sysconf(_SC_PAGESIZE));
#endif
    }

    static void* Reserve(std::size_t size) {
#ifdef _WIN32
      void* ptr = VirtualAlloc(nullptr, size, MEM_RESERVE, PAGE_NOACCESS);
#else
      void* ptr = mmap(nullptr, size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
      if (ptr == MAP_FAILED) return nullptr;
#endif
      return ptr;
    }

    static bool Commit(void* ptr, std::size_t size) {
#ifdef _WIN32
      void* result = VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE);
      return result != nullptr;
#else
      int result = mprotect(ptr, size, PROT_READ | PROT_WRITE);
      return result == 0;
#endif
    }

    static bool Decommit(void* ptr, std::size_t size) {
#ifdef _WIN32
      return VirtualFree(ptr, size, MEM_DECOMMIT) != 0;
#else
      int result = madvise(ptr, size, MADV_DONTNEED);
      mprotect(ptr, size, PROT_NONE);
      return result == 0;
#endif
    }

    static bool Release(void* ptr, std::size_t size) {
#ifdef _WIN32
      return VirtualFree(ptr, 0, MEM_RELEASE) != 0;
#else
      return munmap(ptr, size) == 0;
#endif
    }
  };
}
