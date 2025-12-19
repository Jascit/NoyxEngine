#include "memory/vma_space_manager.hpp"
using noyx::detail::VMASpaceMG;

VMASpaceMG::VMASpaceMG(noyx::size_t size, noyx::size_t pgGranularity, noyx::size_t pgAllocGranuality, noyx::size_t firstCommit = 0) :
  virtualSpaceSize_(size), virtualSpaceForReserve_((virtualSpaceSize_ / 32)),
  pgGranularity_(pgGranularity), pgAllocGranuality_(pgAllocGranuality), map_(virtualSpaceForReserve_ / pgGranularity),
  reservedBytes_{ 0 }, committedBytes_{ 0 }, commitsCount_{ 0 } {

  noyx::uint64 bytesToReserve = ((noyx::uint64)16 * 1024 * 1024 * 1024 + virtualSpaceForReserve_ - 1);
  noyx::uint64 cycleCount = bytesToReserve / virtualSpaceForReserve_;
  reservedBytes_.store(bytesToReserve);
  for (noyx::size_t i = 0; i < cycleCount; i++)
  {
    map_.push(reserveRange(virtualSpaceForReserve_));
  }
  if (firstCommit != 0)
  {
    noyx::uint64 bytesToCommit = firstCommit;
    for (noyx::size_t i = 0; bytesToCommit > 0; ++i) {
      noyx::uint64 chunk = std::min(bytesToCommit, virtualSpaceForReserve_);
      commitPages(map_[i].first, chunk, PAGE_NOACCESS);
      bytesToCommit -= chunk; // may overflow
    }
    committedBytes_.store(firstCommit);
    commitsCount_++;
  }
}

VMASpaceMG::~VMASpaceMG() {

}

bool VMASpaceMG::init() {
  return false;
}

void VMASpaceMG::shutdown() {

}

void* VMASpaceMG::reserveRange(noyx::uint64 size, noyx::size_t alignment = 0) {
  return PlatformVirtualAlloc(nullptr, size, MEM_RESERVE, PAGE_NOACCESS);
}

bool VMASpaceMG::unlockCommitCheck(void* base, void* addr, noyx::uint64 pageCount) {
  auto& bitmap = map_.find_bitmap(base);
  noyx::uintptr diff = (noyx::uintptr)addr - (noyx::uintptr)base;
  noyx::size_t startIdx = diff / pgGranularity_;
  noyx::uint64 tmp = pageCount;
  size_t idx = startIdx;

  while (tmp >= 64) {
    if (bitmap[idx++] != 0) return false;
    tmp -= 64;
  }

  if (tmp > 0) {
    noyx::uint64 mask = (~0ULL) >> (64 - tmp);
    if ((bitmap[idx] & mask) != 0) return false;
  }

  return true;
}

bool VMASpaceMG::unlockDecommitCheck(void* base, void* addr, noyx::uint64 pageCount) {
  auto& bitmap = map_.find_bitmap(base);
  noyx::uintptr diff = (noyx::uintptr)addr - (noyx::uintptr)base;
  size_t startIdx = diff / pgGranularity_;

  noyx::uint64 tmp = pageCount;
  static const noyx::uint64 cond = ~0ULL;
  size_t idx = startIdx;

  while (tmp >= 64) {
    if (bitmap[idx++] != cond) return false;
    tmp -= 64;
  }

  if (tmp > 0) {
    noyx::uint64 mask = (~0ULL) >> (64 - tmp);
    if ((bitmap[idx] & mask) != mask) return false;
  }

  return true;
}

bool VMASpaceMG::commitPages(void* base, noyx::uint64 size, noyx::uint32 protect = PAGE_READWRITE) {
  noyx::uint64 pageCount = (size + pgGranularity_ - 1) / pgGranularity_;
  auto foundedBase = map_.find_base(base);
  if (unlockCommitCheck(foundedBase, base, pageCount) == false) return false;
  PlatformVirtualAlloc(base, size, MEM_COMMIT, protect);
  commitsCount_++;
}

bool VMASpaceMG::decommitPages(void* base, noyx::uint64 size) {
  noyx::uint64 pageCount = (size + pgGranularity_ - 1) / pgGranularity_;
  auto foundedBase = map_.find_base(base);
  if (unlockDecommitCheck(foundedBase, base, pageCount) == false) return false;
  PlatformVirtualAlloc(base, size, MEM_DECOMMIT, PAGE_NOACCESS);
  decommitsCount_++;
}

VMASpaceMG::Stats VMASpaceMG::getStats() const {

}

void VMASpaceMG::lockForRead() const {

}

void VMASpaceMG::unlockForRead() const {

}

void VMASpaceMG::lockForWrite() {

}

void VMASpaceMG::unlockForWrite() {

}