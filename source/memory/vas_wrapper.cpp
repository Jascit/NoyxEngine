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

// TODO: windows version of functions
// TODO: reserve_memory: Large Pages check
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

  int prot = 0;

  if (write) {
    prot |= PROT_WRITE;
    prot |= PROT_READ; // ensure Windows-like behavior
  }
  else if (read) {
    prot |= PROT_READ;
  }

  if (execute) {
    prot |= PROT_EXEC;
  }

  return prot;
}

FORCE_INLINE int to_unix_flags_(std::uint32_t flags) noexcept {
  int result = 0;
  if (flags & Flag::FixedAddress && flags & Flag::PreferAddress) {
    result = -1;
    return result;
  }
  if (flags & Flag::FixedAddress) result |= MAP_FIXED;
  if (flags & Flag::LargePages) result |= MAP_HUGETLB;
  return result;
}

ReserveResponse reserve_memory_unix_(std::uint64_t size, void* preferred_addr, int flags) noexcept {
  ReserveResponse resp(nullptr, 0, Error::Internal);

  int mmap_flags = MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE;

  if (flags & MAP_FIXED) {
#if defined(MAP_FIXED_NOREPLACE)
    int mmap_flags = mmap_flags | MAP_FIXED_NOREPLACE;
#else
    int mmap_flags = mmap_flags | MAP_FIXED;
#endif
  }
  
  void* virtual_address = mmap(preferred_addr, size, PROT_NONE, mmap_flags, -1, 0);
  if (!virtual_address) {
    resp.err = from_unix_error_(errno);
    return resp;
  }
  resp.size = size;
  resp.base = virtual_address;
  resp.err = Error::Ok;
  return resp;
}
#endif

[[nodiscard]] ReserveResponse noyxcore::memory::vas::reserve_memory(const ReserveRequest& req,
  std::uint64_t allocation_granularity) noexcept {
  // Large pages and non-pageable memory cannot be reserved; they must be committed immediately.
  if (req.alloc_flags & Flag::LargePages) return { nullptr, 0, Error::InvalidArg };
  if (req.alloc_flags & Flag::NonPaged) return { nullptr, 0, Error::InvalidArg };
  if (req.size == 0) return { nullptr, 0, Error::InvalidArg };
  if (req.alloc_flags & Flag::FixedAddress && req.preferred_addr) return { nullptr, 0, Error::InvalidArg };

  std::uint64_t alignment = (req.alignment == 0) ? allocation_granularity : req.alignment;
  // TODO: false, new idea: uintptr_t aligned = ((uintptr_t)addr + alignment - 1) & ~(alignment - 1); or with params
  // windows garantiert 64KB alignment; also muss ich das nicht ueberpruefen; Flag::FixedAddr
  if (alignment < allocation_granularity) return { nullptr, 0, Error::InvalidArg };
  if (alignment % allocation_granularity != 0) return { nullptr, 0, Error::InvalidArg };

  if (req.preferred_addr != nullptr) {
    auto addr = reinterpret_cast<std::uintptr_t>(req.preferred_addr);
    if (addr % alignment != 0)
      return { nullptr, 0, Error::InvalidArg };
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
  if (!VirtualFree(req.base, req.size, MEM_RELEASE)) {
    DWORD error = GetLastError();
    return { from_windows_error_(error) };
  }
  return { Error::Ok };
#elif defined(NOYX_LINUX) || defined(NOYX_APPLE)
  if (munmap(req.base, req.size) != 0) {
    return { from_unix_error_(errno) };
  }
  return { Error::Ok };
#endif
}

//[[nodiscard]] CommitResponse commit_pages(const CommitRequest& req, std::uint64_t page_size) noexcept {}
//
//[[nodiscard]] CommitResponse decommit_pages(const CommitRequest& req) noexcept {}
//
//[[nodiscard]] MapResponse map(const MapRequest& req) noexcept {}
//
//[[nodiscard]] UnmapResponse unmap(const UnmapRequest& req) noexcept {}
//
//[[nodiscard]] AdviseResponse advise(const AdviseRequest& req) noexcept {}
//
//[[nodiscard]] std::uint64_t page_size() noexcept {}
