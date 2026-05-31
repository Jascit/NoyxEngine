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
    virtual void* allocate(uint64_t size) = 0;
    virtual void free(void* ptr) = 0;
    virtual ~IVirtualMemoryAllocator() = default;
  };

  template<typename T>
    requires requires (T& obj, uint64_t size, void* ptr)
    {
      { obj.allocate(size) } -> std::same_as<void*>;
      { obj.free(ptr, size) } -> std::same_as<void>;
    }
  class VMAAdapter : public IVirtualMemoryAllocator {
  public:
    explicit VMAAdapter(T* allocator) : m_allocator(allocator) {}
    ~VMAAdapter() override = default;

    FORCE_INLINE void* allocate(uint64_t size) override {
      return m_allocator->allocate(size);
    }

    FORCE_INLINE void free(void* ptr) override {
      m_allocator->free(ptr);
    };

  private:
    T* m_allocator;
  };

  class RAMManager {
  public:
    class RAMRegion {
    public:
      explicit RAMRegion(void* base, std::uint64_t size, std::size_t allocation_granularity);
      RAMRegion(RAMRegion&& other) noexcept;
      ~RAMRegion() = default;
      std::pair<void*, size_t> allocate(std::size_t size);
      void free(void* ptr, std::uint64_t size);
      FORCE_INLINE size_t size() const noexcept { return m_size; }

    private:
      FORCE_INLINE std::size_t index_from_ptr(void* ptr) noexcept;
      //                     bitmap_index,  offset_in_byte
      FORCE_INLINE std::pair<std::uint64_t, std::uint64_t> bit_position(std::size_t index) noexcept;
      FORCE_INLINE void* ptr_from_index(std::size_t index) noexcept;
      std::pair<bool, bool> check_collisions(std::size_t start_index, std::size_t size) noexcept;
      template<bool BitToChange>
      bool change_bits(std::size_t count_of_bits, std::size_t index) noexcept;

    private:
      void* m_base;
      std::size_t m_size;
      std::size_t m_allocated;
      std::size_t m_allocation_granularity;
      // blocks bitmap
      std::vector<std::uint64_t> m_bitmap;
      // bitmap offset -> size in bits
      std::map<std::size_t, std::size_t> m_free_ranges;
    };

    explicit RAMManager(IVirtualMemoryAllocator& allocator, std::size_t initial_reserve,
                        std::size_t allocation_granularity);
    RAMManager(RAMManager&&) = delete;
    RAMManager(RAMManager const&) = delete;
    RAMManager& operator=(RAMManager&&) = delete;
    RAMManager& operator=(RAMManager const&) = delete;

    [[nodiscard]] void* allocate(std::size_t size);
    void free(void* ptr);
    void reserve(std::size_t size);
    void release(std::size_t size);

    [[nodiscard]] FORCE_INLINE std::size_t total_allocated() const noexcept;
    [[nodiscard]] FORCE_INLINE std::size_t total_reserved() const noexcept;
    [[nodiscard]] FORCE_INLINE std::size_t alignment() const noexcept;

  private:
    // free_size->Region
    std::deque<RAMRegion> m_regions;
    std::multimap<std::size_t, RAMRegion*> m_free_by_size;
    std::unordered_map<void*, RAMRegion*> m_owner_by_ptr;

    IVirtualMemoryAllocator* m_allocator{nullptr};

    std::size_t m_allocation_granularity;
    std::size_t m_total_allocated{0};
    std::size_t m_total_reserved{0};
    std::size_t m_total_freed{0};
    std::size_t m_total_released{0};
  };
} // namespace noyxcore::memory


//allocator mit TLSF logic
template<uint64_t SLBits>
class SomeClass {
  // base must be commited
  explicit SomeClass(void* base, std::uint64_t size) noexcept;

private:
  const void* m_base;
  std::size_t m_size_of_slice; // min allocation, in bytes
  std::uint64_t m_size; // aligned to m_size_of_slice
  const std::size_t M_MAX_SIZE = 512 * 1024 * 1024; // max size for reserve, in bytes
  // count for bits to emplace 1 index of slice = while(m_size / 2) m_size /= 2;
  // index = (ptr-base)/m_size_of_slice
  std::size_t m_index_size; // in bits
  // max count of allocated ellements possible in the FL[fl_bins_count], worst case scenario; @ = 2^m_index_size/2
  const std::size_t MAX_COUNT_OF_ELEMENTS;
  std::vector<std::uint64_t> m_bitmap; // slices occupancy data

  // SLBits mal so groß wie m_bitmap+(while(count_of_fl_lvls / 2) count_of_fl_lvls /= 2;)),
  // zu welchen FL und SL bins gehört index von slice
  // erste while(count_of_fl_lvls / 2) count_of_fl_lvls /= 2 bits ist für FL, andere SLBits für SL
  // schnelles lookup für size for coallesing
  std::vector<std::uint64_t> m_slice_size_type;

  //                           lokal_bitmap(gibt es dieses Index hier?)
  std::vector<std::pair<SLBin, char*>> m_fl_bins; // static_vector size = while m_max_size != m_sl_ / 2
};

template<std::size_t sl_bits = 4>
class Allocator final {
  using index = std::uint16_t;

public:
  static constexpr std::size_t MAX_SIZE = 512ull * 1024ull * 1024ull; // 512 MiB

  explicit Allocator(void* base, std::size_t size) noexcept;

  [[nodiscard]] void* allocate(std::size_t size) noexcept;
  void deallocate(void* ptr, std::size_t size) noexcept;

  [[nodiscard]] std::size_t slice_size() const noexcept { return m_slice_size; }
  [[nodiscard]] std::size_t arena_size() const noexcept { return m_arena_size; }
  [[nodiscard]] std::size_t slice_count() const noexcept { return m_slice_count; }

private:
  std::size_t bits_per_word;

  struct SlBin {
    // RadixTree or smth similar with vector as a leaf, linear probing with AVX on indexes
    // will make my own array class with dynamic element
    // if index = 16 bits, erste 8 wird dazu gebraucht, um index, wo liegt das element, von array zu finden, dann
    // kommt linear probing; 8 Bits dürft uns maximal 2^8 elements in einem leaf zu haben,
    // es ist maximal 2^8*m_index_size of bits pro leaf im schlimmsten Fall zu durchsuchen, ich kann es noch schneller machen
    // aber es erzwingt von mir mehr Speicher zu verwenden, nehmen wir mall ein Beispiel mit 10 Bits für radix bins und 6
    // für vector, 2^10*16 bytes für vectors (8 = ptr, 4 = cap, 4 = size, Später ich muss es implementieren), im Fall von 8 bits es wird zum 2^10*16 bytes
    // verringern

    using sl_data = std::array<std::vector<index>,  /*sizeof = m_index_size/2*count_of_bits_for_bin*/>;
    std::array<sl_data, 2 ^ sl_bits> bins;
    std::uint64_t sl_bitmap = 0; // fast lookup in bins
    std::uint64_t max_elements = ;
  };

  struct fl_bin {
    std::array<SlBin, (1ull << sl_bits)> sl_bins{};
    std::uint64_t sl_bitmap = 0; // fast lookup in bins
  };

private:
  static constexpr std::size_t align_up(std::size_t value, std::size_t alignment) noexcept {
    return (value + alignment - 1) & ~(alignment - 1);
  }

  [[nodiscard]] std::size_t ptr_to_slice_index(const void* ptr) const noexcept;
  [[nodiscard]] void* slice_index_to_ptr(std::size_t slice_index) const noexcept;

  [[nodiscard]] std::pair<std::size_t, std::size_t> locate_bin(std::size_t slice_count) const noexcept;

  void insert_free_run(std::uint16_t start, std::uint16_t len) noexcept;
  void remove_free_run(std::uint16_t run_id) noexcept;
  void coalesce_at(std::uint16_t start, std::uint16_t len) noexcept;

private:
  void* m_base = nullptr;

  std::size_t m_arena_size = 0; // aligned arena size
  std::size_t m_slice_size = 0; // minimum allocation size in bytes
  std::size_t m_slice_count = 0; // number of slices in arena
  std::size_t m_index_bits = 0; // bits needed for a slice index

  // 1001 = occupied(block at index x size 4), 0000 = free, 101101 = 2 blocks each one size of 3(collision check)
  std::vector<std::uint64_t> m_occupancy_bitmap;

  // summary bitmap for faster lookup/coalescing
  std::vector<std::uint64_t> m_class_bitmap;

  // First-level bins
  std::vector<fl_bin> m_fl_bins;
};
