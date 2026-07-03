/* SPDX-License-Identifier: Apache-2.0 */

/**
 * @file     vas_wrapper.hpp
 * @brief
 *
 * @date     04.02.2026
 *
 * @copyright Copyright (c) 2026 The Project Contributors
 */
#pragma once

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <platform/debug.h>

namespace noyxcore::memory::vas
{
enum class Error : std::uint8_t
{
    OK = 0,
    INVALID_ARG,
    INVALID_ADDRESS,
    OOM,
    NOT_COMMITTED,
    ALREADY_COMMITTED,
    NO_MAPPING,
    PERMISSION,
    UNSUPPORTED,
    PLATFORM,
    INTERNAL
};

enum class Flag : std::uint32_t
{
    NONE             = 0u,
    PROTECTION_NONE  = 1u << 0,
    PROTECTION_READ  = 1u << 1,
    PROTECTION_WRITE = 1u << 2,
    PROTECTION_EXEC  = 1u << 3,
    FIXED_ADDRESS    = 1u << 8,
    PREFER_ADDRESS   = 1u << 9,
    LARGE_PAGES      = 1u << 16,
    PAGE_64KB        = 1u << 17,
    NON_PAGED        = 1u << 18
};

template <std::unsigned_integral T>
constexpr bool operator==(T lhs, Flag rhs) noexcept
{
    return lhs == static_cast<std::uint32_t>(rhs);
}

template <std::unsigned_integral T>
constexpr std::uint32_t operator|(T lhs, Flag rhs) noexcept
{
    return static_cast<std::uint32_t>(lhs) | static_cast<std::uint32_t>(rhs);
}

constexpr std::uint32_t operator|(Flag lhs, Flag rhs) noexcept
{
    return static_cast<std::uint32_t>(lhs) | static_cast<std::uint32_t>(rhs);
}

template <std::unsigned_integral T>
constexpr std::uint32_t operator&(T lhs, Flag rhs) noexcept
{
    return static_cast<std::uint32_t>(lhs) & static_cast<std::uint32_t>(rhs);
}

constexpr std::uint32_t operator&(Flag lhs, Flag rhs) noexcept
{
    return static_cast<std::uint32_t>(lhs) & static_cast<std::uint32_t>(rhs);
}

FORCE_INLINE static std::uint64_t round_up(const std::uint64_t value,
                                           const std::uint64_t align) noexcept
{
    return ((value + align - 1) / align) * align;
};

struct ReserveRequest
{
    std::uint64_t size {};
    std::uint64_t alignment {};
    void *preferred_addr {};
    std::uint32_t alloc_flags {};
};

struct ReserveResponse
{
    void *base {};
    std::uint64_t size {};
    Error err {Error::INTERNAL};
};

[[nodiscard]] ReserveResponse reserve_memory(const ReserveRequest &req,
                                             std::uint64_t allocation_granularity) noexcept;

struct ReleaseRequest
{
    void *base {};
    std::uint64_t size {};
};

struct ReleaseResponse
{
    Error err {Error::INTERNAL};
};

[[nodiscard]] ReleaseResponse release_memory(const ReleaseRequest &req) noexcept;

struct CommitRequest
{
    void *base {};
    std::uint64_t offset {};
    std::uint64_t size {};
    std::uint32_t protection {};
    std::uint32_t alloc_flags {};
};

struct CommitResponse
{
    Error err {Error::INTERNAL};
};

using DecommitRequest  = CommitRequest;
using DecommitResponse = CommitResponse;

[[nodiscard]] CommitResponse commit_pages(const CommitRequest &req,
                                          std::uint64_t page_size) noexcept;
[[nodiscard]] DecommitResponse decommit_pages(const DecommitRequest &req) noexcept;

struct LargePageAllocationRequest
{
    std::uint64_t size {};
    std::uint32_t protection {};
    std::uint32_t flags {};
};

using LargePageAllocationResult = CommitResponse;

[[nodiscard]] LargePageAllocationResult
allocate_large_pages(const LargePageAllocationRequest &request) noexcept;

[[nodiscard]] LargePageAllocationResult
free_large_pages(const LargePageAllocationRequest &request) noexcept;

struct MapRequest
{
    void *base {};
    std::uint64_t offset {};
    std::uint64_t size {};
    std::uint32_t protection {};
};

struct MapResponse
{
    Error err {Error::INTERNAL};
    std::uintptr_t ptr {};
};

[[nodiscard]] MapResponse map(const MapRequest &req) noexcept;

struct UnmapRequest
{
    void *ptr {};
    std::uint64_t size {};
};

struct UnmapResponse
{
    Error err {Error::INTERNAL};
};

[[nodiscard]] UnmapResponse unmap(const UnmapRequest &req) noexcept;

struct AdviseRequest
{
    void *base {};
    std::uint64_t offset {};
    std::uint64_t size {};
    int advice {};
};

struct AdviseResponse
{
    Error err {Error::INTERNAL};
};

[[nodiscard]] AdviseResponse advise(const AdviseRequest &req) noexcept;

} // namespace noyxcore::memory::vas
