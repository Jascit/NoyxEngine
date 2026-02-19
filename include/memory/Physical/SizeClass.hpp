/* SPDX-License-Identifier: Apache-2.0 /
/*
 * \file   SizeClass.hpp
 * \brief
 *
 * Copyright (c) 2026 Project Contributors
 * \author MaksymRbkh <https://github.com/MaksymRbkh>
 * \date   17.02.2026
 * \note
*/

#pragma once
#include <cstddef>
#include <optional>
#include <vector>
#include <memory/Physical/Block.hpp>

namespace noyxcore::memory {
  class SizeClass {
  public:
    explicit SizeClass(std::size_t pages) : pages_per_block_(pages) {}

    std::optional<std::pair<Block*, std::size_t>> allocate(std::size_t pages) {
      std::lock_guard<std::mutex> lock(mutex_);
      if (blocks_with_space_.empty()) return std::nullopt;

      Block* block = blocks_with_space_.back();
      void* ptr = block->allocate(pages);
      if (!ptr) return std::nullopt;
      std::size_t pageOffset = (static_cast<char*>(ptr) - static_cast<char*>(block->baseAddr())) / block->pageSize();

      if (block->isFull()) {
        blocks_with_space_.pop_back();
      }

      return std::make_pair(block, pageOffset);
    }

    void free(Block* block, std::size_t pageOffset) {
      std::lock_guard<std::mutex> lock(mutex_);
      void* ptr = static_cast<char*>(block->baseAddr()) + pageOffset * block->pageSize();
      block->free(ptr);

      auto it = std::find(blocks_with_space_.begin(), blocks_with_space_.end(), block);
      if (it == blocks_with_space_.end()) {
        blocks_with_space_.push_back(block);
      }

      if (block->isEmpty()) {
        removeBlock(block);
      }
    }

    void addBlock(Block* block) {
      std::lock_guard<std::mutex> lock(mutex_);
      blocks_.push_back(block);
      blocks_with_space_.push_back(block);
    }

    void removeBlock(Block* block) {
      std::lock_guard<std::mutex> lock(mutex_);
      auto it = std::find(blocks_.begin(), blocks_.end(), block);
      if (it != blocks_.end()) {
        blocks_.erase(it);
      }

      auto it2 = std::find(blocks_with_space_.begin(), blocks_with_space_.end(), block);
      if (it2 != blocks_with_space_.end()) {
        blocks_with_space_.erase(it2);
      }
    }

    std::size_t pagesPerBlock() const {
      std::lock_guard<std::mutex> lock(mutex_);
      return pages_per_block_;
    }

  private:
    mutable std::mutex mutex_;
    std::size_t pages_per_block_;
    std::vector<Block*> blocks_;
    std::vector<Block*> blocks_with_space_;
  };
}
