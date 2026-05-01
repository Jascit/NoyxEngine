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
#include <vector>


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
      if (!free_handles_.empty()) {
        new_handle = free_handles_.back();
        free_handles_.pop_back();
      } else {
        new_handle = current_region_++;
      }

      Region new_region(resp.base, resp.size);
      free_map_.emplace(new_handle, new_region);

      total_reserved += resp.size;
      return new_handle;
    }

    void release_vas(region_handle handle) {
      auto it = free_map_.find(handle);
      if (it == free_map_.end()) return;

      Region& region = it->second;
      memory::vas::ReleaseRequest req;
      req.base = region.base();
      req.size = region.size();
      memory::vas::ReleaseResponse resp = memory::vas::release_memory(req);

      free_map_.erase(it);
      free_handles_.push_back(handle);
      total_reserved -= req.size;
    }

    void* allocate_vas(uint64_t size) {
      if (size == 0) return nullptr;

      void* target_ptr = nullptr;
      region_handle target_handle = 0;

      for (auto& [handle, region] : free_map_) {
        if (region.free_bytes() >= size) {
          target_ptr = region.allocate(size);
          target_handle = handle;
          break;
        }
      }

      if (target_ptr == nullptr) {
        target_handle = reserve_vas(size);
        if (target_handle == 0) return nullptr;

        auto it = free_map_.find(target_handle);
        Region& new_region = it->second;
        target_ptr = new_region.allocate(size);
      }

      memory::vas::CommitRequest commit;
      commit.base = target_ptr;
      commit.size = size;
      memory::vas::CommitResponse commit_response = memory::vas::commit_pages(commit, commit.size);

      map_.emplace(target_ptr, AllocationRecord{target_handle, size});
      total_allocated += size;

      return target_ptr;
    }

    void free_vas(void* addr) {
      auto it = map_.find(addr);
      if (it == map_.end()) return;
      void* ptr = it->first;
      region_handle handle = it->second.handle;

      auto itr = free_map_.find(handle);
      if (itr == free_map_.end()) return;
      Region& region = itr->second;

      memory::vas::DecommitRequest req;
      req.base = ptr;
      req.size = it->second.size;
      memory::vas::DecommitResponse resp = memory::vas::decommit_pages(req);
      region.free_allocation();

      map_.erase(it);
      total_allocated -= req.size;
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
    std::unordered_map<void*, AllocationRecord> map_; //allocated
    std::map<region_handle, Region> free_map_; //reserved

    std::vector<region_handle> free_handles_;
    uint64_t current_region_ = 1;

    //metrics
    uint64_t total_reserved = 0;
    uint64_t total_allocated = 0;
  };
}
