/* SPDX-License-Identifier: Apache-2.0 */

/**
 * @file    SystemInfo_test.cpp
 * @brief   
 *
 * @author  Jascit (https://github.com/Jascit)
 * @date    31.03.2026
 *
 * @copyright Copyright (c) 2026 The Project Contributors
 */

#include <tests_details.h>
#include <platform/os/SystemInfo.hpp>
#include <iostream>
#include <chrono>
#include <thread>

using namespace noyxcore::platform;

NOYX_TEST(system_info, smoke_test) {
  SystemInfo::initialize();
  std::cout << std::endl << "allocation_granularity: " << SystemInfo::allocation_granularity << std::endl;
  std::cout << "page_size: " << SystemInfo::page_size << std::endl;
  std::cout << "number_of_processors: " << SystemInfo::number_of_processors << std::endl;
  std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
  auto end = start + std::chrono::milliseconds(1000);
  while (end >= start) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::chrono::steady_clock::time_point check = std::chrono::steady_clock::now();
    if (std::chrono::duration_cast<std::chrono::milliseconds>(check - start).count() >= 100) {
      SystemInfo::update_cpu_usage();
      SystemInfo::update_ram_usage();
      start = std::chrono::steady_clock::now();

      std::cout << "cpu_usage: " << SystemInfo::cpu_usage << std::endl;
      std::cout << "used_memory: " << SystemInfo::used_memory << std::endl;
      std::cout << "available_memory: " << SystemInfo::available_memory << std::endl;
      std::cout << "total_swap: " << SystemInfo::total_swap << std::endl;
      std::cout << "free_swap: " << SystemInfo::free_swap << std::endl;
    }
  }
}