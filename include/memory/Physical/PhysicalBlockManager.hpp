/* SPDX-License-Identifier: Apache-2.0 /
/*
 * \file   PhysicalBlockManager.hpp
 * \brief
 *
 * Copyright (c) 2026 Project Contributors
 * \author MaksymRbkh <https://github.com/MaksymRbkh>
 * \date   17.02.2026
 * \note
*/

#pragma once
#include <vector>
#include <memory/Physical/SizeClass.hpp>
#include <memory/Physical/HandleTable.hpp>
#include <memory/Physical/StartMap.hpp>

namespace noyxcore::memory {
  class PhysicalBlockManager {
  public:
    PhysicalBlockManager(std::size_t pageSize) : page_size_(pageSize)
    {
      size_classes_.emplace_back(4);
      size_classes_.emplace_back(16);
      size_classes_.emplace_back(64);
      size_classes_.emplace_back(256);
    };

    uint64_t allocate(std::size_t pages) {
      if (pages == 0) return 0;

      SizeClass* sizeClass = nullptr;
      for (auto& sc : size_classes_) {
        if (pages <= sc.pagesPerBlock()) {
          sizeClass = &sc;
          break;
        }
      }

      if (sizeClass == nullptr) return 0;

      auto result = sizeClass->allocate(pages);
      if (!result) {
        //VASManager nado koro4e
      }
      auto [block, pageOffset] = *result;
      return handle_table_.allocate(block, pageOffset);
    }

    void free(uint64_t handle) {
      auto result = handle_table_.lookUp(handle);
      if (result == std::nullopt) return;

      std::size_t block_pages_ = result->first->totalSize() / result->first->pageSize();
      SizeClass* sizeClass = nullptr;
      for (auto& sc : size_classes_) {
        if (sc.pagesPerBlock() == block_pages_) {
          sizeClass = &sc;
          break;
        }
      }

      if (sizeClass == nullptr) return;
      sizeClass->free(result->first, result->second);
      handle_table_.free(handle);
    }

    void* lookUp(uint64_t handle) {
      auto result = handle_table_.lookUp(handle);
      if (!result) return nullptr;

      auto [block, pageOffset] = *result;
      return static_cast<char*>(block->baseAddr()) + pageOffset * block->pageSize();
    }


  private:
    std::vector<SizeClass> size_classes_;
    StartMap start_map_;
    HandleTable handle_table_;
    std::size_t page_size_;
  };
}