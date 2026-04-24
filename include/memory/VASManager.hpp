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

namespace noyxcore::memory {
  using region_handle = uint64_t;

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
    const uint64_t page_size = 0;
    //metrics
  };
}
