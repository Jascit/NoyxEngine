//
// Created by Maksym Riabykh on 09.02.2026.
//

#include <cstddef>
#include <mutex>
#include <memory/Physical/CommitBitmap.hpp>
#include <memory/Physical/AllocationMap.hpp>
#include <memory/Physical/VirtualMemory.hpp>

namespace noyxcore::memory {
  class Block {
  public:
    explicit Block(void* baseAddr, std::size_t totalPages, std::size_t pageSize)
      : m_baseAddr(baseAddr), m_totalPages(totalPages), m_pageSize(pageSize), m_commitBitmap(totalPages),
        m_allocMap(totalPages) {}

    Block& operator=(const Block&) = delete;
    Block(const Block&) = delete;
    Block(Block&&) = default;
    Block& operator=(Block&&) = default;

    ~Block() = default;

    void* Allocate(std::size_t pages) {
      std::lock_guard<std::mutex> lock(m_mutex);
      auto result = m_allocMap.Allocate(pages);
      if (!result) return nullptr;

      auto startPage = *result;
      bool commitFailed = false;

      m_commitBitmap.ForEachClear(startPage, pages, [&] (std::size_t s, std::size_t c) {
        if (commitFailed) return;
        void* addr = static_cast<char*>(m_baseAddr) + s * m_pageSize;
        if (!OS::Commit(addr, c * m_pageSize)) {
          commitFailed = true;
        }
      });
      if (commitFailed) {
        m_allocMap.Free(startPage);
        return nullptr;
      }
      m_commitBitmap.SetRange(startPage, pages);
      return static_cast<char*>(m_baseAddr) + startPage * m_pageSize;
    }

    void* Free(void* ptr) {
      std::lock_guard<std::mutex> lock(m_mutex);
      auto startPage = (static_cast<char*>(ptr) - static_cast<char*>(m_baseAddr)) / m_pageSize;
      m_allocMap.Free(startPage);
    }

    bool IsEmpty() const{
      std::lock_guard<std::mutex> lock(m_mutex);
      return m_allocMap.IsEmpty();
    }

  private:
    void* m_baseAddr;
    std::size_t m_totalPages;
    std::size_t m_pageSize;

    AllocationMap m_allocMap;
    CommitBitmap m_commitBitmap;
    mutable std::mutex m_mutex;
  };
}
