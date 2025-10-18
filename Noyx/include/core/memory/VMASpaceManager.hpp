#pragma once

#include <Windows.h> 
#include <memory>
#include "typedef.hpp"
#include "containers/XArray.hpp"

namespace noyx {
  namespace detail {
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
      const uint64 virtualSpaceSize_ = 100ULL * 1024 * 1024 * 1024;
      void* virtualBase_ = nullptr;

    };
  }
}