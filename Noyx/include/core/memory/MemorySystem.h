#include "typedef.hpp"

namespace noyx {
  namespace memory {
    //count of slabs
    enum class BlockSize : uint8 {
      Variable = 0,
      B2 = 2,
      B4 = 4,
      B8 = 8,
      B16 = 16,
      B32 = 32,
      B64 = 64
    };

    // Page metadata. Compact, POD-like. bitmap valid only for slab pages
    struct Page {
      void* base = nullptr;   // start of memory region (page-aligned)
      noyx::size_t size = 0;        // page size in bytes (e.g. 4096)
      noyx::uint32 ownerId = 0;        // manager id or sentinel
      BlockSize   sizeClass = BlockSize::Variable; // 0 => extent/variable
      noyx::uint16 freeCount = 0;        // number of free blocks (for slabs)
      noyx::uint64 bitmap = 0;        // 1 bit per block (1 = free OR used — choose convention)

      // --- semantics notes ---
      // bitmap: bit i corresponds to block i in page. Convention: 1 = free, 0 = used (or vice versa).
      // Use helpers below to avoid confusion.
    };
    class MemorySystem {
    public:
    private:
    public:
      MemorySystem(const MemorySystem&) = delete;
      MemorySystem operator=(const MemorySystem&) = delete;
      static const MemorySystem& Get() {
        static MemorySystem instance;
        return instance;
      }
    private:
      MemorySystem() = default;
    };
  }
}