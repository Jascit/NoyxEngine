#pragma once
#define NOMINMAX
#include <Windows.h>
#include <platform/typedef.hpp>
#include <array>
#include <memory>
#include <cstdint>
#include <cstddef>
#include <atomic>
#include <shared_mutex>
#include <functional>
#include <unordered_map>
#include <platform/xmemory.hpp>
#include "containers/static_array.hpp"

namespace noyx {
  namespace detail {
    /**
     * @brief Virtual Memory Address Space Manager.
     *
     * This class provides a high-level manager for reserving, committing,
     * and decommitting virtual address ranges. The implementation stores
     * metadata in an internal radix-like structure (implementation details
     * expected in the .cpp file).
     *
     * The class is non-copyable and non-movable.
     */
    class VMASpaceMG {
    private:

      /**
       * @brief Helper container that maps sorted virtual base addresses to bitmaps of free pages.
       *
       * VirtualBaseMap keeps an ordered array of virtual base addresses and an associated
       * bitmap (TStaticArray<noyx::uint64>) for each base describing free/used pages.
       *
       * @tparam Size Maximum number of virtual bases the static array can hold.
       */
      class VirtualBaseMap {
      public:
        /**
         * @brief Construct a VirtualBaseMap.
         *
         * @param pageCount Number of pages per virtual base region (used to compute bitmap length).
         */
        VirtualBaseMap(noyx::size_t pageCount) : pageCount_(pageCount) {
          currentSize_ = 0;
          freePagesBitmap_.reserve(32);
          std::fill(std::begin(virtualBases_), std::end(virtualBases_), nullptr);
        }

        /**
         * @brief Access entry by index.
         *
         * Returns a pair of the stored base address and its corresponding free-pages bitmap.
         * If index is out of range returns {nullptr, empty TStaticArray}.
         *
         * @param idx Index into the sorted virtualBases_ array.
         * @return std::pair<void*, noyx::containers::TStaticArray<noyx::uint64>>
         */
        [[nodiscard]] std::pair<void*, noyx::containers::StaticArray<noyx::uint64>> operator[](noyx::size_t idx) noexcept {
          if (idx >= currentSize_) return { nullptr, noyx::containers::StaticArray<noyx::uint64>(0) };
          void* base = insertionOrder_[idx];
          return std::make_pair(base, freePagesBitmap_[base]);
        }

        /**
         * @brief Insert a new virtual base address into the sorted array.
         *
         * If the address is new, allocates a bitmap for it sized to hold pageCount_ bits.
         * This keeps the array sorted (binary-search insertion).
         *
         * @param addr Pointer to the start of virtual base region to insert.
         */
          void push(void* addr) {
          uintptr_t key = reinterpret_cast<uintptr_t>(addr);
          size_t lo = 0;
          size_t hi = currentSize_;

          while (lo < hi) {
            size_t mid = lo + (hi - lo) / 2;
            uintptr_t mid_addr = reinterpret_cast<uintptr_t>(virtualBases_[mid]);
            if (mid_addr < key) lo = mid + 1;
            else hi = mid;
          }

          std::copy_backward(virtualBases_ + lo, virtualBases_ + currentSize_, virtualBases_ + currentSize_ + 1);
          virtualBases_[lo] = addr;
          size_t words = (pageCount_ + 63) / 64;
          auto& bitmap = freePagesBitmap_[addr];
          if (bitmap.size() == 0) {
            bitmap = noyx::containers::StaticArray<noyx::uint64>(words);
          }
          insertionOrder_[currentSize_] = addr;
          currentSize_++;
        }

        /**
         * @brief Find the containing base for an arbitrary address.
         *
         * Performs a binary search to find the largest base <= addr.
         *
         * @param addr Address to look up.
         * @return The base pointer that contains @p addr, or nullptr if none found.
         */
        [[nodiscard]] void* find_base(void* addr) const noexcept {
          if (currentSize_ == 0) return nullptr;
          uintptr_t key = reinterpret_cast<uintptr_t>(addr);
          size_t lo = 0;
          size_t hi = currentSize_;
          while (lo < hi) {
            size_t mid = lo + (hi - lo) / 2;
            uintptr_t mid_addr = reinterpret_cast<uintptr_t>(virtualBases_[mid]);
            if (mid_addr <= key) lo = mid + 1;
            else hi = mid;
          }
          if (lo == 0) return nullptr;
          return virtualBases_[lo - 1];
        }

        [[nodiscard]] noyx::containers::StaticArray<noyx::uint64>*
            find_bitmap(void* base) noexcept
        {
            auto it = freePagesBitmap_.find(base);
            if (it == freePagesBitmap_.end())
                return nullptr;
            return &it->second;
        }

      private:
        /// Sorted static array of virtual base addresses (capacity = Size).
        void* virtualBases_[32];
        void* insertionOrder_[32];

        /// Number of pages in each base region (used to compute bitmap length).
        noyx::size_t pageCount_;

        /**
         * Map from base pointer -> bitmap describing free pages (one bit per page).
         * Using unordered_map to associate a TStaticArray for each base.
         */
        std::unordered_map<void*, noyx::containers::StaticArray<noyx::uint64>> freePagesBitmap_;

        /// Current number of stored virtual bases (<= Size).
        noyx::size_t currentSize_;
      };

    public:
      // Disable copying and moving; ownership is sole and unique.
      VMASpaceMG(const VMASpaceMG&) = delete;
      VMASpaceMG& operator=(const VMASpaceMG&) = delete;
      VMASpaceMG(VMASpaceMG&&) = delete;
      VMASpaceMG& operator=(VMASpaceMG&&) = delete;

      /**
       * @brief Construct a VMASpaceMG with the requested virtual space size and page granularity.
       *
       * @param size Total virtual address space size the manager should handle (implementation defined).
       * @param pageGranularity Page granularity (page size) used for commits/decommits and bitmap bookkeeping.
       */
      VMASpaceMG(noyx::size_t size, noyx::size_t pageGranularity, noyx::size_t pgAllocGranuality, noyx::size_t firstCommit = 0);

      /**
       * @brief Destructor. Cleans up resources and unregisters any handlers (if any).
       */
      ~VMASpaceMG();

      /**
       * @brief Initialize internal structures (optional; may be invoked from ctor).
       *
       * @return true on success, false on failure.
       */
      bool init();

      /**
       * @brief Shutdown and release any held resources (optional; may be invoked from dtor).
       */
      void shutdown();

      /**
       * @brief Reserve a contiguous virtual address range (MEM_RESERVE).
       *
       * Reserves virtual address space of @p size with optional @p alignment.
       * The returned address should be considered the base of the reserved region.
       *
       * @param size Number of bytes to reserve.
       * @param alignment Optional alignment for the returned base (0 = default OS alignment).
       * @return Pointer to the reserved virtual base, or nullptr on failure.
       */
      void* reserveRange(noyx::uint64 size, noyx::size_t alignment = 0);

      /**
       * @brief Commit physical memory within a previously reserved range (MEM_COMMIT).
       *
       * Commits pages starting at @p base for @p size bytes using protection @p protect.
       *
       * @param base Base address within a reserved range.
       * @param size Number of bytes to commit.
       * @param protect Memory protection flags (defaults to PAGE_READWRITE).
       * @return true on success, false on failure.
       */
      bool commitPages(void* base, noyx::uint64 size, noyx::uint32 protect = PAGE_READWRITE);

      /**
       * @brief Decommit previously committed pages (MEM_DECOMMIT).
       *
       * @param base Base address of the committed region.
       * @param size Number of bytes to decommit.
       * @return true on success, false on failure.
       */
      bool decommitPages(void* base, noyx::uint64 size);

      /**
       * @brief Runtime statistics for the VMA manager.
       */
      struct Stats {
        /// Total reserved bytes across all reservations.
        std::uint64_t reservedBytes = 0;

        /// Total currently committed bytes.
        std::uint64_t committedBytes = 0;

        /// Number of commit operations performed.
        std::uint64_t allocationCount = 0;
      };

      /**
       * @brief Retrieve current statistics snapshot.
       *
       * This is safe for concurrent readers; the values are atomically read.
       *
       * @return Stats snapshot.
       */
      Stats getStats() const;

      /**
       * @brief Optional features the manager may support (implementation note).
       *
       * Examples:
       * - Enable/disable guard pages.
       * - Fault handler registration (custom structured exception handler).
       * - Snapshot/dump metadata for analysis.
       */

    private:
      /**
       * @name Internal synchronization helpers
       * @{
       *
       * The manager uses read/write locking for concurrent access to its metadata.
       * lockForRead/unlockForRead are const so that const-correct inspection methods
       * can acquire read locks without breaking the const interface.
       */
      void lockForRead() const;
      void unlockForRead() const;
      void lockForWrite();
      void unlockForWrite();
      bool unlockCommitCheck(void* base, void* addr, noyx::uint64 pageCount);
      bool unlockDecommitCheck(void* base, void* addr, noyx::uint64 pageCount);
      /** @} */

      // ------------------------
      // Member fields
      // ------------------------

      /// Maximum virtual space size the manager supports (implementation constant).
      uint64 virtualSpaceSize_;

      /// Size of chunk used to compute map capacity (implementation constant).
      uint64 virtualSpaceForReserve_;

      /// Page granularity used by this manager (page size in bytes).
      noyx::size_t pgGranularity_;

      /// Page allocation granularity used by this manager (virtual reserve must be a multiple of this number).
      noyx::size_t pgAllocGranuality_;

      /**
       * @brief Internal metadata store mapping base addresses to free-page bitmaps.
       *
       * The template parameter is derived from virtualSpaceSize_ / virtualSpaceForReserve_.
       * Actual storage and more complex structures live in the .cpp file.
       */
      VirtualBaseMap map_;

      /// Number of reserved bytes (atomic for concurrent reads).
      std::atomic<uint64> reservedBytes_;

      /// Number of committed bytes (atomic for concurrent reads).
      std::atomic<uint64> committedBytes_;

      /// Number of commits done (atomic).
      std::atomic<uint64> decommitsCount_;
      std::atomic<uint64> commitsCount_;

      // Additional constants and maps (e.g. size classes, alignment classes) are expected
      // to be implemented in the .cpp and are intentionally omitted here.
    };
  } // namespace detail
} // namespace noyx