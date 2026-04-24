/* SPDX-License-Identifier: Apache-2.0 */
/**
 * @file    vas_wrapper_test.cpp
 * @brief   Unit tests for Virtual Address Space (VAS) wrapper
 *
 * @author  Jascit (https://github.com/Jascit)
 * @date    06.04.2026
 *
 * @copyright Copyright (c) 2026 The Project Contributors
 */

#include <tests_details.h>
#include <memory/vas_wrapper.hpp>
#include <platform/os/SystemInfo.hpp>

using namespace noyxcore::memory::vas;

NOYX_TEST(vas_reserve_memory, invalid_args_test) {
  uint32_t allocation_granularity, page_size, number_of_processor;
  noyxcore::platform::details::get_system_info(allocation_granularity, page_size, number_of_processor);

  ReserveRequest req;
  req.alloc_flags = static_cast<std::uint32_t>(Flag::None);
  req.alignment = allocation_granularity;
  req.preferred_addr = nullptr;

  // Test 1: Zero size
  req.size = 0;
  ReserveResponse resp = reserve_memory(req, allocation_granularity);
  NOYX_ASSERT_TRUE_MESSAGE(resp.err == Error::InvalidArg, "reserve_memory should fail with size 0");

  // Test 2: LargePages flag in reserve (must be committed immediately, so reserve fails)
  req.size = 10 * 1024 * 1024;
  req.alloc_flags = static_cast<std::uint32_t>(Flag::LargePages);
  resp = reserve_memory(req, allocation_granularity);
  NOYX_ASSERT_TRUE_MESSAGE(resp.err == Error::InvalidArg, "reserve_memory should fail with LargePages flag");

  // Test 3: NonPaged flag in reserve
  req.alloc_flags = static_cast<std::uint32_t>(Flag::NonPaged);
  resp = reserve_memory(req, allocation_granularity);
  NOYX_ASSERT_TRUE_MESSAGE(resp.err == Error::InvalidArg, "reserve_memory should fail with NonPaged flag");

  // Test 4: Invalid alignment (less than allocation_granularity)
  req.alloc_flags = static_cast<std::uint32_t>(Flag::None);
  req.alignment = allocation_granularity / 2;
  resp = reserve_memory(req, allocation_granularity);
  NOYX_ASSERT_TRUE_MESSAGE(resp.err == Error::InvalidArg, "reserve_memory should fail if alignment < granularity");

  // Test 5: Invalid preferred_addr alignment
  req.alignment = allocation_granularity;
  req.preferred_addr = reinterpret_cast<void*>(static_cast<std::uintptr_t>(allocation_granularity) + 1);
  resp = reserve_memory(req, allocation_granularity);
  NOYX_ASSERT_TRUE_MESSAGE(resp.err == Error::InvalidArg, "reserve_memory should fail with unaligned preferred_addr");
}

NOYX_TEST(vas_commit_decommit_memory_test, partial_commit_test) {
  uint32_t allocation_granularity, page_size, number_of_processor;
  noyxcore::platform::details::get_system_info(allocation_granularity, page_size, number_of_processor);

  // Reserve a larger block
  ReserveRequest reserve_req;
  reserve_req.alloc_flags = static_cast<std::uint32_t>(Flag::None);
  reserve_req.alignment = allocation_granularity;
  reserve_req.size = 4 * allocation_granularity; // e.g., 4 * 64KB
  reserve_req.preferred_addr = nullptr;
  ReserveResponse reserve_resp = reserve_memory(reserve_req, allocation_granularity);
  NOYX_ASSERT_TRUE_MESSAGE(reserve_resp.err == Error::Ok, "reserve_memory failed");

  // Commit only a part of the reserved memory (second block)
  CommitRequest commit_req;
  commit_req.base = reserve_resp.base;
  commit_req.size = allocation_granularity;
  commit_req.offset = allocation_granularity; // Offset by 1 block
  commit_req.protection = Flag::ProtWrite | Flag::ProtRead;
  commit_req.alloc_flags = static_cast<std::uint32_t>(Flag::None);
  CommitResponse commit_resp = commit_pages(commit_req, page_size);
  NOYX_ASSERT_TRUE_MESSAGE(commit_resp.err == Error::Ok, "partial commit_pages failed");

  // Test write to the committed partial block
  auto ptr = reinterpret_cast<char*>(reserve_resp.base) + commit_req.offset;
  ptr[0] = 'N';
  ptr[1] = 'O';
  ptr[2] = 'Y';
  ptr[3] = 'X';

  // Decommit the partial block
  DecommitRequest decommit_req;
  decommit_req.base = reserve_resp.base;
  decommit_req.size = commit_req.size;
  decommit_req.offset = commit_req.offset;
  decommit_req.protection = static_cast<std::uint32_t>(Flag::None);
  decommit_req.alloc_flags = static_cast<std::uint32_t>(Flag::None);
  DecommitResponse decommit_resp = decommit_pages(decommit_req);
  NOYX_ASSERT_TRUE_MESSAGE(decommit_resp.err == Error::Ok, "partial decommit_pages failed");

  // Release the entire reserved region
  ReleaseRequest release_req;
  release_req.base = reserve_resp.base;
  release_req.size = reserve_resp.size;
  ReleaseResponse release_resp = release_memory(release_req);
  NOYX_ASSERT_TRUE_MESSAGE(release_resp.err == Error::Ok, "release_memory failed");
}

NOYX_TEST(vas_commit_decommit_memory_test, invalid_args_test) {
  uint32_t allocation_granularity, page_size, number_of_processor;
  noyxcore::platform::details::get_system_info(allocation_granularity, page_size, number_of_processor);

  CommitRequest commit_req;
  commit_req.base = reinterpret_cast<void*>(0x10000000); // Arbitrary address
  commit_req.offset = 0;
  commit_req.protection = static_cast<std::uint32_t>(Flag::ProtRead);
  commit_req.alloc_flags = static_cast<std::uint32_t>(Flag::None);

  // Test 1: Zero size commit
  commit_req.size = 0;
  CommitResponse commit_resp = commit_pages(commit_req, page_size);
  NOYX_ASSERT_TRUE_MESSAGE(commit_resp.err == Error::InvalidArg, "commit_pages should fail with size 0");

  // Test 2: LargePages flag with non-null base (in commit logic)
  commit_req.size = page_size;
  commit_req.alloc_flags = static_cast<std::uint32_t>(Flag::LargePages);
  commit_resp = commit_pages(commit_req, page_size);
  NOYX_ASSERT_TRUE_MESSAGE(commit_resp.err == Error::InvalidArg, "commit_pages should fail if LargePages is used with non-null base");

  DecommitRequest decommit_req;
  decommit_req.offset = 0;
  decommit_req.protection = static_cast<std::uint32_t>(Flag::None);
  decommit_req.alloc_flags = static_cast<std::uint32_t>(Flag::None);

  // Test 3: Decommit with null base
  decommit_req.base = nullptr;
  decommit_req.size = page_size;
  DecommitResponse decommit_resp = decommit_pages(decommit_req);
  NOYX_ASSERT_TRUE_MESSAGE(decommit_resp.err == Error::InvalidArg, "decommit_pages should fail with null base");

  // Test 4: Decommit with zero size
  decommit_req.base = reinterpret_cast<void*>(0x10000000);
  decommit_req.size = 0;
  decommit_resp = decommit_pages(decommit_req);
  NOYX_ASSERT_TRUE_MESSAGE(decommit_resp.err == Error::InvalidArg, "decommit_pages should fail with size 0");

  // Test 5: Decommit with LargePages flag
  decommit_req.size = page_size;
  decommit_req.alloc_flags = static_cast<std::uint32_t>(Flag::LargePages);
  decommit_resp = decommit_pages(decommit_req);
  NOYX_ASSERT_TRUE_MESSAGE(decommit_resp.err == Error::InvalidArg, "decommit_pages should fail with LargePages flag");
}