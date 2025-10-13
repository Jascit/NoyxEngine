#pragma once
#include "VMAManager.hpp"
#include "typedef.hpp"

namespace noyx {
  namespace memory {
    //Decisions: when to swap, when to defragment, which size classes are allowed.
    class PolicyEngine;
    class QuotaManager;
    class MetricsCollector;
    //Delegates, communication
    class EventBus;
    class CentralMemoryManager {
    public:
    private:
      VMAManager MemoryAccessManager_;
      PolicyEngine Policy_;
      QuotaManager Quota_;
      MetricsCollector MCollector_;
      EventBus Events_;
    public:
      CentralMemoryManager(const CentralMemoryManager&) = delete;
      CentralMemoryManager operator=(const CentralMemoryManager&) = delete;
      static const CentralMemoryManager& Get() {
        static CentralMemoryManager instance;
        return instance;
      }
    private:

    };
    // Allocate n bytes, optionally aligned
    void* cmm_alloc(size_t size, size_t alignment = 16);

    // Free pointer allocated by cmm_alloc
    void cmm_free(void* ptr);

    // map a file-backed region (for shared memory or swap-backed)
    void* cmm_map_file(const std::wstring& path, size_t size, uint32_t flags);

    // reserve virtual address range without commit
    void* cmm_reserve(size_t size);

    // commit previously reserved region
    bool cmm_commit(void* addr, size_t size, uint32_t protFlags);

    // unmap / decommit
    bool cmm_unmap(void* addr, size_t size);
  }
}