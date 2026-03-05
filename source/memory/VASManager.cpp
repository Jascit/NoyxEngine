/* SPDX-License-Identifier: Apache-2.0 */
/*
 * \file   VASManager.hpp
 * \brief  VAS manager
 *
 * Copyright (c) 2026 Project Contributors
 * \author Jascit<https://github.com/Jascit>
 * \date   13.02.2026
 */

#include <filesystem>
#include <memory/VASManager.hpp>
#include <platform/os/vas_wrapper.h>

#include "platform/debug.h"

using namespace noyxcore::memory;

static FORCE_INLINE uint64_t align_up(uint64_t size, uint64_t alignment) noexcept {
  if (alignment == 0) return size;
  uint64_t r = size % alignment;
  return r == 0 ? size : size + (alignment - r);
}

static FORCE_INLINE uint64_t padding_for(uint64_t size, uint64_t alignment) noexcept {
  if (alignment == 0) return 0;
  uint64_t r = size % alignment;
  return r == 0 ? 0 : alignment - r;
}

uint32_t VASManager::reserveBlock(uint64_t size, uint64_t alignment, uint32_t flags) noexcept {
  vaw_reserve_req_t reserve_req;
  reserve_req.size = size;
  reserve_req.alignment = alignment;
  reserve_req.alloc_flags = flags;

  vaw_reserve_resp_t reserve_resp = vaw_reserve_memory(&reserve_req, page_granularity_);

  if (reserve_resp.err != VAW_OK) {}

  uint32_t id = handle_table_.allocate_index();
  auto [block_it, block_insert_succeeded] = blocks_by_id_.emplace(
    std::pair(id, BlockRecord(reserve_resp.base, reserve_resp.size, 0, id)));
  if (!block_insert_succeeded) {
    vaw_release_req_t release_req;
    release_req.base = reserve_resp.base;
    release_req.size = reserve_resp.size;
    vaw_release_memory(&release_req);
    handle_table_.free_index(id);
    return 0;
  }

  auto opt_start_it  = start_map_.insert(reserve_resp.base, id);
  if (!opt_start_it.has_value()) {
    vaw_release_req_t release_req;
    release_req.base = reserve_resp.base;
    release_req.size = reserve_resp.size;
    vaw_release_memory(&release_req);
    handle_table_.free_index(id);
    blocks_by_id_.erase(block_it);
    return 0;
  }
  const auto& start_it = opt_start_it.value();

  FreeExtent extent;
  extent.size = reserve_resp.size;
  extent.start = reserve_resp.base;
  extent.block_id = id;
  auto opt_extent_it = largest_free_set_.insert(extent);

  if (!opt_extent_it.has_value()) {
    vaw_release_req_t release_req;
    release_req.base = reserve_resp.base;
    release_req.size = reserve_resp.size;
    vaw_release_memory(&release_req);
    handle_table_.free_index(id);
    blocks_by_id_.erase(block_it);
    start_map_.erase(start_it);
    return 0;
  }

  total_reserved_.fetch_add(reserve_resp.size, std::memory_order_relaxed);
  return id;
}

void* VASManager::try_allocate_in_block_(BlockRecord& target_block, uint64_t aligned_size, uint64_t alignment) {
  //TODO: block_lock lock

  uint64_t old_offset = target_block.current_offset.load(std::memory_order_acquire);
  auto base_addr = reinterpret_cast<uintptr_t>(target_block.base);
  uint64_t pad = padding_for(base_addr + old_offset, alignment);

  if (old_offset > UINT64_MAX - pad) return nullptr;
  uint64_t needed = pad;
  if (aligned_size > UINT64_MAX - needed) return nullptr;
  needed += aligned_size;

  FreeExtent prev_extent;
  prev_extent.start = reinterpret_cast<void*>(static_cast<uint8_t*>(target_block.base) + old_offset);
  prev_extent.size = target_block.size - old_offset;
  prev_extent.block_id = target_block.block_id;
  largest_free_set_.erase(prev_extent);

  uint64_t new_offset = old_offset + needed;
  target_block.current_offset.store(new_offset, std::memory_order_relaxed);
  target_block.ref_count.fetch_add(1, std::memory_order_relaxed);

  //TODO: block_look unlock

  if (target_block.size > new_offset) {
    FreeExtent new_largest_extent;
    new_largest_extent.size = target_block.size - new_offset;
    new_largest_extent.start = reinterpret_cast<void*>(static_cast<uint8_t*>(target_block.base) + new_offset);
    new_largest_extent.block_id = target_block.block_id;
    largest_free_set_.insert(new_largest_extent);
  }

  void* res = static_cast<uint8_t*>(target_block.base) + old_offset + pad;
  return res;
}

void* VASManager::allocate(void* block_hint, uint64_t size, uint64_t alignment, uint32_t flags,
                           bool auto_reserve) noexcept {
  if (size == 0) { return nullptr; }

  uint64_t aligned_size = align_up(size, alignment);

  if (block_hint) {
    auto opt_id = start_map_.findByStart(block_hint);
    if (opt_id.has_value()) {
      const uint32_t id = opt_id.value();
      auto block_it = blocks_by_id_.find(id);
      if (block_it != blocks_by_id_.end()) {
        void* res = try_allocate_in_block_(block_it->second, aligned_size, alignment);
        if (res) {
          total_allocated_.fetch_add(aligned_size, std::memory_order_release);
          return res;
        }
      }
    }
  }

  // 2. block_hint == nullptr or couldn't find block, try find_fit

  {
    auto opt_extent_it = largest_free_set_.findFit(aligned_size, alignment);
    if (opt_extent_it.has_value()) {
      const FreeExtent chosen = opt_extent_it.value()->second;
      auto block_it = blocks_by_id_.find(chosen.block_id);
      if (block_it != blocks_by_id_.end()) {
        void* res = try_allocate_in_block_(block_it->second, aligned_size, alignment);
        if (res) {
          total_allocated_.fetch_add(aligned_size, std::memory_order_release);
          return res;
        }
      } else {
        largest_free_set_.erase(opt_extent_it.value());
      }
    }
  }

  // 3. try auto_reserve and allocate
  if (auto_reserve) {
    // release helper
    auto vaw_release_mem = [&] (void* base, uint64_t release_size) noexcept -> void {
      vaw_release_req_t release_req;
      release_req.size = release_size;
      release_req.base = base;
      vaw_release_resp_t release_resp = vaw_release_memory(&release_req);
      if (release_resp.err != VAW_OK) {
        /*can't do shit*/
      }
    };

    uint64_t n = (default_block_size_ + aligned_size - 1) / aligned_size; // ceil(default_size / aligned_size)
    if (n == 0) n = 1;
    uint64_t reserve_size = n * aligned_size;

    vaw_reserve_req_t reserve_req;
    reserve_req.size = reserve_size;
    reserve_req.alignment = alignment;
    reserve_req.alloc_flags = common_flags_;

    vaw_reserve_resp_t reserve_resp = vaw_reserve_memory(&reserve_req, page_granularity_);
    if (reserve_resp.err != 0 || reserve_resp.base == nullptr || reserve_resp.size < aligned_size) {
      // reserve failed
      return nullptr;
    }

    // if base_addr is not aligned
    auto base_addr = reinterpret_cast<uintptr_t>(reserve_resp.base);
    uint64_t pad = padding_for(base_addr, alignment);
    if (reserve_resp.size < aligned_size + pad) {
      vaw_release_mem(reserve_resp.base, reserve_resp.size);
      return nullptr;
    }

    uint32_t id = handle_table_.allocate_index();
    auto [block_insert_it, block_insert_succeeded] = blocks_by_id_.emplace(
      std::pair(id, BlockRecord(reserve_resp.base, reserve_resp.size, 0, id)));
    if (!block_insert_succeeded) {
      vaw_release_mem(reserve_resp.base, reserve_resp.size);
      return nullptr;
    }

    BlockRecord& target_block = block_insert_it->second;
    uint64_t new_offset = pad + aligned_size;
    target_block.current_offset.store(new_offset, std::memory_order_release);
    target_block.generation.fetch_add(1, std::memory_order_release);

    auto opt_start_it  = start_map_.insert(reserve_resp.base, id);
    if (!opt_start_it.has_value()) {
      blocks_by_id_.erase(block_insert_it);
      handle_table_.free_index(id);
      vaw_release_mem(reserve_resp.base, reserve_resp.size);
      return nullptr;
    }
    const auto& start_it = opt_start_it.value();
    if (target_block.size > new_offset) {
      FreeExtent new_largest_extent;
      new_largest_extent.size = target_block.size - new_offset;
      new_largest_extent.start = reinterpret_cast<void*>(base_addr + new_offset);
      new_largest_extent.block_id = target_block.block_id;
      auto opt_extent_it = largest_free_set_.insert(new_largest_extent);
      if (!opt_extent_it.has_value()) {
        blocks_by_id_.erase(block_insert_it);
        handle_table_.free_index(id);
        start_map_.erase(start_it);
        vaw_release_mem(reserve_resp.base, reserve_resp.size);
        return nullptr;
      }
    }

    total_allocated_.fetch_add(aligned_size, std::memory_order_release);
    void* res = static_cast<uint8_t*>(target_block.base) + pad;
    return res;
  }
  // 4. nothing worked; failing
  return nullptr;
}

bool VASManager::release_block_(const std::unordered_map<uint32_t, BlockRecord>::iterator& block_it) {
  //TODO: block_lock lock
  BlockRecord& target_block = block_it->second;

  FreeExtent extent_to_erase;
  extent_to_erase.block_id = target_block.block_id;
  extent_to_erase.size = target_block.size - target_block.current_offset.load(std::memory_order_acquire);
  extent_to_erase.start = reinterpret_cast<void*>(
    reinterpret_cast<uint64_t>(target_block.base) + target_block.current_offset.load(std::memory_order_acquire));
  largest_free_set_.erase(extent_to_erase);

  start_map_.erase(target_block.base);

  vaw_release_req_t release_req;
  release_req.size = target_block.size;
  release_req.base = target_block.base;
  vaw_release_memory(&release_req);

  total_released_.fetch_add(target_block.size, std::memory_order_release);
  blocks_by_id_.erase(block_it);
  handle_table_.free_index(target_block.block_id);

  //TODO: block_lock unlock
  return true;
}

bool VASManager::freeByAddr(void* addr, uint64_t size) noexcept {
  if (addr == nullptr || size == 0) return false;

  auto opt_block = start_map_.predecessor(addr);
  if (!opt_block.has_value()) {
    return false;
  }

  auto [base_addr, block_id] = opt_block.value();
  auto block_it = blocks_by_id_.find(block_id);
  if (block_it == blocks_by_id_.end()) {
    //TODO:
    return false;
  }
  uint64_t current_refs = block_it->second.ref_count.fetch_sub(1, std::memory_order_release) - 1;
  if (current_refs == 0) {
    release_block_(block_it);
  }
  total_free_.fetch_add(size, std::memory_order_release);
  return true;
}

VASManager::VASManager(size_t common_flags, size_t default_block_size) noexcept : default_block_size_(
    default_block_size), common_flags_(common_flags), page_granularity_(page_size()), total_reserved_{0},
  total_allocated_{0}, total_free_{0}, total_released_{0} {}

std::optional<uint32_t> VASManager::findByAddr(void* addr) const noexcept {
  return start_map_.findByStart(addr);
}

bool VASManager::releaseBlock(uint32_t block_id, bool force) noexcept {
  auto block_it = blocks_by_id_.find(block_id);
  if (block_it == blocks_by_id_.end()) return false;

  if (force) {
    return release_block_(block_it);
  } else {
    if (block_it->second.ref_count.load(std::memory_order_acquire) == 0) {
      return release_block_(block_it);
    }
    return false;
  }
}

VASManager::Statistics VASManager::statistics() const noexcept {
  Statistics statistics = {
    .total_released = total_released_.load(std::memory_order_acquire),
    .total_reserved = total_reserved_.load(std::memory_order_acquire),
    .total_allocated = total_allocated_.load(std::memory_order_acquire),
    .total_freed = total_free_.load(std::memory_order_acquire)
  };
  return statistics;
}

VASManager::~VASManager() {}

bool StartMap::erase(const map_type::iterator& map_it) {
  if (map_it == predecessor_map_.end()) return false;
  // TODO: mutex
  auto lookup_it = lookup_map_.find(map_it->first);
  if (lookup_it != lookup_map_.end()) {
    lookup_map_.erase(lookup_it);
  }
  predecessor_map_.erase(map_it);
  return true;
}

bool StartMap::erase(void* start) {
  // TODO: mutex
  auto hash_it = lookup_map_.find(reinterpret_cast<uintptr_t>(start));
  if (hash_it != lookup_map_.end()) {
    lookup_map_.erase(hash_it);
  }
  auto pred_it = predecessor_map_.find(reinterpret_cast<uintptr_t>(start));
  if (pred_it != predecessor_map_.end()) {
    predecessor_map_.erase(pred_it);
  }
  return true;
}

std::optional<uint32_t> StartMap::findByStart(void* start) const {
  // TODO: mutex
  auto hash_it = lookup_map_.find(reinterpret_cast<uintptr_t>(start));
  if (hash_it == lookup_map_.end()) {
    return std::nullopt;
  }
  return {hash_it->second};
}

std::optional<std::pair<void*, uint32_t>> StartMap::predecessor(void* addr) {
  // TODO: mutex
  auto map_it = predecessor_map_.upper_bound(reinterpret_cast<uintptr_t>(addr));
  if (map_it == predecessor_map_.end()) {
    return std::nullopt;
  }
  return {std::pair(reinterpret_cast<void*>(map_it->first), map_it->second)};
}

std::optional<std::map<uintptr_t, uint32_t>::iterator> StartMap::emplace(void* start, uint32_t id) {
  // TODO: mutex
  auto lookup_it = lookup_map_.emplace(
    std::pair<uintptr_t, uint32_t>(reinterpret_cast<uintptr_t>(start), id));
  if (lookup_it == lookup_map_.end()) {
    return std::nullopt;
  }

  auto [predecessor_it, predecessor_insert_succeeded] = predecessor_map_.emplace(
    std::pair<uintptr_t, uint32_t>(reinterpret_cast<uintptr_t>(start), id));

  if (!predecessor_insert_succeeded) {
    lookup_map_.erase(lookup_it);
    return std::nullopt;
  }

  return {predecessor_it};
}

std::optional<FreeSet::map_type::iterator> FreeSet::findFit(uint64_t size, uint64_t alignment) {
  if (alignment == 0 || (alignment & (alignment - 1)) != 0) {
    return std::nullopt; // Alignment muss Power-of-Two
  }

  auto it = largest_extents_map_.lower_bound(size);

  for (; it != largest_extents_map_.end(); ++it) {
    auto addr = reinterpret_cast<uintptr_t>(it->second.start);

    if ((addr & (alignment - 1)) == 0) {
      return it;
    }
  }

  return std::nullopt;
}
