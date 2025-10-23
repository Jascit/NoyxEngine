#pragma once

namespace noyx {
  namespace detail {

      struct FreeBlock {
          FreeBlock* next;
          FreeBlock* prev;
      };

      class BuddyAllocator {
      public:
          BuddyAllocator(void* memoryPoolStart, size_t poolSize) {
              m_poolStart = reinterpret_cast<uintptr_t>(memoryPoolStart);
              m_totalSize = poolSize;

              assert((m_poolStart % MIN_BLOCK_SIZE) == 0);
              assert(m_totalSize >= MIN_BLOCK_SIZE);

              size_t blocksCount = m_totalSize / MIN_BLOCK_SIZE;
              assert((blocksCount & (blocksCount - 1)) == 0);

              m_maxOrder = static_cast<size_t>(log2(blocksCount));

              m_freeLists.resize(m_maxOrder + 1, nullptr);

              FreeBlock* initialBlock = reinterpret_cast<FreeBlock*>(m_poolStart);
              listPush(m_maxOrder, initialBlock);
          }

          BuddyAllocator(const BuddyAllocator&) = delete;
          BuddyAllocator& operator=(const BuddyAllocator&) = delete;

          void* allocate(size_t size) {
              size_t requiredOrder = getOrder(size);
              if (requiredOrder > m_maxOrder) {
                  return nullptr; 
              }

              size_t currentOrder = requiredOrder;
              while (currentOrder <= m_maxOrder && m_freeLists[currentOrder] == nullptr) {
                  currentOrder++;
              }

              if (currentOrder > m_maxOrder) {
                  return nullptr;
              }

              FreeBlock* blockToSplit = m_freeLists[currentOrder];
              listRemove(currentOrder, blockToSplit);

              while (currentOrder > requiredOrder) {
                  currentOrder--;
                  size_t currentBlockSize = MIN_BLOCK_SIZE << currentOrder;

                  uintptr_t buddyAddr = reinterpret_cast<uintptr_t>(blockToSplit) + currentBlockSize;
                  FreeBlock* buddy = reinterpret_cast<FreeBlock*>(buddyAddr);

                  listPush(currentOrder, buddy);
              }

              return blockToSplit;
          }

          void deallocate(void* ptr, size_t size) {
              if (ptr == nullptr) return;

              size_t order = getOrder(size);
              uintptr_t blockAddr = reinterpret_cast<uintptr_t>(ptr);

              while (order < m_maxOrder) {
                  uintptr_t buddyAddr = getBuddyAddress(blockAddr, order);

                  FreeBlock* buddy = nullptr;
                  FreeBlock* current = m_freeLists[order];
                  while (current != nullptr) {
                      if (reinterpret_cast<uintptr_t>(current) == buddyAddr) {
                          buddy = current;
                          break;
                      }
                      current = current->next;
                  }

                  if (buddy == nullptr) {
                      break;
                  }

                  listRemove(order, buddy);

                  if (buddyAddr < blockAddr) {
                      blockAddr = buddyAddr;
                  }
                  order++;
              }
              FreeBlock* finalBlock = reinterpret_cast<FreeBlock*>(blockAddr);
              listPush(order, finalBlock);
          }

      private:
          static constexpr size_t MIN_BLOCK_SIZE = 4096;

          uintptr_t m_poolStart;

          size_t m_totalSize;

          size_t m_maxOrder;

          std::vector<FreeBlock*> m_freeLists;
          size_t getOrder(size_t size) const {
              if (size <= MIN_BLOCK_SIZE) {
                  return 0;
              }
              size_t actualSize = 1;
              while (actualSize < size) {
                  actualSize <<= 1;
              }
              return static_cast<size_t>(log2(actualSize / MIN_BLOCK_SIZE));
          }

          void listPush(size_t order, FreeBlock* block) {
              block->next = m_freeLists[order];
              block->prev = nullptr;
              if (m_freeLists[order] != nullptr) {
                  m_freeLists[order]->prev = block;
              }
              m_freeLists[order] = block;
          }

          void listRemove(size_t order, FreeBlock* block) {
              if (block->prev != nullptr) {
                  block->prev->next = block->next;
              }
              else {
                  m_freeLists[order] = block->next;
              }
              if (block->next != nullptr) {
                  block->next->prev = block->prev;
              }
          }

          uintptr_t getBuddyAddress(uintptr_t blockAddr, size_t order) const {
              size_t blockSize = MIN_BLOCK_SIZE << order;
              return (blockAddr - m_poolStart) ^ blockSize + m_poolStart;
          }
      };



    class FrameBitmap
    {
    private:
        uint8_t* bitmap;
        size_t frameCount;

    public:
        FrameBitmap(uint8_t* buffer, size_t totalFrames) : bitmap(buffer), frameCount(totalFrames) {};

        void set(size_t frameIndex)
        {
            if (frameIndex >= frameCount) return;
            size_t byteIndex = frameIndex / 8;
            uint8_t mask = static_cast<uint8_t>(1u << (frameIndex % 8));
            bitmap[byteIndex] |= mask;
        }
        
        void clear(size_t frameIndex) {
            if (frameIndex >= frameCount) return;
            size_t byteIndex = frameIndex / 8;
            uint8_t mask = static_cast<uint8_t>(1u << (frameIndex % 8));
            bitmap[byteIndex] &= static_cast<uint8_t>(~mask);
        }

        bool test(size_t frameIndex) const {
            if (frameIndex >= frameCount) return false;
            size_t byteIndex = frameIndex / 8;
            uint8_t mask = static_cast<uint8_t>(1u << (frameIndex % 8));
            return (bitmap[byteIndex] & mask) != 0;
        }

        size_t find_Free(size_t i = 0; i < frameCount; i++)
        {
            if (!test[i]) return i;
            return (size_t)-1;
        }
    };


    // frame/physical memory management (buddy, bitmap, page frame allocator).
    class VMAPhysicalFrameMG {

    };
  }
}