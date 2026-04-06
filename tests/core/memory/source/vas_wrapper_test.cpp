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
  ReserveRequest request;
  request.alloc_flags = static_cast<std::uint32_t>(Flag::None);
  request.alignment = allocation_granularity;
  request.size = 100*1024*1024;
  request.preferred_addr = nullptr;
  ReserveResponse resp = reserve_memory(request, allocation_granularity);
  NOYX_ASSERT_TRUE_MESSAGE(resp.err == Error::Ok, "reserve_memory failed");
  NOYX_ASSERT_TRUE_MESSAGE(resp.size == request.size, "reserve_memory returned different size");
}