/* SPDX-License-Identifier: Apache-2.0 */

/**
 * @file     Block.hpp
 * @brief     
 *
 * @author   MaksymRbkh (https://github.com/MaksymRbkh)
 * @date     15.01.2026
 *
 * @copyright Copyright (c) 2026 The Project Contributors
 */

#pragma once
#include <cstddef>
#include <mutex>
#include <memory/Physical/CommitBitmap.hpp>
#include <memory/Physical/AllocationMap.hpp>
#include <memory/Physical/VirtualMemory.hpp>

namespace noyxcore::memory {
  class Block {
  public:
    explicit Block(void* baseAddr, std::size_t totalPages, std::size_t pageSize)
      : base_addr_(baseAddr), total_pages_(totalPages), page_size_(pageSize), commit_bitmap_(totalPages),
        alloc_map_(totalPages) {}

    Block& operator=(const Block&) = delete;
    Block(const Block&) = delete;
    Block(Block&&) = default;
    Block& operator=(Block&&) = default;

    ~Block() = default;

    void* allocate(std::size_t pages) {
      std::lock_guard<std::mutex> lock(mutex_);
      auto result = alloc_map_.allocate(pages);
      if (!result) return nullptr;

      auto startPage = *result;
      bool commitFailed = false;

      commit_bitmap_.forEachClear(startPage, pages, [&] (std::size_t s, std::size_t c) {
        if (commitFailed) return;
        void* addr = static_cast<char*>(base_addr_) + s * page_size_;
        if (!OS::Commit(addr, c * page_size_)) {
          commitFailed = true;
        }
      });
      if (commitFailed) {
        alloc_map_.free(startPage);
        return nullptr;
      }
      commit_bitmap_.setRange(startPage, pages);
      return static_cast<char*>(base_addr_) + startPage * page_size_;
    }

    void free(void* ptr) {
      std::lock_guard<std::mutex> lock(mutex_);
      auto startPage = (static_cast<char*>(ptr) - static_cast<char*>(base_addr_)) / page_size_;
      alloc_map_.free(startPage);
    }

    bool isEmpty() const {
      std::lock_guard<std::mutex> lock(mutex_);
      return alloc_map_.isEmpty();
    }

    bool isFull() const {
      std::lock_guard<std::mutex> lock(mutex_);
      return alloc_map_.isFull();
    }

    std::size_t totalSize() const {
      return total_pages_ * page_size_;
    }

    void* baseAddr() const { return base_addr_; }
    std::size_t pageSize() const { return page_size_; }

  private:
    void* base_addr_;
    std::size_t total_pages_;
    std::size_t page_size_;

    AllocationMap alloc_map_;
    CommitBitmap commit_bitmap_;

    mutable std::mutex mutex_;
  };
}
