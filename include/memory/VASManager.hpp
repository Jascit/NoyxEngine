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
#include <atomic>
#include <mutex>
#include <shared_mutex>
#include <optional>
#include <unordered_map>
#include <list>
#include <functional>
#include <map>
#include <memory>
#include <utility> // for std::pair

namespace noyxcore::memory {
  // Public handle type (Block-ID / region-handle | gen)
  constexpr uint32_t INVALID_ID = 0;

  // Minimaler FreeExtent für freie Bereiche / First-Fit-Index
  struct FreeExtent {
    void* start = nullptr;
    uint64_t size = 0;
    uint32_t block_id = INVALID_ID;
  };

  // Metadaten eines Blocks (VAS block)
  struct BlockRecord {
    void* base; // base address (VAS)
    size_t size; // block size in bytes
    uint32_t block_id;

    // Reference count for RAII handles
    std::atomic<uint32_t> ref_count;

    // Bump pointer: protected by atomic.
    std::atomic<uint64_t> current_offset;

    // Generation tag to detect stale handles (increment on reserve/recreate)
    std::atomic<uint64_t> generation;

    // Optional debug counters (disabled by default)
    // std::atomic<uint64_t> alloc_count{0}, release_count{0};

    BlockRecord() = delete;

    BlockRecord(BlockRecord&& other) noexcept : base(std::exchange(other.base, nullptr)),
                                                size(std::exchange(other.size, 0)),
                                                block_id(std::exchange(other.block_id, 0)),
                                                ref_count(other.ref_count.exchange(0)),
                                                current_offset(other.current_offset.exchange(0)),
                                                generation(other.generation.exchange(0)) {};

    BlockRecord(void* b, size_t s, uint64_t gen, uint32_t block_id) noexcept
      : base(b), size(s), block_id(block_id), ref_count(0), current_offset(0), generation(gen) {}
  };

  // Interface for a free-set index (e.g. largest-first or best-fit structure)
  class FreeSet {
  public:
    using map_type = std::map<uintptr_t, FreeExtent>;

    ~FreeSet() = default;

    // Insert / remove an extent into the free-set index
    std::optional<map_type::iterator> insert(const FreeExtent& extent);
    bool erase(const FreeExtent& extent);
    bool erase(const map_type::iterator& it);

    // Find a fit for size with alignment; returns std::nullopt if none
    std::optional<map_type::iterator> findFit(uint64_t size, uint64_t alignment);

  private:
    map_type largest_extents_map_;
  };

  // Mapping from start-address -> region handle (or block id).
  // Must be thread-safe in implementations.
  class StartMap {
    using map_type = std::multimap<uintptr_t, uint32_t>;
  public:
    ~StartMap() = default;
    // Associate start -> region/block id
    std::optional<std::map<uintptr_t, uint32_t>::iterator> insert(void* start, uint32_t id) ;
    bool erase(void* start);
    bool erase(const map_type::iterator& it);
    std::optional<uint32_t> findByStart(void* start) const;

    // predecessor: largest start <= addr: returns pair(start, id) or nullopt
    std::optional<std::pair<void*, uint32_t>> predecessor(void* addr);
  private:
    std::map<uintptr_t, uint32_t> predecessor_map_;
    map_type lookup_map_;
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
    VASManager(size_t common_flags, size_t default_block_size = 256ull * 1024 * 1024) noexcept;

    ~VASManager();

    // Reserve a block of VAS. Returns block-handle or INVALID_HANDLE on failure.
    // alignment and flags are forwarded to the platform-specific reserveroutine.
    uint32_t reserveBlock(uint64_t size, uint64_t alignment = 0, uint32_t flags = 0) noexcept;

    // Release block by id. Returns true on success, false otherwise.
    // If force==true the block will be removed even with non-zero refcount (dangerous).
    bool releaseBlock(uint32_t block_id, bool force = false) noexcept;

    // Allocate a region (optionally within a specific block_hint). Returns region-handle
    // (or INVALID_HANDLE on failure). The returned handle must be released via free(handle)
    void* allocate(void* block_hint,
                      uint64_t size,
                      uint64_t alignment = 1,
                      uint32_t flags = 0,
                      bool auto_reserve = true) noexcept;

    // Free a previously allocated region (by handle or by address/size).
    // Returns true on success.
    bool freeByAddr(void* addr, uint64_t size) noexcept;

    // Address lookup: returns optional block-handle if present.
    std::optional<uint32_t> findByAddr(void* addr) const noexcept;

    // Diagnostics (const)

    struct Statistics {
      uint64_t total_released;
      uint64_t total_reserved;
      uint64_t total_allocated;
      uint64_t total_freed;
    };

    Statistics statistics() const noexcept;

    // Non-copyable
    VASManager(const VASManager&) = delete;
    VASManager& operator=(const VASManager&) = delete;

  private:

    void* try_allocate_in_block_(BlockRecord& target_block, uint64_t aligned_size, uint64_t alignment);

    bool release_block_(const std::unordered_map<uint32_t, BlockRecord>::iterator& target_block);
  private:
    // Primary storage: block id -> BlockRecord
    std::unordered_map<uint32_t, BlockRecord> blocks_by_id_;
    mutable std::shared_mutex blocks_mutex_; // allows concurrent readers for lookups

    // Injected index structures (not owned)
    StartMap start_map_; // optional, for address->handle mapping
    FreeSet largest_free_set_; // optional, fragmentation index

    // Handle allocator for blocks/regions
    HandleTable handle_table_;

    // Global counters / stats
    std::atomic<uint64_t> total_reserved_;
    std::atomic<uint64_t> total_released_;
    std::atomic<uint64_t> total_allocated_;
    std::atomic<uint64_t> total_free_;

    // Configuration knobs
    const uint64_t page_granularity_;
    const size_t default_block_size_;
    const uint32_t common_flags_;
  };
} // namespace noyxcore::memory
