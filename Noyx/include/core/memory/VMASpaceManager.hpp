#pragma once

#include <Windows.h> 
#include <memory>
#include "typedef.hpp"

namespace noyx {
  namespace detail {
    class RadixMap; 
    struct PageMetadata;
    //reserve | free virtual memory space, hold metadata in radix tree
    class VMASpaceMG {
    public:
      VMASpaceMG& operator=(const VMASpaceMG&&) = delete;
      VMASpaceMG(const VMASpaceMG&&) = delete;
      VMASpaceMG() {
        virtualBase_ = VirtualAlloc(NULL, virtualSpaceSize_, MEM_RESERVE, PAGE_READWRITE);
      }
    private:
      const uint64 virtualSpaceSize_ = 200ULL * 1024 * 1024 * 1024;
      void* virtualBase_ = nullptr;

    };
  }
}