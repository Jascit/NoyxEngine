/* SPDX-License-Identifier: Apache-2.0 */
/*
 * \file   vas_wrapper.cpp
 * \brief  
 *
 * Copyright (c) 2026 Project Contributors
 * \author Jascit <https://github.com/Jascit>
 * \date   04.02.2026
 * \note   
 */

#include <utility>
#include <platform/os/vas_wrapper.h>
#include <platform/os/os_detect.h>
#include <platform/debug.h>

#include "platform/utils.hpp"
#ifdef NOYX_WINDOWS
#include <platform/os/os_windows.h>
#elif defined(NOYX_LINUX)
#include <platform/os/os_linux.h>
#elif defined (NOYX_APPLE)
#include <platform/os/os_apple.h>
#endif

static FORCE_INLINE uint64_t page_size() noexcept {
#ifdef NOYX_WINDOWS
  SYSTEM_INFO sys_info;
  GetSystemInfo(&sys_info);
  return (uint64_t)sys_info.dwPageSize;
#else
  uint64_t p = sysconf(_SC_PAGESIZE);
  return (uint64_t)(p > 0 ? p : 4096);
#endif
}

// from vaw_prot_t
inline uint32_t vaw_to_page_prot(uint32_t vflags) noexcept {
#ifdef NOYX_WINDOWS
  uint32_t prot;
  if (vflags & VAW_FLAG_PROT_NONE) return PAGE_NOACCESS;
  bool r = (vflags & VAW_FLAG_PROT_READ) != 0;
  bool w = (vflags & VAW_FLAG_PROT_WRITE) != 0;
  bool x = (vflags & VAW_FLAG_PROT_EXEC) != 0;
  if (x) {
    if (r && w) return PAGE_EXECUTE_READWRITE;
    if (r) return PAGE_EXECUTE_READ;
    // write without read - map to execute+read+write defensively
    return PAGE_EXECUTE_READWRITE;
  } else {
    if (r && w) return PAGE_READWRITE;
    if (r) return PAGE_READONLY;
    return PAGE_READWRITE; // Windows doesn't have write-only user page;
  }
  return PAGE_NOACCESS;
#else
  if (vflags & VAW_FLAG_PROT_NONE) return PROT_NONE;
  uint32_t p = 0;
  if (vflags & VAW_FLAG_PROT_READ) p |= PROT_READ;
  if (vflags & VAW_FLAG_PROT_WRITE) p |= PROT_WRITE;
  if (vflags & VAW_FLAG_PROT_EXEC) p |= PROT_EXEC;
  if (p == 0) p = PROT_NONE;
  return p;
#endif
}

FORCE_INLINE uint64_t align_to(uint64_t val, uint64_t alignment) noexcept {
  return (val + alignment - 1) & ~(alignment - 1);
}
#ifdef NOYX_WINDOWS

static vaw_reserve_resp_t vaw_reserve_memory_windows_(const vaw_reserve_req_t* req, uint64_t pgsize) noexcept {
  vaw_reserve_resp_t out = {nullptr, 0, VAW_OK};

  uint64_t size = align_to(req->size, pgsize);

  // Setup allocation type and protection for reserve
  ULONG allocType = MEM_RESERVE;
  ULONG prot = PAGE_NOACCESS;

  if (req->alloc_flags & VAW_FLAG_LARGE_PAGES) {
    allocType |= MEM_LARGE_PAGES;
    // caller must ensure privilege; VirtualAlloc2 will fail otherwise
  }
  if (req->alloc_flags & VAW_FLAG_64K_PAGES) {
    allocType |= MEM_64K_PAGES;
  }

  // If alignment == pagesize and no address requirement, simple VirtualAlloc2 can be used without params.
  // But to honor alignment > page_size or preferred_addr we build address requirements.

  // Build AddressRequirements structure
  MEM_ADDRESS_REQUIREMENTS addrReq = {};
  addrReq.Alignment = req->alignment;

  if (req->preferred_addr) {
    if (req->alloc_flags & VAW_FLAG_FIXED_ADDRESS) {
      addrReq.LowestStartingAddress = req->preferred_addr;
      addrReq.HighestEndingAddress = reinterpret_cast<PVOID>(
        reinterpret_cast<uintptr_t>(req->preferred_addr) + size - 1);
    } else if (req->alloc_flags & VAW_FLAG_PREFER_ADDRESS) {
      addrReq.LowestStartingAddress = req->preferred_addr;
      addrReq.HighestEndingAddress = nullptr; // allow kernel to choose upper bound
    } else {
      // leave as full-range hint via Pointer only
    }
  }

  // MEM_EXTENDED_PARAMETER
  MEM_EXTENDED_PARAMETER param = {};
  memset(&param, 0, sizeof(param));
  param.Type = MemExtendedParameterAddressRequirements;
  param.Pointer = &addrReq;

  PVOID virt = VirtualAlloc2(GetCurrentProcess(),
                             nullptr,
                             size,
                             allocType,
                             prot,
                             &param,
                             1);
  if (!virt) {
    DWORD err = GetLastError();
    if (err == ERROR_PRIVILEGE_NOT_HELD) out.err = VAW_ERR_PERMISSION;
    else out.err = VAW_ERR_PLATFORM;
    return out;
  }
  out.base = virt;
  out.size = size;
  out.err = VAW_OK;
  return out;
}
#else
static vaw_reserve_resp_t vaw_reserve_memory_posix_(const vaw_reserve_req_t* req, uint64_t pg) noexcept {
  vaw_reserve_resp_t out = {nullptr, 0, VAW_OK};

  // size rounded up
  uint64_t size = align_to(req->size, pg);

  // Reserve should use PROT_NONE (no access) - commit/protect will set real protections later.
  int prot = PROT_NONE;
  int mflags = MAP_ANONYMOUS | MAP_PRIVATE;

  void* hint = req->preferred_addr;

  // Fixed address case
  if (hint && (req->alloc_flags & VAW_FLAG_FIXED_ADDRESS)) {
// MAP_FIXED/_NOREPLACE failed -> respect FIXED semantics: return error
#if defined(MAP_FIXED_NOREPLACE)
void* virt = mmap(hint, (size_t)size, prot, mflags | MAP_FIXED_NOREPLACE, -1, 0);
    if (virt!= MAP_FAILED) {
      out.base = virt;
      out.size = size;
      return out;
    }
out.err= VAW_ERR_PLATFORM;
    return out;
#else
// MAP_FIXED (risky - overwrites). We still obey FIXED semantics: either success or fail.
void* virt = mmap(hint, (size_t)size, prot, mflags | MAP_FIXED, -1, 0);
    if (virt!= MAP_FAILED) {
      out.base = virt;
      out.size = size;
      return out;
    }
out.err= VAW_ERR_PLATFORM;
    return out;
#endif
}

// Hint / Prefer address: try hint as a normal mmap (kernel may ignore the hint)
  if (hint) {
  void* virt = mmap(hint, (size_t)size, prot, mflags, -1, 0);
  if (virt != MAP_FAILED) {
    // check alignment
    if (((uintptr_t)virt % req->alignment) == 0) {
      out.base = virt;
      out.size = size;
      return out;
    } else {
      // not aligned: unmap and fall through to carve strategy
      munmap(virt, (size_t)size);
    }
  }
  // if mmap failed with EACCES/ENOMEM -> continue to carve or fail later
}

// Carve strategy: mmap(size + alignment) and carve out aligned subrange
// protect against overflow
  if (req->alignment> UINT64_MAX- size) {
    out.err = VAW_ERR_INVALID_ARG;
    return out;
  }
size_t alloc_size = (size_t)(size + req->alignment);

void* big = mmap(nullptr, alloc_size, prot, mflags, -1, 0);
  if (big== MAP_FAILED) {
    out.err = (errno == ENOMEM) ? VAW_ERR_OOM : VAW_ERR_PLATFORM;
    return out;
  }

uintptr_t start = (uintptr_t)big;
uintptr_t aligned = (start + (req->alignment - 1)) & ~(req->alignment - 1);
size_t prefix = (size_t)(aligned - start);
size_t suffix = (size_t)(alloc_size - prefix - size);

  if (prefix> 0) {
    munmap((void*)start, prefix);
  }
  if (suffix> 0) {
    munmap((void*)(aligned + size), suffix);
  }

out.base= (void*)aligned;
out.size= size;
out.err= VAW_OK;
  return out;
}
#endif

vaw_reserve_resp_t vaw_reserve_memory(const vaw_reserve_req_t* req, uint64_t pg_size) {
  if (!req) return {nullptr, 0, VAW_ERR_INVALID_ARG};
  vaw_reserve_req_t local = *req;

  if (req->size == 0) return {nullptr, 0, VAW_ERR_INVALID_ARG};

  if (local.alignment == 0) local.alignment = pg_size;

  if (local.alignment % pg_size != 0) return {nullptr, 0, VAW_ERR_INVALID_ARG};

#ifdef NOYX_WINDOWS
  return vaw_reserve_memory_windows_(&local, pg_size);
#else
  return vaw_reserve_memory_posix_(&local, pg_size);
#endif
}

vaw_release_resp_t vaw_release_memory(const vaw_release_req_t* req) {
  vaw_release_resp_t out{VAW_OK};
  if (!req || !req->base || req->size == 0) return {VAW_ERR_INVALID_ARG};
#ifdef NOYX_WINDOWS
  BOOL success = VirtualFree(req->base, req->size,MEM_RELEASE);
  if (success == FALSE) {
    DWORD err = GetLastError();
    if (err == ERROR_INVALID_ADDRESS) out.err = VAW_ERR_INVALID_ADDRESS;
    else if (err == ERROR_INVALID_PARAMETER) out.err = VAW_ERR_INVALID_ARG;
    else if (err == ERROR_ACCESS_DENIED) out.err = VAW_ERR_PERMISSION;
    else if (err == ERROR_NOT_ENOUGH_MEMORY) out.err = VAW_ERR_OOM;
    else out.err = VAW_ERR_PLATFORM;
    return out;
  }
#else
  munmap(req->base, req->size);
#endif
  return {VAW_OK};
}

vaw_commit_resp_t vaw_commit_pages(const vaw_commit_req_t* req, uint64_t pg_size) {
  vaw_commit_resp_t out = {VAW_OK};

  if (!req || !req->base || req->size == 0) {
    out.err = VAW_ERR_INVALID_ARG;
    return out;
  }

  if ((uintptr_t)req->base % pg_size != 0 || req->size % page_size() != 0) {
    out.err = VAW_ERR_INVALID_ARG;
    return out;
  }

  uint32_t prot = vaw_to_page_prot(req->prot);

#ifdef NOYX_WINDOWS
  uint32_t flags = MEM_COMMIT;
  if (req->alloc_flags & VAW_FLAG_LARGE_PAGES) {
    flags |= MEM_LARGE_PAGES;
    // caller must ensure privilege; VirtualAlloc2 will fail otherwise
  }
  if (req->alloc_flags & VAW_FLAG_64K_PAGES) {
    flags |= MEM_64K_PAGES;
  }

  PVOID virt = VirtualAlloc2(GetCurrentProcess(),
                             req->base,
                             req->size,
                             flags,
                             prot,
                             nullptr,
                             0);
  if (!virt) {
    DWORD err = GetLastError();
    if (err == ERROR_PRIVILEGE_NOT_HELD) out.err = VAW_ERR_PERMISSION;
    else if (err == ERROR_INVALID_PARAMETER) out.err = VAW_ERR_INVALID_ARG;
    else if (err == ERROR_INVALID_ADDRESS) out.err = VAW_ERR_INVALID_ADDRESS;
    else if (err == ERROR_NOT_ENOUGH_MEMORY) out.err = VAW_ERR_OOM;
    else out.err = VAW_ERR_PLATFORM;
    return out;
  }

  return out;
#else
  int mmap_prot = vaw_to_page_prot(req->prot);

  int flags = MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED;

#ifdef MAP_POPULATE
  if (req->alloc_flags & VAW_FLAG_PREFAULT)
    flags |= MAP_POPULATE;
#endif

  void* res = mmap(req->base,
                   req->size,
                   mmap_prot,
                   flags,
                   -1,
                   0);

  if (res == MAP_FAILED) {
    switch (errno) {
      case EACCES:
      case EPERM:
        out.err = VAW_ERR_PERMISSION;
        break;
      case EINVAL:
        out.err = VAW_ERR_INVALID_ARG;
        break;
      case ENOMEM:
        out.err = VAW_ERR_OOM;
        break;
#ifdef ENOTSUP
      case ENOTSUP:
#endif
      default:
        out.err = VAW_ERR_PLATFORM;
        break;
    }
    return out;
  }

  if (res != req->base) {
    munmap(res, req->size);
    out.err = VAW_ERR_INVALID_ADDRESS;
    return out;
  }

  return out;
#endif
}

vaw_commit_resp_t vaw_decommit_pages(const vaw_commit_req_t* req) {
  vaw_commit_resp_t out{VAW_OK};
  if (!req || !req->base || req->size == 0) return {VAW_ERR_INVALID_ARG};

#ifdef NOYX_WINDOWS
  BOOL success = 0;
  if (req->alloc_flags & VAW_FLAG_LARGE_PAGES) {
    success = VirtualFree(req->base, 0, MEM_RELEASE);
  } else {
    success = VirtualFree(req->base, req->size, MEM_DECOMMIT);
  }

  if (success == FALSE) {
    DWORD err = GetLastError();
    if (err == ERROR_INVALID_ADDRESS) out.err = VAW_ERR_INVALID_ADDRESS;
    else if (err == ERROR_INVALID_PARAMETER) out.err = VAW_ERR_INVALID_ARG;
    else if (err == ERROR_ACCESS_DENIED) out.err = VAW_ERR_PERMISSION;
    else if (err == ERROR_NOT_ENOUGH_MEMORY) out.err = VAW_ERR_OOM;
    else out.err = VAW_ERR_PLATFORM;
    return out;
  }

  return out;
#else /* POSIX */

  if (req->alloc_flags & VAW_FLAG_LARGE_PAGES) {
    /* Large pages: munmap == release */
    if (munmap(req->base, req->size) != 0) {
      if (errno == EINVAL) out.err = VAW_ERR_INVALID_ADDRESS;
      else out.err = VAW_ERR_PLATFORM;
    }
    return out;
  }

  if (mprotect(req->base, req->size, PROT_NONE) != 0) {
    if (errno == EACCES || errno == EPERM) out.err = VAW_ERR_PERMISSION;
    else if (errno == EINVAL) out.err = VAW_ERR_INVALID_ARG;
    else out.err = VAW_ERR_PLATFORM;
    return out;
  }

#if defined(MADV_FREE)
  int adv = madvise(req->base, req->size, MADV_FREE);
#else
  int adv = madvise(req->base, req->size, MADV_DONTNEED);
#endif

  if (adv != 0) {
    if (errno == EINVAL) out.err = VAW_ERR_INVALID_ARG;
    else if (errno == ENOMEM) out.err = VAW_ERR_OOM;
    else if (errno == EPERM) out.err = VAW_ERR_PERMISSION;
    else out.err = VAW_ERR_PLATFORM;
    return out;
  }

  return out;
#endif
}

vaw_map_resp_t vaw_map(const vaw_map_req_t* req) {

}

vaw_unmap_resp_t vaw_unmap(const vaw_unmap_req_t* req) {}

vaw_advise_resp_t vaw_advise(const vaw_advise_req_t* req) {}
