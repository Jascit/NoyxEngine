/* SPDX-License-Identifier: Apache-2.0 */

/**
 * @file    SystemInfo.cpp
 * @brief   
 *
 * @date    30.03.2026
 *
 * @copyright Copyright (c) 2026 The Project Contributors
 */

#include <cstdio>
#include <platform/os/os_detect.h>
#include <platform/os/SystemInfo.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>

#ifdef NOYX_WINDOWS
#include <windows.h>
#elif defined(NOYX_LINUX) || defined(NOYX_APPLE)
#include <unistd.h>
#include <errno.h>
#endif

// TODO: Linux/MacOS versions

using namespace noyxcore::platform;

void details::get_cpu_usage(double& cpu_usage) noexcept {
#if defined(NOYX_WINDOWS)
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
#elif defined(NOYX_LINUX)
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
#elif defined(NOYX_APPLE)
#endif
}

void details::get_memory_info(uint64_t& used_memory,
                              uint64_t& available_memory,
                              uint64_t& total_swap,
                              uint64_t& free_swap) noexcept {
#ifdef NOYX_WINDOWS
  MEMORYSTATUSEX mem_info;
  mem_info.dwLength = sizeof(mem_info);

  GlobalMemoryStatusEx(&mem_info);

  DWORDLONG total_phys = mem_info.ullTotalPageFile;
  available_memory = mem_info.ullAvailPhys;
  used_memory = total_phys - available_memory;

  total_swap = mem_info.ullTotalPageFile;
  free_swap = mem_info.ullAvailPageFile;
#elifdef NOYX_LINUX
  using field_name = std::string;
  using data = std::uint64_t;

  std::ifstream file("/proc/meminfo");
  if (!file) return;

  std::unordered_map<field_name, data> meminfo(60);
  std::string line;

  while (std::getline(file, line)) {
    std::istringstream iss(line);
    field_name label;
    data value;
    char unit_buffer[2];

    if (iss >> label >> value >> unit_buffer) {
      if (!label.empty() && label.back() == ':')
        label.pop_back();

      meminfo[label] = value;
    }
  }

  data total_phys = meminfo["MemTotal"];
  available_memory = meminfo.count("MemAvailable") ? meminfo["MemAvailable"] : meminfo["MemFree"];
  used_memory = total_phys - available_memory;
  total_swap = meminfo["SwapTotal"];
  free_swap = meminfo["SwapFree"];
#endif
}

void details::get_system_info(uint32_t& allocation_granularity,
                              uint32_t& page_size,
                              uint32_t& number_of_processors) noexcept {
#ifdef NOYX_WINDOWS
  SYSTEM_INFO system_info;

  GetSystemInfo(&system_info);

  allocation_granularity = system_info.dwAllocationGranularity;
  page_size = system_info.dwPageSize;
  number_of_processors = system_info.dwNumberOfProcessors;
#elif defined(NOYX_APPLE) || defined(NOYX_LINUX)
    page_size = static_cast<uint32_t>(sysconf(_SC_PAGE_SIZE));
    number_of_processors = static_cast<uint32_t>(sysconf(_SC_NPROCESSORS_CONF));
    allocation_granularity = page_size;
#endif
}