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
static FORCE_INLINE uint32_t handle_id(handle_t h) noexcept { return static_cast<uint32_t>(h); }
static FORCE_INLINE uint32_t handle_generation(handle_t h) noexcept { return static_cast<uint32_t>(h >> 32); }

static FORCE_INLINE handle_t make_handle(uint32_t generation, uint32_t id) noexcept {
  return (static_cast<uint64_t>(generation) << 32) | id;
}

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

IFreeSet* create_std_free_set(uint64_t capacity) {}

IStartMap* create_std_start_map(uint64_t capacity) {}

handle_t VASManager::reserve_block(uint64_t size, uint64_t alignment, uint32_t flags) noexcept {
  vaw_reserve_req_t reserve_req;
  reserve_req.size = size;
  reserve_req.alignment = alignment;
  reserve_req.alloc_flags = flags;

  vaw_reserve_resp_t reserve_resp = vaw_reserve_memory(&reserve_req, page_granularity_);

  if (reserve_resp.err != 0) {}

  uint32_t id = handle_table_.allocate_index();
  blocks_by_id_.emplace(std::pair(id, BlockRecord(reserve_resp.base, reserve_resp.size, 0)));
  uint32_t err = start_map_->insert(reserve_resp.base, id);

  if (err != 0) {
    // TODO: release block
    handle_table_.free_index(id);
  }

  FreeExtent extent;
  extent.size = reserve_resp.size;
  extent.start = reserve_resp.base;
  extent.block_id = id;
  err = largest_free_set_->insert(extent);

  if (err != 0) {
    // TODO: release block
  }

  return make_handle(0, id);
}

void* VASManager::try_allocate_in_block_(BlockRecord& target_block, uint64_t aligned_size, uint64_t alignment) {
  //TODO: block_lock lock

  uint64_t old_offset = target_block.current_offset.load(std::memory_order_acquire);
  uintptr_t base_addr = reinterpret_cast<uintptr_t>(target_block.base);
  uint64_t pad = padding_for(base_addr + old_offset, alignment);

  if (old_offset > UINT64_MAX - pad) return nullptr;
  uint64_t needed = pad;
  if (aligned_size > UINT64_MAX - needed) return nullptr;
  needed += aligned_size;

  FreeExtent prev_extent;
  prev_extent.start = reinterpret_cast<void*>(static_cast<uint8_t*>(target_block.base) + old_offset);
  prev_extent.size = target_block.size - old_offset;
  prev_extent.block_id = target_block.id;
  largest_free_set_->erase(prev_extent);

  uint64_t new_offset = old_offset + needed;
  target_block.current_offset.store(new_offset, std::memory_order_relaxed);
  target_block.ref_count.fetch_add(1, std::memory_order_relaxed);

  //TODO: block_look unlock

  if (target_block.size > new_offset) {
    FreeExtent new_largest_extent;
    new_largest_extent.size = target_block.size - new_offset;
    new_largest_extent.start = reinterpret_cast<void*>(static_cast<uint8_t*>(target_block.base) + new_offset);
    new_largest_extent.block_id = target_block.id;
    largest_free_set_->insert(new_largest_extent);
  }

  void* res = static_cast<uint8_t*>(target_block.base) + old_offset + pad;
  return res;
}

void* VASManager::allocate(void* block_hint, uint64_t size, uint64_t alignment, bool auto_reserve) {
  if (size == 0) { return nullptr; }

  uint64_t aligned_size = align_up(size, alignment);

  if (block_hint) {
    auto opt_id = start_map_->find_by_start(block_hint);
    if (opt_id.has_value()) {
      const uint32_t id = opt_id.value();
      auto block_it = blocks_by_id_.find(id);
      if (block_it != blocks_by_id_.end()) {
        void* res = try_allocate_in_block_(block_it->second, aligned_size, alignment);
        if (res) return res;
      }
    }
  }

  // 2. block_hint == nullptr or couldn't find block, try find_fit

  auto opt_extent = largest_free_set_->find_fit(aligned_size, alignment);
  if (opt_extent.has_value()) {
    FreeExtent chosen = opt_extent.value();
    auto block_it = blocks_by_id_.find(chosen.block_id);
    if (block_it != blocks_by_id_.end()) {
      void* res = try_allocate_in_block_(block_it->second, aligned_size, alignment);
      if (res) return res;
    } else {
      largest_free_set_->erase(chosen);
    }
  }

  // 3. try auto_reserve and allocate
  if (auto_reserve) {
    // little helper
    auto vaw_release_mem = [&](void* base, uint64_t release_size) noexcept -> void {
      vaw_release_req_t release_req;
      release_req.size = release_size;
      release_req.base = base;
      vaw_release_resp_t release_resp = vaw_release_memory(&release_req);
      if (release_resp.err != VAW_OK) {
        /*can't do shit*/
      }
    };

    uint64_t n = (default_size + aligned_size - 1) / aligned_size; // ceil(default_size / aligned_size)
    if (n == 0) n = 1;
    uint64_t reserve_size = n * aligned_size;

    vaw_reserve_req_t reserve_req;
    reserve_req.size = reserve_size;
    reserve_req.alignment = alignment;
    reserve_req.alloc_flags = common_flags;

    vaw_reserve_resp_t reserve_resp = vaw_reserve_memory(&reserve_req, page_granularity_);
    if (reserve_resp.err != 0 || reserve_resp.base == nullptr || reserve_resp.size < aligned_size) {
      // reserve failed
      return nullptr;
    }

    // if base_addr is not aligned
    uintptr_t base_addr = reinterpret_cast<uintptr_t>(reserve_resp.base);
    uint64_t pad = padding_for(base_addr, alignment);
    if (reserve_resp.size < aligned_size + pad) {
      vaw_release_mem(reserve_resp.base, reserve_resp.size);
      return nullptr;
    }

    uint32_t id = handle_table_.allocate_index();
    auto [it, succeeded] = blocks_by_id_.insert(std::pair(id, BlockRecord(reserve_resp.base, reserve_resp.size, 0)));
    if (!succeeded) {
      vaw_release_mem(reserve_resp.base, reserve_resp.size);
      return nullptr;
    }

    BlockRecord& target_block = it->second;
    uint64_t new_offset = pad + aligned_size;
    target_block.current_offset.store(new_offset, std::memory_order_release);
    target_block.generation.fetch_add(1, std::memory_order_release);

    uint32_t err = start_map_->insert(reserve_resp.base, id);
    if (err != 0) {
      handle_table_.free_index(id);
      vaw_release_mem(reserve_resp.base, reserve_resp.size);
      return nullptr;
    }

    if (target_block.size > new_offset) {
      FreeExtent new_largest_extent;
      new_largest_extent.size = target_block.size - new_offset;
      new_largest_extent.start = reinterpret_cast<void*>(base_addr + new_offset);
      new_largest_extent.block_id = target_block.id;
      largest_free_set_->insert(new_largest_extent);
      if (err != 0) {
        handle_table_.free_index(id);
        start_map_->erase(reserve_resp.base);
        vaw_release_mem(reserve_resp.base, reserve_resp.size);
        return nullptr;
      }
    }

    void* res = static_cast<uint8_t*>(target_block.base) + pad;
    return res;
  }
  // 4. nothing worked; failed
  return nullptr;
}

void VASManager::release_if_unreferenced_unlocked_(BlockRecord& target_block) {
  if (target_block.ref_count.load(std::memory_order_relaxed) == 0) {
    start_map_->erase(target_block.base);
    {
      FreeExtent extent;
      extent.size = target_block.size - target_block.current_offset.load(std::memory_order_relaxed);
      extent.block_id = target_block.id;
      extent.start = static_cast<void*>(static_cast<uint8_t*>(target_block.base) + target_block.current_offset.load(std::memory_order_relaxed));
      largest_free_set_->erase(extent);
    }
    blocks_by_id_.erase(target_block.id);
    handle_table_.free_index(target_block.id);
  }
}

bool VASManager::free_by_addr(void* addr, uint64_t size) noexcept {
  if (addr == nullptr || size == 0) return false;

  auto opt_block = start_map_->predecessor(addr);
  if (!opt_block.has_value()) {
    return false;
  }

  auto [base_addr, block_id] = opt_block.value();
  BlockRecord& target_block = blocks_by_id_.find(block_id)->second;
  //block_lock lock
  target_block.ref_count.fetch_sub(1, std::memory_order_release);
  release_if_unreferenced_unlocked_(target_block);
  //block_lock unlock
  return true;
}

