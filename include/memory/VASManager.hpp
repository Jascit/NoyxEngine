/* SPDX-License-Identifier: Apache-2.0 */

/**
 * @file    VASManager.hpp
 * @brief   Virtual Address Space Manager
 *
 * @date    2026-02-13
 *
 * @copyright
 * Copyright (c) 2026 Project Contributors
 */

#pragma once
#include <cinttypes>
#include <unordered_map>
#include <map>
#include <list>


#include "vas_wrapper.hpp"
#include "platform/debug.h"
#include "utils/File.hpp"

namespace noyxcore::memory {
  using region_handle = uint64_t;

  class Region {
  public:
    Region(void* base, uint64_t size) noexcept;
    ~Region() = default;

    void* allocate(uint64_t size) noexcept {
      if (m_offset + size > m_size) return nullptr;
      void* ptr = static_cast<char*>(m_base) + m_offset;
      m_offset += size;
      m_active_allocations++;
      return ptr;
    }

    void free_allocation() {
      m_active_allocations--;
      if (m_active_allocations == 0) {
        m_offset = 0;
      }
    }


    FORCE_INLINE void* base() noexcept {
      return m_base;
    }
    FORCE_INLINE uint64_t size() noexcept {
      return m_size;
    }
    FORCE_INLINE uint64_t free_bytes() noexcept {
      return m_size - m_offset;
    } //void* -> uint64_t

  private:
    void* m_base;
    uint64_t m_size;
    uint64_t m_offset = 0;
    uint64_t m_active_allocations = 0;
  };

  struct AllocationRecord { //FOR MAP
    region_handle handle;
    uint64_t size;
  };

  class VASManager {
  public:
    VASManager(const VASManager&) = delete;
    VASManager(VASManager&&) = delete;
    VASManager& operator=(const VASManager&) = delete;
    VASManager& operator=(VASManager&&) = delete;

    region_handle reserve_vas(uint64_t size);
    void release_vas(region_handle handle);
    void* allocate_vas(uint64_t size);
    void free_vas(void* addr);
    void initialize(uint64_t page_size, uint64_t initial_reserve);

    static VASManager& instance() {
      static VASManager instance;
      return instance;
    }

  private:
    VASManager() = default;

  private:
    //data
    std::unordered_map<void*, AllocationRecord> m_map; //allocated
    std::map<region_handle, Region> m_free_map; //reserved

    std::list<region_handle> m_free_handles;
    uint64_t m_current_region = 1;

    //metrics
    uint64_t m_total_reserved = 0;
    uint64_t m_total_allocated = 0;
  };
}
