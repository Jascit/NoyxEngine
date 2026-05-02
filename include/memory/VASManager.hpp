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
      if (offset_ + size > size_) return nullptr;
      void* ptr = static_cast<char*>(base_) + offset_;
      offset_ += size;
      active_allocations_++;
      return ptr;
    }

    void free_allocation() {
      active_allocations_--;
      if (active_allocations_ == 0) {
        offset_ = 0;
      }
    }


    FORCE_INLINE void* base() noexcept {
      return base_;
    }
    FORCE_INLINE uint64_t size() noexcept {
      return size_;
    }
    FORCE_INLINE uint64_t free_bytes() noexcept {
      return size_ - offset_;
    } //void* -> uint64_t

  private:
    void* base_;
    uint64_t size_;
    uint64_t offset_ = 0;
    uint64_t active_allocations_ = 0;
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

    region_handle reserve_vas(uint64_t size) {
      memory::vas::ReserveRequest req;
      req.size = size;    //TODO: round_up from vaswr.cpp
      memory::vas::ReserveResponse resp = memory::vas::reserve_memory(req, req.size); //TODO: alloc_granu vmesto size (?)

      if (resp.base == nullptr) return 0;
      region_handle new_handle;
      if (!m_free_handles.empty()) {
        new_handle = m_free_handles.back();
        m_free_handles.pop_back();
      } else {
        new_handle = m_current_region++;
      }

      Region new_region(resp.base, resp.size);
      m_free_map.emplace(new_handle, new_region);

      m_total_reserved += resp.size;
      return new_handle;
    }

    void release_vas(region_handle handle) {
      auto it = m_free_map.find(handle);
      if (it == m_free_map.end()) return;

      Region& region = it->second;
      memory::vas::ReleaseRequest req;
      req.base = region.base();
      req.size = region.size();
      memory::vas::ReleaseResponse resp = memory::vas::release_memory(req);

      m_free_map.erase(it);
      m_free_handles.push_back(handle);
      m_total_reserved -= req.size;
    }

    void* allocate_vas(uint64_t size) {
      if (size == 0) return nullptr;

      void* target_ptr = nullptr;
      region_handle target_handle = 0;

      for (auto& [handle, region] : m_free_map) {
        if (region.free_bytes() >= size) {
          target_ptr = region.allocate(size);
          target_handle = handle;
          break;
        }
      }

      if (target_ptr == nullptr) {
        target_handle = reserve_vas(size);
        if (target_handle == 0) return nullptr;

        auto it = m_free_map.find(target_handle);
        Region& new_region = it->second;
        target_ptr = new_region.allocate(size);
      }

      m_map[target_ptr] = {target_handle, size};
      m_total_allocated += size;

      return target_ptr;
    }

    void free_vas(void* addr) {
      auto it = m_map.find(addr);
      if (it == m_map.end()) return;

      region_handle handle = it->second.handle;
      uint64_t size = it->second.size;

      auto itr = m_free_map.find(handle);
      if (itr != m_free_map.end()) {
        Region& region = itr->second;
        region.free_allocation();
      }

      m_map.erase(it);
      m_total_allocated -= size;
    }

    void initialize(uint64_t page_size, uint64_t initial_reserve) {
      //TODO: page_size nachodit' gdeto
      auto it = reserve_vas(initial_reserve);
      if (it == 0) return;
    }

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
