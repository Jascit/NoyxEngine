/* SPDX-License-Identifier: Apache-2.0 */

/**
 * @file    vas_wrapper_test.cpp
 * @brief   
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

NOYX_TEST(vas_reserve_memory, smoke_test) {
  uint32_t allocation_granularity, page_size, number_of_processor;
  noyxcore::platform::details::get_system_info(allocation_granularity, page_size, number_of_processor);
  ReserveRequest reserve_req;
  reserve_req.alloc_flags = static_cast<std::uint32_t>(Flag::None);
  reserve_req.alignment = allocation_granularity;
  reserve_req.size = 100*1024*1024;
  reserve_req.preferred_addr = nullptr;
  ReserveResponse reserve_resp = reserve_memory(reserve_req, allocation_granularity);
  NOYX_ASSERT_TRUE_MESSAGE(reserve_resp.err == Error::Ok, "reserve_memory failed");
  NOYX_ASSERT_TRUE_MESSAGE(reserve_resp.size == reserve_req.size, "reserve_memory returned different size");
  ReleaseRequest release_req;
  release_req.base = reserve_resp.base;
  release_req.size = reserve_resp.size;
  ReleaseResponse release_resp = release_memory(release_req);
  NOYX_ASSERT_TRUE_MESSAGE(release_resp.err == Error::Ok, "release_memory failed");

}