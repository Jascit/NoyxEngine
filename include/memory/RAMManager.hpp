/* SPDX-License-Identifier: Apache-2.0 */

/**
 * @file    RAMManager.hpp
 * @brief
 *
 * @date    28.04.2026
 *
 * @copyright Copyright (c) 2026 The Project Contributors
 */
#pragma once
#include <concepts>
#include <deque>
#include <map>
#include <unordered_map>
#include <vector>

#include "platform/debug.h"

namespace noyxcore::memory {
class IVirtualMemoryAllocator {
 public:
  virtual void *allocate(uint64_t size) = 0;
  virtual void free(void *ptr) = 0;
  virtual ~IVirtualMemoryAllocator() = default;
};

template <typename T>
  requires requires(T &obj, uint64_t size, void *ptr) {
    { obj.allocate(size) } -> std::same_as<void *>;
    { obj.free(ptr, size) } -> std::same_as<void>;
  }
class VMAAdapter : public IVirtualMemoryAllocator {
 public:
  explicit VMAAdapter(T *allocator) : m_allocator(allocator) {}
  ~VMAAdapter() override = default;

  FORCE_INLINE void *allocate(uint64_t size) override {
    return m_allocator->allocate(size);
  }

  FORCE_INLINE void free(void *ptr) override { m_allocator->free(ptr); };

 private:
  T *m_allocator;
};

class RAMManager {
  using RegionID = std::uint32_t;

 public:
  class RAMRegion {
    using size_type = std::size_t;
    using range_map = std::map<size_type, size_type>;

   public:
    class PageBitmap {
     public:
      PageBitmap(std::size_t granularity, std::size_t total_size) noexcept;

      [[nodiscard]] bool is_range_free(void *base, void *ptr,
                                       std::size_t count) const noexcept;
      [[nodiscard]] bool is_range_used(void *base, void *ptr,
                                       std::size_t count) const noexcept {
        return !is_range_free(base, ptr, count);
      }

      void set_range(void *base, void *ptr, std::size_t count,
                     bool value) noexcept;

      [[nodiscard]] std::size_t granularity() const noexcept {
        return m_granularity;
      }

      [[nodiscard]] std::size_t page_count() const noexcept {
        return m_bitmap.size() * 64u;
      }

     private:
      std::vector<std::uint64_t> m_bitmap;
      std::size_t m_granularity{0};
    };

   public:
    RAMRegion() noexcept = default;

    [[nodiscard]] size_type total_free() const noexcept { return m_total_free; }

   private:
    PageBitmap m_pages;
    range_map m_free_ranges;

    size_type m_total_free{0};
  };

  class RegionManager {
   public:
    void split_region(RegionID id, std::size_t size);
    RegionID find_region_by_ptr(void *base);
    void coalesce(RegionID region_id) noexcept;

   private:
    RegionID m_next_region_id{0};
    std::list<RegionID> m_free_ids;
    std::vector<RAMRegion> m_regions;
  };

  explicit RAMManager(IVirtualMemoryAllocator &allocator,
                      std::size_t initial_reserve,
                      std::size_t allocation_granularity);
  RAMManager(RAMManager &&) = delete;
  RAMManager(RAMManager const &) = delete;
  RAMManager &operator=(RAMManager &&) = delete;
  RAMManager &operator=(RAMManager const &) = delete;

  [[nodiscard]] void *allocate_region(std::size_t size);
  void free_region(void *ptr);
  void reserve(std::size_t size);
  void release(std::size_t size);

  [[nodiscard]] FORCE_INLINE std::size_t total_allocated() const noexcept;
  [[nodiscard]] FORCE_INLINE std::size_t total_reserved() const noexcept;
  [[nodiscard]] FORCE_INLINE std::size_t alignment() const noexcept;

 private:
  RegionID find_best_fit(uint64_t size);
  void create_bitmap(RegionID id, std::size_t size);

 private:
  RegionManager m_region_manager;
  std::multimap<std::size_t, RegionID> m_free_by_size;
  std::map<void *, RegionID> m_owner_by_ptr;

  IVirtualMemoryAllocator *m_virtual_allocator{nullptr};

  std::size_t m_allocation_granularity;
  std::size_t m_total_allocated{0};
  std::size_t m_total_reserved{0};
  std::size_t m_total_freed{0};
  std::size_t m_total_released{0};
};
}  // namespace noyxcore::memory
