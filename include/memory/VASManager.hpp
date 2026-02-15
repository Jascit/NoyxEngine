/* SPDX-License-Identifier: Apache-2.0 */
/*
 * \file   VASManager.hpp
 * \brief  VAS manager
 *
 * Copyright (c) 2026 Project Contributors
 * \author Jascit<https://github.com/Jascit>
 * \date   13.02.2026
 */

#pragma once

#include <cstdint>
#include <cstddef>
#include <atomic>
#include <mutex>
#include <shared_mutex>
#include <optional>
#include <unordered_map>
#include <list>
#include <vector>
#include <functional>
#include <map>
#include <memory>
#include <utility> // for std::pair
#include <type_traits>

namespace noyxcore::memory {
  // Public handle type (Block-ID / region-handle | gen)
  using handle_t = uint64_t;
  constexpr handle_t INVALID_HANDLE = 0;

  // Minimaler FreeExtent für freie Bereiche / Best-Fit-Index
  struct FreeExtent {
    void* start = 0;
    uint64_t size = 0;
    handle_t block_id = INVALID_HANDLE;
  };

  // Metadaten eines Blocks (VAS block)
  struct BlockRecord {
    void* base = nullptr; // base address (VAS)
    size_t size = 0; // block size in bytes
    uint32_t id;

    // Reference count for RAII handles
    std::atomic<uint32_t> ref_count{0};

    // Bump pointer: protected by atomic.
    std::atomic<uint64_t> current_offset = 0;

    // Generation tag to detect stale handles (increment on reserve/recreate)
    std::atomic<uint64_t> generation{0};

    // Optional debug counters (disabled by default)
    // std::atomic<uint64_t> alloc_count{0}, release_count{0};

    BlockRecord() noexcept = default;

    BlockRecord(BlockRecord&& other) noexcept : size(std::exchange(other.size, 0)),
                                                base(std::exchange(other.base, nullptr)),
                                                current_offset(other.current_offset.exchange(0)),
                                                generation(other.generation.exchange(0)),
                                                ref_count(other.ref_count.exchange(0)) {};

    BlockRecord(void* b, size_t s, uint64_t gen) noexcept
      : base(b), size(s), ref_count(0), current_offset(0), generation(gen) {}
  };

  // Interface for a free-set index (e.g. largest-first or best-fit structure)
  class IFreeSet {
  public:
    virtual ~IFreeSet() = default;

    // Insert / remove an extent into the free-set index
    virtual bool insert(const FreeExtent& extent);
    virtual bool erase(const FreeExtent& extent);

    // Find a fit for size with alignment; returns std::nullopt if none
    virtual std::optional<FreeExtent> find_fit(uint64_t size, uint64_t alignment);

    virtual uint64_t total_free() const;
    virtual uint64_t largest_free() const;

  private:
    std::atomic<uint64_t> total_free_;
    std::atomic<uint64_t> largest_free_;
  };

  // Mapping from start-address -> region handle (or block id).
  // Must be thread-safe in implementations.
  class IStartMap {
  public:
    using hash_map_type = std::unordered_map<uintptr_t, uint32_t>;

    // Associate start -> region/block id
    hash_map_type::iterator insert(void* start, uint32_t id) ;
    bool erase(void* start);
    bool erase(hash_map_type::iterator it);
    std::optional<uint32_t> find_by_start(void* start);

    // predecessor: largest start <= addr: returns pair(start, id) or nullopt
    std::optional<std::pair<void*, uint32_t>> predecessor(void* addr);

  private:
    std::map<uintptr_t, uint32_t> predecessor_map_;
    hash_map_type _map_;
  };

  // Simple thread-safe HandleTable for allocating small integer handles.
  // Inline implementation provided for convenience.
  class HandleTable {
  public:
    HandleTable() noexcept : next_index_(0) {}
    ~HandleTable() = default;

    // Allocate a new handle (monotonic). Reuse freed indices if available.
    uint32_t allocate_index() {
      if (!free_list_.empty()) {
        const uint32_t h = free_list_.back();
        free_list_.pop_back();
        return h;
      }
      return next_index_.fetch_add(1, std::memory_order_release);
    }

    // Free a previously allocated handle: returns true if freed, false if invalid
    bool free_index(uint32_t h) {
      if (h >= next_index_) return false; // never allocated
      free_list_.push_back(h);
      return true;
    }

    // Non-copyable
    HandleTable(const HandleTable&) = delete;
    HandleTable& operator=(const HandleTable&) = delete;

  private:
    std::atomic<uint32_t> next_index_;
    std::list<uint32_t> free_list_;
  };

  // Main manager API (declaration only). Implementierung bleibt in .cpp.
  class VASManager {
  public:
    // Construct with injected indices (optional: nullptr allowed -> manager may use simple defaults)
    VASManager(IFreeSet* free_set = nullptr, IStartMap* start_map = nullptr,
               size_t default_block_size = 256ull * 1024 * 1024) noexcept;

    ~VASManager();

    // Reserve a block of VAS. Returns block-handle or INVALID_HANDLE on failure.
    // alignment and flags are forwarded to the platform-specific reserveroutine.
    handle_t reserve_block(uint64_t size, uint64_t alignment = 0, uint32_t flags = 0) noexcept;

    // Release block by id. Returns true on success, false otherwise.
    // If force==true the block will be removed even with non-zero refcount (dangerous).
    bool release_block(handle_t block_id, bool force = false) noexcept;

    // Allocate a region (optionally within a specific block_hint). Returns region-handle
    // (or INVALID_HANDLE on failure). The returned handle must be released via free(handle)
    handle_t allocate(void* block_hint,
                      uint64_t size,
                      uint64_t alignment = 1,
                      uint32_t flags = 0,
                      bool auto_reserve = true) noexcept;

    // Free a previously allocated region (by handle or by address/size).
    // Returns true on success.
    bool free_handle(handle_t region_handle) noexcept;
    bool free_by_addr(void* addr, uint64_t size) noexcept;

    // Address lookup: returns optional region-handle if present.
    std::optional<handle_t> find_by_addr(void* addr) const noexcept;

    // Resolve a block handle to BlockRecord (returns optional reference_wrapper)
    // Use reference_wrapper to avoid returning pointer-sized sentinel types.
    std::optional<std::reference_wrapper<BlockRecord>> resolve_block(handle_t h) noexcept;

    // Diagnostics (const)
    uint64_t get_largest_free() const noexcept;
    uint64_t get_total_free() const noexcept;

    struct FragmentationMetrics {
      double fragmentation_ratio = 0.0;
      uint64_t largest_free = 0;
      uint64_t total_free = 0;
    };

    FragmentationMetrics fragmentation_metrics() const noexcept;

    // Non-copyable
    VASManager(const VASManager&) = delete;
    VASManager& operator=(const VASManager&) = delete;

  private:
    // increment refcount only if generation matches — used by RAII handles/copies
    bool inc_ref_if_valid_(handle_t block_id, uint64_t expected_generation) noexcept;

    // Map lookup helper that returns pointer under shared lock or nullptr
    BlockRecord* get_block_nolock_(handle_t id) noexcept;

    void* try_allocate_in_block_(BlockRecord& target_block, uint64_t aligned_size, uint64_t alignment);

    void release_if_unreferenced_unlocked_(BlockRecord& target_block);

  private:
    // Primary storage: block id -> BlockRecord (use unique_ptr to allow stable addresses)
    std::unordered_map<uint32_t, BlockRecord> blocks_by_id_;
    mutable std::shared_mutex blocks_mutex_; // allows concurrent readers for lookups

    // Injected index structures (not owned)
    IStartMap start_map_; // optional, for address->handle mapping
    IFreeSet largest_free_set_; // optional, fragmentation index

    // Handle allocator for blocks/regions
    HandleTable handle_table_;

    // Global counters / stats
    std::atomic<uint64_t> total_reserved_{0};
    std::atomic<uint64_t> total_released_{0};
    std::atomic<uint64_t> total_allocated_{0};
    std::atomic<uint64_t> total_free_{0};

    // Configuration knobs
    const uint64_t page_granularity_{4096};
    uint64_t min_split_size_{4096};
    const size_t default_block_size_;
  };
} // namespace noyxcore::memory
