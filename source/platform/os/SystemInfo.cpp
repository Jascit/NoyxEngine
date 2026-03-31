/* SPDX-License-Identifier: Apache-2.0 */

/**
 * @file    SystemInfo.cpp
 * @brief   
 *
 * @author  Jascit (https://github.com/Jascit)
 * @date    30.03.2026
 *
 * @copyright Copyright (c) 2026 The Project Contributors
 */

#include <cstdio>
#include <platform/os/os_detect.h>
#include <platform/os/SystemInfo.hpp>
#include OS_DEPENDENCY_HEADER
#include <fstream>
// TODO: Linux/MacOS versions

using namespace noyxcore::platform;

void details::get_cpu_usage(double& cpu_usage) noexcept {
#ifdef NOYX_WINDOWS
  FILETIME idle_time, kernel_time, user_time;
  static ULARGE_INTEGER prev_idle = {}, prev_kernel = {}, prev_user = {};
  static bool initialized = false;

  if (!GetSystemTimes(&idle_time, &kernel_time, &user_time)) {
    cpu_usage = 0.0f;
    return;
  }

  ULARGE_INTEGER idle, kernel, user;
  idle.LowPart = idle_time.dwLowDateTime;
  idle.HighPart = idle_time.dwHighDateTime;

  kernel.LowPart = kernel_time.dwLowDateTime;
  kernel.HighPart = kernel_time.dwHighDateTime;

  user.LowPart = user_time.dwLowDateTime;
  user.HighPart = user_time.dwHighDateTime;

  if (!initialized) {
    prev_idle = idle;
    prev_kernel = kernel;
    prev_user = user;
    initialized = true;
    cpu_usage = 0.0f;
    return;
  }

  ULONGLONG idle_diff = idle.QuadPart - prev_idle.QuadPart;
  ULONGLONG kernel_diff = kernel.QuadPart - prev_kernel.QuadPart;
  ULONGLONG user_diff = user.QuadPart - prev_user.QuadPart;

  ULONGLONG total = kernel_diff + user_diff;

  prev_idle = idle;
  prev_kernel = kernel;
  prev_user = user;

  if (total == 0) {
    cpu_usage = 0.0f;
    return;
  }

  cpu_usage = 100.0 * (1.0 - static_cast<double>(idle_diff) / static_cast<double>(total));
#else
  static uint64_t prev_user = 0, prev_nice = 0, prev_system = 0, prev_idle = 0;
  static bool initialized = false;

  std::ifstream procs("/proc/stat");

  if (!procs.is_open()) {
    cpu_usage = 0.0f;
    return;
  }

  std::string label;
  uint64_t user, nice, system, idle;

  procs >> label >> user >> nice >> system >> idle;

  if (!initialized) {
    prev_idle = idle;
    prev_system = system;
    prev_nice = nice;
    prev_user = user;
    initialized = true;
    cpu_usage = 0.0f;
    return;
  }

  uint64_t user_diff = user - prev_user;
  uint64_t nice_diff = nice - prev_nice;
  uint64_t system_diff = system - prev_system;
  uint64_t idle_diff = idle - prev_idle;

  uint64_t total = user_diff + nice_diff + system_diff + idle_diff;

  if (total == 0) {
    cpu_usage = 0.0f;
    return;
  }

  prev_user = user;
  prev_nice = nice;
  prev_system = system;
  prev_idle = idle;

  cpu_usage = 100.0 * (1.0 - static_cast<double>(idle_diff) / static_cast<double>(total));
#endif
}

void details::get_memory_info(uint64_t& used_memory,
                              uint64_t& available_memory,
                              uint64_t& total_swap,
                              uint64_t& free_swap) noexcept {
#ifdef NOYX_WINDOWS
  MEMORYSTATUSEX memInfo;
  memInfo.dwLength = sizeof(memInfo);

  GlobalMemoryStatusEx(&memInfo);

  DWORDLONG total_phys = memInfo.ullTotalPageFile;
  DWORDLONG avail_phys = memInfo.ullAvailPhys;
  available_memory = avail_phys;
  used_memory = total_phys - avail_phys;

  total_swap = memInfo.ullTotalPageFile;
  free_swap = memInfo.ullAvailPageFile;
#else
#endif
}

void details::get_system_info(uint32_t& allocation_granularity,
                              uint32_t& page_size,
                              uint32_t& number_of_processors) noexcept {
#ifdef NOYX_WINDOWS
  SYSTEM_INFO system_info;

  GetSystemInfo(&system_info);

  allocation_granularity = system_info.dwAllocationGranularity;
  page_size = system_info.dwAllocationGranularity;
  number_of_processors = system_info.dwNumberOfProcessors;
#else
#endif
}

void SystemInfo::initialize() noexcept {
  details::get_system_info(allocation_granularity, page_size, number_of_processors);
  details::get_cpu_usage(cpu_usage);
  details::get_memory_info(used_memory, available_memory, total_swap, free_swap);
};

void SystemInfo::update_cpu_usage() noexcept {
  details::get_cpu_usage(cpu_usage);
}

void SystemInfo::update_ram_usage() noexcept {
  details::get_memory_info(used_memory, available_memory, total_swap, free_swap);
}
