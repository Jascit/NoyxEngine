/* SPDX-License-Identifier: Apache-2.0 */

/**
 * @file     vas_wrapper.cpp
 * @brief
 *
 * @author   Jascit (https://github.com/Jascit)
 * @date     04.02.2026
 *
 * @copyright Copyright (c) 2026 The Project Contributors
 */

#include <utility>
#include <memory/vas_wrapper.hpp>
#include <platform/os/os_detect.h>
#include <platform/debug.h>

#if defined(NOYX_WINDOWS)
#include <windows.h>
#elif defined(NOYX_LINUX) || defined(NOYX_APPLE)
#include <unistd.h>
#include <errno.h>
#include <sys/mman.h>
#endif

using namespace noyxcore::memory::vas;

FORCE_INLINE auto round_up(const std::uint64_t value, const std::uint64_t align) noexcept -> std::uint64_t {
  return ((value + align - 1) / align) * align;
};

#if defined(NOYX_WINDOWS)
FORCE_INLINE Error from_windows_error_(DWORD err) noexcept {
  switch (err) {
    case ERROR_ACCESS_DENIED:
      return Error::Permission;
    case ERROR_NOT_ENOUGH_MEMORY:
      return Error::OOM;
    case ERROR_INVALID_PARAMETER:
      return Error::InvalidArg;
    default:
      return Error::Internal;
  }
}

FORCE_INLINE DWORD to_windows_prots_(std::uint32_t flags) noexcept {
  const bool read = (flags & Flag::ProtRead) != 0;
  const bool write = (flags & Flag::ProtWrite) != 0;
  const bool execute = (flags & Flag::ProtExec) != 0;

  if (!read && !write && !execute)
    return PAGE_NOACCESS;

  if (execute) {
    if (read && write) return PAGE_EXECUTE_READWRITE;
    if (read) return PAGE_EXECUTE_READ;
    if (write) return PAGE_EXECUTE_READWRITE; // Windows has no WX-only protection
    return PAGE_EXECUTE;
  }

  if (read && write) return PAGE_READWRITE;
  if (read) return PAGE_READONLY;
  if (write) return PAGE_READWRITE;
  return PAGE_NOACCESS;
}


FORCE_INLINE DWORD to_windows_flags_(std::uint32_t flags) noexcept {
  DWORD result = 0;
  if (flags & Flag::LargePages) result |= MEM_LARGE_PAGES;
  if (flags & Flag::Page64K) result |= MEM_64K_PAGES;
  return result;
}

ReserveResponse reserve_memory_windows_(std::uint64_t size, void* preferred_addr, DWORD alloc_flags) noexcept {
  ReserveResponse resp(nullptr, 0, Error::Internal);
  DWORD flags = MEM_RESERVE | alloc_flags;

  void* virtual_address = VirtualAlloc(preferred_addr,
                                       size,
                                       flags,
                                       PAGE_NOACCESS);

  if (virtual_address == nullptr) {
    DWORD error = GetLastError();
    resp.err = from_windows_error_(error);
    return resp;
  }

  resp.size = size;
  resp.base = virtual_address;
  resp.err = Error::Ok;
  return resp;
}

CommitResponse commit_pages_windows_(std::uint64_t size, void* addr, DWORD alloc_flags, DWORD prots,
                                     uint64_t page_size) noexcept {
  DWORD flags = MEM_COMMIT | alloc_flags;
  void* result = VirtualAlloc(addr, size, flags, prots);
  if (result == nullptr) {
    return {from_windows_error_(GetLastError())};
  }
  for (char* ptr = static_cast<char*>(result); ptr < static_cast<char*>(result) + size; ptr += page_size) {
    ptr[0] = 0;
  }
  return {Error::Ok};
}

#elif defined(NOYX_LINUX) || defined(NOYX_APPLE)
FORCE_INLINE Error from_unix_error_(int err/*errno*/) noexcept {
  switch (err) {
    case EEXIST: return Error::InvalidAddress;
    case EACCES: return Error::Permission;
    case ENOMEM: return Error::OOM;
    case EINVAL: return Error::InvalidArg;
    default: return Error::Internal;
  }
}

FORCE_INLINE int to_unix_prots_(std::uint32_t flags) noexcept {
  const bool read = (flags & Flag::ProtRead) != 0;
  const bool write = (flags & Flag::ProtWrite) != 0;
  const bool execute = (flags & Flag::ProtExec) != 0;

  if (!read && !write && !execute)
    return PROT_NONE;

  int protection = 0;

  if (write) {
    protection |= PROT_WRITE;
    protection |= PROT_READ; // ensure Windows-like behavior
  } else if (read) {
    protection |= PROT_READ;
  }

  if (execute) {
    protection |= PROT_EXEC;
  }

  return protection;
}

FORCE_INLINE int to_unix_flags_(std::uint32_t flags) noexcept {
  int result = 0;
  if (flags & Flag::FixedAddress && flags & Flag::PreferAddress) {
    result = -1;
    return result;
  }
  if (flags & Flag::FixedAddress) result |= MAP_FIXED;
#if defined (NOYX_LINUX)
if (flags &Flag::LargePages) result|= MAP_HUGETLB;
#endif
return result;
}

ReserveResponse reserve_memory_unix_(std::uint64_t size, void* preferred_addr, int flags) noexcept {
  ReserveResponse resp(nullptr, 0, Error::Internal);

  int mmap_flags = MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE;

  if (flags & MAP_FIXED) {
#if defined(MAP_FIXED_NOREPLACE)
mmap_flags= mmap_flags| MAP_FIXED_NOREPLACE;
#else
mmap_flags= mmap_flags| MAP_FIXED;
#endif
}

void* virtual_address = mmap(preferred_addr, size, PROT_NONE, mmap_flags, -1, 0);
  if (virtual_address== MAP_FAILED) {
    resp.err = from_unix_error_(errno);
    return resp;
  }
resp.size= size;
resp.base= virtual_address;
resp.err= Error::Ok;
  return resp;
}
#endif

[[nodiscard]] ReserveResponse noyxcore::memory::vas::reserve_memory(const ReserveRequest& req,
                                                                    std::uint64_t allocation_granularity) noexcept {
  // Large pages and non-pageable memory cannot be reserved; they must be committed immediately.
  if (req.alloc_flags & Flag::LargePages) return {nullptr, 0, Error::InvalidArg};
  if (req.alloc_flags & Flag::NonPaged) return {nullptr, 0, Error::InvalidArg};
  if (req.size == 0) return {nullptr, 0, Error::InvalidArg};
  if (req.alloc_flags & Flag::FixedAddress && req.preferred_addr) return {nullptr, 0, Error::InvalidArg};

  std::uint64_t alignment = allocation_granularity;
  if (req.alignment != 0) {
    if (req.alignment % allocation_granularity != 0) return {nullptr, 0, Error::InvalidArg};
    alignment = req.alignment;
  }

  if (req.preferred_addr != nullptr) {
    auto addr = reinterpret_cast<std::uintptr_t>(req.preferred_addr);
    if (addr % alignment != 0)
      return {nullptr, 0, Error::InvalidArg};
  }
  std::uint64_t aligned_size = round_up(req.size, alignment);

#if defined(NOYX_WINDOWS)
  const DWORD req_flags = to_windows_flags_(req.alloc_flags);
  return reserve_memory_windows_(aligned_size, req.preferred_addr, req_flags);
#elif defined(NOYX_LINUX) || defined(NOYX_APPLE)
  const int req_flags = to_unix_flags_(req.alloc_flags);
  return reserve_memory_unix_(req.size, req.preferred_addr, req_flags);
#endif
}

[[nodiscard]] ReleaseResponse noyxcore::memory::vas::release_memory(const ReleaseRequest& req) noexcept {
#if defined(NOYX_WINDOWS)
  if (!VirtualFree(req.base, 0, MEM_RELEASE)) {
    DWORD error = GetLastError();
    return {from_windows_error_(error)};
  }
  return {Error::Ok};
#elif defined(NOYX_LINUX) || defined(NOYX_APPLE)
  if (munmap(req.base, req.size) != 0) {
    return {from_unix_error_(errno)};
  }
  return {Error::Ok};
#endif
}

[[nodiscard]] CommitResponse noyxcore::memory::vas::commit_pages(const CommitRequest& req,
                                                                 std::uint64_t page_size) noexcept {
  if (req.size == 0) {
    return {Error::InvalidArg};
  }

  const bool large_pages = (req.alloc_flags & Flag::LargePages) != 0;
  if (large_pages && req.base != nullptr) {
    return {Error::InvalidArg};
  }

  void* raw_addr = (req.base == nullptr)
                     ? nullptr
                     : static_cast<char*>(req.base) + req.offset;

  if (reinterpret_cast<std::uintptr_t>(raw_addr) % page_size != 0 || req.size % page_size != 0) {
    return {Error::InvalidArg};
  }
#if defined(NOYX_WINDOWS)
  // try to allocate large pages
  if (large_pages) {
    if (/*TODO: Process info mb, check for Permission)*/ false) {
      /*TODO: spec func for HugePages*/
    }
  }

  DWORD windows_flags = to_windows_flags_(req.alloc_flags);
  DWORD windows_prots = to_windows_prots_(req.protection);

  return commit_pages_windows_(
    req.size,
    raw_addr,
    windows_flags,
    windows_prots,
    page_size
  );
#elif defined(NOYX_LINUX) || defined(NOYX_APPLE)
#if !defined(NOYX_APPLE)
  if (req.alloc_flags & Flag::LargePages) {
    if (/*TODO: Process info mb, check for Permission)*/false) {
      /*TODO: spec func for HugePages*/
    }
  }
#endif
  if (mprotect(raw_addr, req.size, PROT_READ | PROT_WRITE) != 0) {
    return {from_unix_error_(errno)};
  }

  char* begin = static_cast<char*>(raw_addr);
  char* end = begin + req.size;

  for (char* ptr = begin; ptr < end; ptr += page_size) {
    volatile char* p = ptr;
    *p = 0;
  }

  int prots = to_unix_prots_(req.protection);
  if (mprotect(raw_addr, req.size, prots) != 0) {
    return {from_unix_error_(errno)};
  }

  return {Error::Ok};

#endif
  //TODO: lazy/sobald commit flag?
}


[[nodiscard]] DecommitResponse noyxcore::memory::vas::decommit_pages(const DecommitRequest& req) noexcept {
  if (req.base == nullptr) return {Error::InvalidArg};
  if (req.size == 0) return {Error::InvalidArg};
  if (req.alloc_flags & Flag::LargePages) return {Error::InvalidArg};
  void* addr = static_cast<char*>(req.base) + req.offset;

#if defined(NOYX_WINDOWS)
  BOOL result = VirtualFree(addr, req.size, MEM_DECOMMIT);
  if (result == false) {
    DWORD error = GetLastError();
    return {from_windows_error_(error)};
  }
  return {Error::Ok};
#elif defined(NOYX_LINUX) || defined(NOYX_APPLE)
  if (madvise(addr, req.size, MADV_DONTNEED) != 0) {
    return {from_unix_error_(errno)};
  }
  if (mprotect(addr, req.size, PROT_NONE) != 0) {
    return {from_unix_error_(errno)};
  }
  return {Error::Ok};
#endif
}

//
//[[nodiscard]] MapResponse map(const MapRequest& req) noexcept {}
//
//[[nodiscard]] UnmapResponse unmap(const UnmapRequest& req) noexcept {}
//
//[[nodiscard]] AdviseResponse advise(const AdviseRequest& req) noexcept {}
