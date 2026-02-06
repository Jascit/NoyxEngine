/* SPDX-License-Identifier: Apache-2.0 /
/*
 * \file   PhysicalPageManager.hpp
 * \brief
 *
 * Copyright (c) 2026 Project Contributors
 * \author MaksymRbkh <https://github.com/MaksymRbkh>
 * \date   06.02.2026
 */

#include <algorithm>
#include <cstddef>
#include <vector>
#if defined(_WIN32)
#include <windows.h>
#else
#include <sys/mman.h>
#include <unistd.h>
#endif

namespace noyxcore::memory {
  class PhysicalPageManager {
  public:
    static size_t PageSize() {
      static size_t page_size = [] {
#ifdef _WIN32
        SYSTEM_INFO si;
        GetSystemInfo(&si);
        return static_cast<std::size_t>(si.dwPageSize);
#else
        return static_cast<std::size_t>(sysconf(_SC_PAGESIZE));
#endif
      }();
      return page_size;
    }

    static void* ReservePages(size_t pageCount) {
      if (pageCount == 0) return nullptr;
      std::size_t sizeInBytes = pageCount * PageSize();
#ifdef _WIN32
      auto result = VirtualAlloc(nullptr, sizeInBytes, MEM_RESERVE, PAGE_NOACCESS);
      return reinterpret_cast<void*>(result);
#else
      auto result = mmap(nullptr, sizeInBytes, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
      if (result == MAP_FAILED) return nullptr;
      return reinterpret_cast<void*>(result);
#endif
    }

    static void ReleasePages(void* base, size_t pageCount) {
      if (pageCount == 0 || base == nullptr) return;
      std::size_t sizeInBytes = pageCount * PageSize();
#ifdef _WIN32
      VirtualFree(base, 0, MEM_RELEASE);
#else
      munmap(base, sizeInBytes);
#endif
    }

      bool CommitPages(void* base, size_t pageCount) {
      if (pageCount == 0 || base == nullptr) return false;
      std::size_t sizeInBytes = pageCount * PageSize();
#ifdef _WIN32
      void* ptr = VirtualAlloc(base, sizeInBytes, MEM_COMMIT, PAGE_READWRITE);
      if (!ptr) return false;
#else
      int result = mprotect(base, sizeInBytes, PROT_READ | PROT_WRITE);
      if (result != 0) return false;
#endif
      committedRanges.push_back({base, pageCount});
      return true;
    }

      bool DecommitPages(void* base, size_t pageCount) {
      if (pageCount == 0 || base == nullptr) return false;
      std::size_t sizeInBytes = pageCount * PageSize();

      //Trying to find base
      Range* target = nullptr;
      for (auto& i : committedRanges) {
        if (i.base == base) {
          target = &i;
          break;
        }
      }
      if (target == nullptr) return false;
      std::size_t pagesToDecommit = std::min(pageCount, target->pageCount);
      std::size_t bytesToDecommit = pagesToDecommit * PageSize();
#ifdef _WIN32
      if (!VirtualFree(base, bytesToDecommit, MEM_DECOMMIT)) return false;
#else
      madvise(base, bytesToDecommit, MADV_DONTNEED);
      //Check on base's PROT_NONE after madvise (0 is successfulness)
      if (mprotect(base, bytesToDecommit, PROT_NONE) != 0) return false;
#endif
      if (target->pageCount == pagesToDecommit) {
        committedRanges.erase(target);
      } else {
        target->base = static_cast<char*>(target->base) + bytesToDecommit;
        target->pageCount -= pagesToDecommit;
      }
      return true;
    }

      bool IsCommitted(void* addr) {
      if (addr == nullptr) return false;
      for (auto& i : committedRanges) {
        auto start = reinterpret_cast<std::uintptr_t>(i.base);
        auto end = start + (i.pageCount * PageSize());
        auto ptr = reinterpret_cast<std::uintptr_t>(addr);
        if (ptr >= start && ptr < end) return true;
      }
      return false;
    }

    enum class MemoryPressure {
      Low, Medium, High
    };

    void HandleMemoryPressure(MemoryPressure level) {
      switch (level) {
        case MemoryPressure::Low:
          break;

        case MemoryPressure::Medium:
          for (std::size_t i = committedRanges.size(); i-- > 0; ) {
            auto& r = committedRanges[i];
            std::size_t pagesToDecommit = r.pageCount / 2;
            if (pagesToDecommit > 0)
              DecommitPages(r.base, pagesToDecommit);
          }
          break;

        case MemoryPressure::High:
          while (!committedRanges.empty()) {
            auto& r = committedRanges.back();
            DecommitPages(r.base, r.pageCount);
          }
          break;
      }
    }

  private:
    struct Range {
      void* base;
      size_t pageCount;
    };

    std::vector<Range> committedRanges;
  };
} // namespace noyxcore::memory
