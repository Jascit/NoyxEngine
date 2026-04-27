/* SPDX-License-Identifier: Apache-2.0 */

/**
 * @file    VASManager.hpp
 * @brief   Virtual Address Space Manager
 *
 * @author  Jascit (https://github.com/Jascit)
 * @date    2026-02-13
 *
 * @copyright
 * Copyright (c) 2026 Project Contributors
 */

#pragma once
#include <cinttypes>
#include <unordered_map>
#include <hash_map>

#include "platform/debug.h"

namespace noyxcore::memory {
  using region_handle = uint64_t;

  class Region {
  public:
    Region(void* base, uint64_t size) noexcept;
    ~Region() = default;

    void* allocate(uint64_t size) noexcept;
    FORCE_INLINE void* base() noexcept;
    FORCE_INLINE void* free_bytes() noexcept;

  private:
    void* base_;
    uint64_t size_;
    uint64_t offset_;
  };

  class VASManager {
  public:
    VASManager(const VASManager&) = delete;
    VASManager(VASManager&&) = delete;
    VASManager& operator=(const VASManager&) = delete;
    VASManager& operator=(VASManager&&) = delete;

    region_handle reserve_vas(uint64_t size);
    void release_vas(region_handle);
    void* allocate_vas(uint64_t size);
    void free_vas(void*);
    void initialize(uint64_t page_size, uint64_t initial_reserve);

    static VASManager& instance() {
      static VASManager instance;
      return instance;
    }

  private:
    VASManager() = default;

  private:
    //data
    std::unordered_map<void*, region_handle> map_;
    std::hash_map<region_handle, Region> free_map_;

    std::vector<region_handle> free_regions_;
    uint64_t current_region_;

    //metrics
    uint64_t total_reserved;
    uint64_t total_allocated;
  };
}
