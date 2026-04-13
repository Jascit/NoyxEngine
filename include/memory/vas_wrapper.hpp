/* SPDX-License-Identifier: Apache-2.0 */

/**
 * @file     vas_wrapper.h
 * @brief
 *
 * @author   Jascit (https://github.com/Jascit)
 * @date     04.02.2026
 *
 * @copyright Copyright (c) 2026 The Project Contributors
 */
#pragma once

#include <cstdint>
#include <cstddef>
#include <concepts>

namespace noyxcore::memory::vas {
  enum class Error : std::uint8_t {
    Ok = 0,
    InvalidArg,
    InvalidAddress,
    OOM,
    NotCommitted,
    AlreadyCommitted,
    NoMapping,
    Permission,
    Unsupported,
    Platform,
    Internal
  };

  enum class Flag : std::uint32_t {
    None = 0u,
    ProtNone = 1u << 0,
    ProtRead = 1u << 1,
    ProtWrite = 1u << 2,
    ProtExec = 1u << 3,
    FixedAddress = 1u << 8,
    PreferAddress = 1u << 9,
    LargePages = 1u << 16,
    Page64K = 1u << 17,
    NonPaged = 1u << 18
  };

  template<std::unsigned_integral T>
  constexpr bool operator==(T lhs, Flag rhs) noexcept {
    return lhs == static_cast<std::uint32_t>(rhs);
  }

  template<std::unsigned_integral T>
  constexpr std::uint32_t operator|(T lhs, Flag rhs) noexcept {
    return static_cast<std::uint32_t>(lhs) | static_cast<std::uint32_t>(rhs);
  }

  constexpr std::uint32_t operator|(Flag lhs, Flag rhs) noexcept {
    return static_cast<std::uint32_t>(lhs) | static_cast<std::uint32_t>(rhs);
  }

  template<std::unsigned_integral T>
  constexpr std::uint32_t operator&(T lhs, Flag rhs) noexcept {
    return static_cast<std::uint32_t>(lhs) & static_cast<std::uint32_t>(rhs);
  }

  constexpr std::uint32_t operator&(Flag lhs, Flag rhs) noexcept {
    return static_cast<std::uint32_t>(lhs) & static_cast<std::uint32_t>(rhs);
  }

  struct ReserveRequest {
    std::uint64_t size{};
    std::uint64_t alignment{};
    void* preferred_addr{};
    std::uint32_t alloc_flags{};
  };

  struct ReserveResponse {
    void* base{};
    std::uint64_t size{};
    Error err{Error::Internal};
  };

  [[nodiscard]] ReserveResponse reserve_memory(const ReserveRequest& req, std::uint64_t allocation_granularity) noexcept;

  struct ReleaseRequest {
    void* base{};
    std::uint64_t size{};
  };

  struct ReleaseResponse {
    Error err{Error::Internal};
  };

  [[nodiscard]] ReleaseResponse release_memory(const ReleaseRequest& req) noexcept;

  struct CommitRequest {
    void* base{};
    std::uint64_t offset{};
    std::uint64_t size{};
    std::uint32_t protection{};
    std::uint32_t alloc_flags{};
  };

  struct CommitResponse {
    Error err{Error::Internal};
  };

  using DecommitRequest = CommitRequest;
  using DecommitResponse = CommitResponse;

  [[nodiscard]] CommitResponse commit_pages(const CommitRequest& req, std::uint64_t page_size) noexcept;
  [[nodiscard]] DecommitResponse decommit_pages(const DecommitRequest& req) noexcept;

  struct LargePageAllocationRequest {
    std::uint64_t size{};
    std::uint32_t protection{};
    std::uint32_t flags{};
  };

  using LargePageAllocationResult = CommitResponse;

  [[nodiscard]] LargePageAllocationResult allocate_large_pages(const LargePageAllocationRequest& request) noexcept;

  [[nodiscard]] LargePageAllocationResult free_large_pages(const LargePageAllocationRequest& request) noexcept;

  struct MapRequest {
    void* base{};
    std::uint64_t offset{};
    std::uint64_t size{};
    std::uint32_t protection{};
  };

  struct MapResponse {
    Error err{Error::Internal};
    std::uintptr_t ptr{};
  };

  [[nodiscard]] MapResponse map(const MapRequest& req) noexcept;

  struct UnmapRequest {
    void* ptr{};
    std::uint64_t size{};
  };

  struct UnmapResponse {
    Error err{Error::Internal};
  };

  [[nodiscard]] UnmapResponse unmap(const UnmapRequest& req) noexcept;

  struct AdviseRequest {
    void* base{};
    std::uint64_t offset{};
    std::uint64_t size{};
    int advice{};
  };

  struct AdviseResponse {
    Error err{Error::Internal};
  };

  [[nodiscard]] AdviseResponse advise(const AdviseRequest& req) noexcept;

} // namespace noyx::vas
