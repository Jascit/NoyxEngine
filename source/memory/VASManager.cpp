/* SPDX-License-Identifier: Apache-2.0 */

/**
 * @file     VASManager.hpp
 * @brief    VAS manager
 *
 * @date     13.02.2026
 *
 * @copyright Copyright (c) 2026 The Project Contributors
 */

#include <memory/VASManager.hpp>
#include <memory/vas_wrapper.hpp>
#include <platform/os/SystemInfo.hpp>
using namespace noyxcore::memory;

Region::Region(void* base, uint64_t size) noexcept : m_base(base), size_(size), m_offset(0) {}

region_handle VASManager::reserve_vas(uint64_t size) {
  vas::ReserveRequest request = {
    .size = size,
    .alignment = 0,
    .alloc_flags = static_cast<std::uint32_t>(vas::Flag::NONE),
    .preferred_addr = nullptr
  };

  vas::ReserveResponse resp = vas::reserve_memory(request, platform::SystemInfo::allocation_granularity);
  if (resp.err != vas::Error::OK) {
      //smth
  }
  region_handle index = m_current_region++;
  Region new_region(resp.base, resp.size);
  m_map.insert(std::make_pair<Region, region_handle>(new_region, index));
}

void VASManager::release_vas(region_handle) {}
void* VASManager::allocate_vas(uint64_t size) {}
void VASManager::free_vas(void*) {}
void VASManager::initialize(uint64_t page_size, uint64_t initial_reserve) {}
