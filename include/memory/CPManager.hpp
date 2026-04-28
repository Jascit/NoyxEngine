/* SPDX-License-Identifier: Apache-2.0 */

/**
 * @file    CPManager.hpp
 * @brief
 *
 * @date    28.04.2026
 *
 * @copyright Copyright (c) 2026 The Project Contributors
 */
#pragma once
#include <concepts>

#include "platform/debug.h"

namespace noyxcore::memory {
  class IVirtualMemoryAllocator {
  public:
    virtual void* allocate(uint64_t size) = 0;
    virtual ~IVirtualMemoryAllocator() = default;
  };

  template<typename T>
    requires requires (T obj, uint64_t size)
  {
    { obj.allocate(size) } -> std::same_as<void*>;
  }
  class VMAAdapter : public IVirtualMemoryAllocator {
  public:
    explicit VMAAdapter(T* allocator) : allocator_(allocator) {}

    FORCE_INLINE void* allocate(uint64_t size) override {
      return allocator_->allocate(size);
    }

  private:
    T* allocator_;
  };

  class CPManager {
  public:
    CPManager(CPManager&&) = delete;
    CPManager(CPManager const&) = delete;
    CPManager& operator=(CPManager&&) = delete;
    CPManager& operator=(CPManager const&) = delete;

    void reserve(uint64_t size);
    void release();
    void* allocate(uint64_t size);
    void free(void* ptr);
    bool initialize(IVirtualMemoryAllocator* interface);

  private:
    CPManager() noexcept;

  private:
    // grupen sort? dirty/pinned Flag::*
    // 4/64/1024KB Blocks
    //
    uint64_t total_allocated_;
    uint64_t total_reserved_;
    uint64_t total_freed_;
    uint64_t total_released_;
  };
} // namespace noyxcore::memory
