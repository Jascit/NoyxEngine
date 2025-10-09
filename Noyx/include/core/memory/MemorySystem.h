#include "typedef.hpp"

namespace NOYX {
  namespace CORE {
    namespace MEMORY {
      struct Page {
        void* base;            // memory start
        size_t size;           // 4KB
        uint32_t ownerId;      // meneger owner, if there is one
        uint16_t sizeClass;    // 0 = variable/extent, >0 = slab bucket index
        uint16_t freeBytes;
        uint8_t* bitmap;       // для slab: 1 bit per block
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
}