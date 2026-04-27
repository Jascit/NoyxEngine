/* SPDX-License-Identifier: Apache-2.0 */

/**
 * @file    SystemInfo.hpp
 * @brief   
 *
 * @date    30.03.2026
 *
 * @copyright Copyright (c) 2026 The Project Contributors
 */
#pragma once
#include <cstdint>

#include "platform/debug.h"

namespace noyxcore::platform {
  namespace details {
    void get_cpu_usage(double& cpu_usage) noexcept;
    void get_memory_info(uint64_t& used_memory,
                         uint64_t& available_memory,
                         uint64_t& total_swap,
                         uint64_t& free_swap) noexcept;

    void get_system_info(uint32_t& allocation_granularity,
                         uint32_t& page_size,
                         uint32_t& number_of_processors) noexcept;
  }

  // The data is modified only once per frame and only by the main thread
  class SystemInfo {
  public:
    SystemInfo() = delete;
    SystemInfo(SystemInfo&) = delete;
    SystemInfo(SystemInfo&&) = delete;

    FORCE_INLINE static void initialize() noexcept {
        details::get_system_info(allocation_granularity, page_size, number_of_processors);
        details::get_cpu_usage(cpu_usage);
        details::get_memory_info(used_memory, available_memory, total_swap, free_swap);
    };

    FORCE_INLINE static void update_ram_usage() noexcept {
        details::get_memory_info(used_memory, available_memory, total_swap, free_swap);
    };

    FORCE_INLINE static void update_cpu_usage() noexcept {
        details::get_cpu_usage(cpu_usage);
    };

  public:
    inline static uint32_t allocation_granularity = 0;
    inline static uint32_t page_size = 0;

    inline static uint32_t number_of_processors = 0;
    inline static double cpu_usage = 0;

    inline static uint32_t large_pages_size = 0;

    inline static uint64_t used_memory = 0;
    inline static uint64_t available_memory = 0;
    inline static uint64_t total_swap = 0;
    inline static uint64_t free_swap = 0;
  };
}
