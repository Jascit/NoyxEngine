/* SPDX-License-Identifier: Apache-2.0 */

/**
 * @file     vas_wrapper.cpp
 * @brief
 *
 * @date     04.02.2026
 *
 * @copyright Copyright (c) 2026 The Project Contributors
 */

#include <memory/vas_wrapper.hpp>
#include <platform/debug.h>
#include <platform/os/os_detect.h>

#include <utility>

#if defined(NOYX_WINDOWS)
#include <windows.h>
#elif defined(NOYX_LINUX) || defined(NOYX_APPLE)
#include <errno.h>
#include <sys/mman.h>
#include <unistd.h>
#endif

using namespace noyxcore::memory::vas;

// TODO: windows version of functions
// TODO: reserve_memory: Large Pages check

#if defined(NOYX_WINDOWS)
FORCE_INLINE static Error from_windows_error(DWORD err) noexcept
{
    switch (err)
    {
    case ERROR_ACCESS_DENIED:
        return Error::PERMISSION;
    case ERROR_NOT_ENOUGH_MEMORY:
        return Error::OOM;
    case ERROR_INVALID_PARAMETER:
        return Error::INVALID_ARG;
    default:
        return Error::INTERNAL;
    }
}

FORCE_INLINE static DWORD to_windows_prots(std::uint32_t flags) noexcept
{
    const bool read    = (flags & Flag::PROTECTION_READ) != 0;
    const bool write   = (flags & Flag::PROTECTION_WRITE) != 0;
    const bool execute = (flags & Flag::PROTECTION_EXEC) != 0;

    if (!read && !write && !execute)
        return PAGE_NOACCESS;

    if (execute)
    {
        if (read && write)
            return PAGE_EXECUTE_READWRITE;
        if (read)
            return PAGE_EXECUTE_READ;
        if (write)
            return PAGE_EXECUTE_READWRITE; // Windows has no WX-only protection
        return PAGE_EXECUTE;
    }

    if (read && write)
        return PAGE_READWRITE;
    if (read)
        return PAGE_READONLY;
    return PAGE_NOACCESS;
}

FORCE_INLINE static DWORD to_windows_flags(std::uint32_t flags) noexcept
{
    DWORD result = 0;
    if (flags & Flag::LARGE_PAGES)
        result |= MEM_LARGE_PAGES;
    if (flags & Flag::PAGE_64KB)
        result |= MEM_64K_PAGES;
    return result;
}

static ReserveResponse reserve_memory_windows(std::uint64_t size, void *preferred_addr,
                                              DWORD alloc_flags) noexcept
{
    ReserveResponse resp(nullptr, 0, Error::INTERNAL);
    DWORD flags = MEM_RESERVE | alloc_flags;

    void *virtual_address = VirtualAlloc(preferred_addr, size, flags, PAGE_NOACCESS);

    if (virtual_address == nullptr)
    {
        DWORD error = GetLastError();
        resp.err    = from_windows_error(error);
        return resp;
    }

    resp.size = size;
    resp.base = virtual_address;
    resp.err  = Error::OK;
    return resp;
}

static CommitResponse commit_pages_windows(std::uint64_t size, void *addr, DWORD alloc_flags,
                                           DWORD prots, uint64_t page_size) noexcept
{
    DWORD flags  = MEM_COMMIT | alloc_flags;
    void *result = VirtualAlloc(addr, size, flags, prots);
    if (result == nullptr)
    {
        return {from_windows_error(GetLastError())};
    }
    for (char *ptr = static_cast<char *>(result); ptr < static_cast<char *>(result) + size;
         ptr += page_size)
    {
        ptr[0] = 0;
    }
    return {Error::OK};
}

#elif defined(NOYX_LINUX) || defined(NOYX_APPLE)
FORCE_INLINE Error from_unix_error(int err /*errno*/) noexcept
{
    switch (err)
    {
    case EEXIST:
        return Error::INVALID_ADDRESS;
    case EACCES:
        return Error::PERMISSION;
    case ENOMEM:
        return Error::OOM;
    case EINVAL:
        return Error::INVALID_ARG;
    default:
        return Error::INTERNAL;
    }
}

FORCE_INLINE int to_unix_prots(std::uint32_t flags) noexcept
{
    const bool read    = (flags & Flag::PROTECTION_READ) != 0;
    const bool write   = (flags & Flag::PROTECTION_WRITE) != 0;
    const bool execute = (flags & Flag::PROTECTION_EXEC) != 0;

    if (!read && !write && !execute)
        return PROT_NONE;

    int protection = 0;

    if (write)
    {
        protection |= PROT_WRITE;
        protection |= PROT_READ; // ensure Windows-like behavior
    }
    else if (read)
    {
        protection |= PROT_READ;
    }

    if (execute)
    {
        protection |= PROT_EXEC;
    }

    return protection;
}

FORCE_INLINE int to_unix_flags(std::uint32_t flags) noexcept
{
    int result = 0;
    if (flags & Flag::FIXED_ADDRESS && flags & Flag::PREFER_ADDRESS)
    {
        result = -1;
        return result;
    }
    if (flags & Flag::FIXED_ADDRESS)
        result |= MAP_FIXED;
#if defined(NOYX_LINUX)
    if (flags & Flag::LARGE_PAGES)
        result |= MAP_HUGETLB;
#endif
    return result;
}

static ReserveResponse reserve_memory_unix(std::uint64_t size, void *preferred_addr,
                                           int flags) noexcept
{
    ReserveResponse resp(nullptr, 0, Error::INTERNAL);

    int mmap_flags = MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE;

    if (flags & MAP_FIXED)
    {
#if defined(MAP_FIXED_NOREPLACE)
        mmap_flags = mmap_flags | MAP_FIXED_NOREPLACE;
#else
        mmap_flags = mmap_flags | MAP_FIXED;
#endif
    }

    void *virtual_address = mmap(preferred_addr, size, PROT_NONE, mmap_flags, -1, 0);
    if (virtual_address == MAP_FAILED)
    {
        resp.err = from_unix_error(errno);
        return resp;
    }
    resp.size = size;
    resp.base = virtual_address;
    resp.err  = Error::OK;
    return resp;
}
#endif

[[nodiscard]] ReserveResponse
noyxcore::memory::vas::reserve_memory(const ReserveRequest &req,
                                      std::uint64_t allocation_granularity) noexcept
{
    // Large pages and non-pageable memory cannot be reserved; they must be
    // committed immediately.
    if (req.alloc_flags & Flag::LARGE_PAGES)
        return {nullptr, 0, Error::INVALID_ARG};
    if (req.alloc_flags & Flag::NON_PAGED)
        return {nullptr, 0, Error::INVALID_ARG};
    if (req.size == 0)
        return {nullptr, 0, Error::INVALID_ARG};
    if (req.alloc_flags & Flag::FIXED_ADDRESS && req.preferred_addr)
        return {nullptr, 0, Error::INVALID_ARG};

    std::uint64_t alignment = (req.alignment == 0) ? allocation_granularity : req.alignment;
    // TODO: false, new idea: uintptr_t aligned = ((uintptr_t)addr + alignment -
    // 1) & ~(alignment - 1); or with params windows garantiert 64KB alignment;
    // also muss ich das nicht ueberpruefen; Flag::FixedAddr
    if (alignment < allocation_granularity)
        return {nullptr, 0, Error::INVALID_ARG};
    if (alignment % allocation_granularity != 0)
        return {nullptr, 0, Error::INVALID_ARG};

    if (req.preferred_addr != nullptr)
    {
        auto addr = reinterpret_cast<std::uintptr_t>(req.preferred_addr);
        if (addr % alignment != 0)
            return {nullptr, 0, Error::INVALID_ARG};
    }
    std::uint64_t aligned_size = round_up(req.size, alignment);

#if defined(NOYX_WINDOWS)
    const DWORD req_flags = to_windows_flags(req.alloc_flags);
    return reserve_memory_windows(aligned_size, req.preferred_addr, req_flags);
#elif defined(NOYX_LINUX) || defined(NOYX_APPLE)
    const int req_flags = to_unix_flags(req.alloc_flags);
    return reserve_memory_unix(req.size, req.preferred_addr, req_flags);
#endif
}

[[nodiscard]] ReleaseResponse
noyxcore::memory::vas::release_memory(const ReleaseRequest &req) noexcept
{
#if defined(NOYX_WINDOWS)
    if (!VirtualFree(req.base, req.size, MEM_RELEASE))
    {
        DWORD error = GetLastError();
        return {from_windows_error(error)};
    }
    return {Error::OK};
#elif defined(NOYX_LINUX) || defined(NOYX_APPLE)
    if (munmap(req.base, req.size) != 0)
    {
        return {from_unix_error(errno)};
    }
    return {Error::OK};
#endif
}

[[nodiscard]] CommitResponse noyxcore::memory::vas::commit_pages(const CommitRequest &req,
                                                                 std::uint64_t page_size) noexcept
{
    if (req.size == 0)
    {
        return {Error::INVALID_ARG};
    }

    const bool large_pages = (req.alloc_flags & Flag::LARGE_PAGES) != 0;
    if (large_pages && req.base != nullptr)
    {
        return {Error::INVALID_ARG};
    }

    void *raw_addr = (req.base == nullptr) ? nullptr : static_cast<char *>(req.base) + req.offset;

#if defined(NOYX_WINDOWS)
    // try to allocate large pages
    if (large_pages)
    {
        if (/*TODO: Process info mb, check for PERMISSION)*/ false)
        {
            /*TODO: spec func for HugePages*/
        }
    }
    // TODO: addr and size must to be aligned to page_size
    // TODO: error handling
    // TODO: lazy/sobald commit flag?

    DWORD windows_flags = to_windows_flags(req.alloc_flags);
    DWORD windows_prots = to_windows_prots(req.protection);

    return commit_pages_windows(req.size, const_cast<void *>(raw_addr), windows_flags,
                                windows_prots, page_size);
#elif defined(NOYX_LINUX) || defined(NOYX_APPLE)
#if !defined(NOYX_APPLE)
    if (req.alloc_flags & Flag::LARGE_PAGES)
    {
        if (/*TODO: Process info mb, check for PERMISSION)*/ false)
        {
            /*TODO: spec func for HugePages*/
        }
    }
#endif
    if (mprotect(raw_addr, req.size, PROT_READ | PROT_WRITE) != 0)
    {
        return {from_unix_error(errno)};
    }

    char *begin = static_cast<char *>(raw_addr);
    char *end   = begin + req.size;

    for (char *ptr = begin; ptr < end; ptr += page_size)
    {
        volatile char *p = ptr;
        *p               = 0;
    }

    int prots = to_unix_prots(req.protection);
    if (mprotect(raw_addr, req.size, prots) != 0)
    {
        return {from_unix_error(errno)};
    }

    return {Error::OK};

#endif
    // TODO: lazy/sobald commit flag?
}

[[nodiscard]] DecommitResponse
noyxcore::memory::vas::decommit_pages(const DecommitRequest &req) noexcept
{
    if (req.base == nullptr)
        return {Error::INVALID_ARG};
    if (req.size == 0)
        return {Error::INVALID_ARG};
    if (req.alloc_flags & Flag::LARGE_PAGES)
        return {Error::INVALID_ARG};
    void *addr = static_cast<char *>(req.base) + req.offset;

#if defined(NOYX_WINDOWS)
    LPVOID result = VirtualAlloc(addr, req.size, MEM_DECOMMIT, PAGE_READWRITE);
    if (result == nullptr)
    {
        DWORD error = GetLastError();
        return {from_windows_error(error)};
    }
    return {Error::OK};
#elif defined(NOYX_LINUX) || defined(NOYX_APPLE)
    if (madvise(addr, req.size, MADV_DONTNEED) != 0)
    {
        return {from_unix_error(errno)};
    }
    if (mprotect(addr, req.size, PROT_NONE) != 0)
    {
        return {from_unix_error(errno)};
    }

    return {Error::OK};
#endif
}

//
//[[nodiscard]] MapResponse map(const MapRequest& req) noexcept {}
//
//[[nodiscard]] UnmapResponse unmap(const UnmapRequest& req) noexcept {}
//
//[[nodiscard]] AdviseResponse advise(const AdviseRequest& req) noexcept {}
