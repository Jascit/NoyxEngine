//
// Created by Maksym Riabykh on 15.02.2026.
//

#include <cstddef>
#include <platform/os/os_detect.h>

namespace OS {
  bool  Commit(void* addr, std::size_t bytes);
  void  Decommit(void* addr, std::size_t bytes);
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
}

#elif defined(NOYX_LINUX) || defined(NOYX_MACOS)
#include <sys/mman.h>
namespace OS {
  inline bool Commit(void* addr, std::size_t bytes) {
    return mprotect(addr, bytes, PROT_READ | PROT_WRITE) == 0;
  }
  inline void Decommit(void* addr, std::size_t bytes) {
    madvise(addr, bytes, MADV_FREE);
    mprotect(addr, bytes, PROT_NONE);
  }
}

#else
#error "NOYX: VirtualMemory not implemented for this platform"
#endif