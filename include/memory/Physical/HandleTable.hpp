/* SPDX-License-Identifier: Apache-2.0 /
/*
 * \file   HandleTable.hpp
 * \brief
 *
 * Copyright (c) 2026 Project Contributors
 * \author MaksymRbkh <https://github.com/MaksymRbkh>
 * \date   17.02.2026
 * \note
*/

#pragma once
#include <cstddef>
#include <vector>
#include "Block.hpp"

namespace noyxcore::memory {
  class HandleTable {
  public:
    uint64_t allocate(Block* block, std::size_t pageOffset) {
      std::lock_guard<std::mutex> lock(mutex_);
      std::size_t index;
      if (!free_indexes_.empty()) {
        index = free_indexes_.back();
        free_indexes_.pop_back();
      }
      else {
        index = slots_.size();
        slots_.push_back({});
      }
      Slot& slot = slots_[index];
      slot.block_ = block;
      slot.occupied_ = true;
      slot.page_offset_ = pageOffset;

      uint64_t handle = (static_cast<uint64_t>(slot.generation_) << 32) | index;
      return handle;
    }

    void free(uint64_t handle) {
      std::lock_guard<std::mutex> lock(mutex_);
      std::size_t index = handle & 0xFFFFFFFF;
      uint32_t gen   = handle >> 32;
      Slot& slot = slots_[index];
      if (slot.generation_ == gen) {
        slot.generation_++;
        slot.occupied_ = false;
        slot.block_ = nullptr;
        slot.page_offset_ = 0;
        free_indexes_.push_back(index);
      }
    }

    std::optional<std::pair<Block*, size_t>> lookUp(uint64_t handle) {
      std::lock_guard<std::mutex> lock(mutex_);
      std::size_t index = handle & 0xFFFFFFFF;
      uint32_t gen   = handle >> 32;
      if (index >= slots_.size()) return std::nullopt;

      Slot& slot = slots_[index];
      if (slot.generation_ == gen && slot.occupied_) {
        return std::make_pair(slot.block_, slot.page_offset_);
      }
      return std::nullopt;
    }

  private:
    struct Slot {
      Block* block_ = nullptr;
      std::size_t page_offset_ = 0;
      uint32_t generation_ = 1;
      bool occupied_ = false;
    };
    mutable std::mutex mutex_;
    std::vector<Slot> slots_;
    std::vector<std::size_t> free_indexes_;
  };
}
