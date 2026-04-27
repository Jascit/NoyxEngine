/* SPDX-License-Identifier: Apache-2.0 */

/**
 * @file     StartMap.hpp
 * @brief     
 *
 * @date     16.01.2026
 *
 * @copyright Copyright (c) 2026 The Project Contributors
 */

#pragma once
#include <memory/Physical/Block.hpp>
#include <map>
#include <shared_mutex>

namespace noyxcore::memory {
  class StartMap {
  public:
    void sign(void* baseAddr, Block* block) {
      std::unique_lock<std::shared_mutex> lock(mutex_);
      block_map_.emplace(reinterpret_cast<uintptr_t>(baseAddr), block);
    }

    void unsign(void* baseAddr) {
      std::unique_lock<std::shared_mutex> lock(mutex_);
      block_map_.erase(reinterpret_cast<uintptr_t>(baseAddr));
    }

    Block* find(void* ptr) {
      std::shared_lock<std::shared_mutex> lock(mutex_);
      auto addr = reinterpret_cast<uintptr_t>(ptr);
      auto it = block_map_.upper_bound(addr);
      if (it == block_map_.begin()) return nullptr;
      --it;

      Block* block = it->second;
      uintptr_t start = it->first;
      uintptr_t end = start + block->totalSize();
      if (addr >= start && addr < end) {
        return block;
      }
      return nullptr;
    }


  private:
    mutable std::shared_mutex mutex_;
    std::map<uintptr_t, Block*> block_map_;
  };
}
