#pragma once 
#include "PhysicalFrameManager.hpp"
#include "VMASpaceManager.hpp"

namespace noyx {
  namespace memory {
    //Defragmentation / coalescence of regions can run in the background.
    class VMACompactor;
    //file/swap area, compression, asynchronous I/O.
    class SwapMG;
    class VMAManager {
    public:
      VMAManager& operator=(const VMAManager&&) = delete;
      VMAManager(const VMAManager&&) = delete;
      VMAManager() {
          
      };

    private:
      detail::VMASpaceMG VMASpace_;
      detail::VMAPhysicalFrameMG VMAPhysicalSpace_;
      VMACompactor Compactor_;
      SwapMG PageSwapper_;
    };
  }
}