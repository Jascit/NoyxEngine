/* SPDX-License-Identifier: Apache-2.0 */

/**
 * @file    RAMManager.cpp
 * @brief   
 *
 * @date    30.04.2026
 *
 * @copyright Copyright (c) 2026 The Project Contributors
 */

#include <filesystem>
#include <memory/RAMManager.hpp>
#include <type_traits>

#include "memory/vas_wrapper.hpp"
#include "platform/os/SystemInfo.hpp"

noyxcore::memory::RAMManager::RAMRegion::RAMRegion(void* base, std::uint64_t size,
                                                   std::size_t allocation_granularity) : m_base(base), m_size(size),
  m_allocated(0), m_allocation_granularity(allocation_granularity) {
  m_bitmap.reserve(m_size);
  m_free_ranges.emplace(std::pair<const size_t, size_t>(0, size));
}

noyxcore::memory::RAMManager::RAMRegion::RAMRegion(RAMRegion&& other) noexcept
  : m_bitmap(std::move(other.m_bitmap))
    , m_free_ranges(std::move(other.m_free_ranges))
    , m_base(std::exchange(other.m_base, nullptr))
    , m_allocation_granularity(other.m_allocation_granularity)
    , m_allocated(std::exchange(other.m_allocated, 0))
    , m_size(std::exchange(other.m_size, 0)) {}


template<bool BitToChange>
bool noyxcore::memory::RAMManager::RAMRegion::change_bits(std::size_t count_of_bits, std::size_t index) noexcept {
  auto [bitmap_index, offset] = bit_position(index);

  std::size_t word = bitmap_index;
  std::size_t bit_offset = offset;
  std::size_t remaining_bits = count_of_bits;
  std::size_t taken_bits = 0;

  while (remaining_bits > 0) {
    std::size_t take = std::min(remaining_bits, 64 - bit_offset);
    std::size_t mask;
    if (take == 64) {
      mask = ~std::size_t(0);
    } else {
      mask = ((std::size_t(1) << take) - 1) << bit_offset;
      bit_offset = 0;
    }
    // recall bits
    if constexpr (BitToChange == 0) {
      if (m_bitmap[word] & mask) {
        change_bits<1>(taken_bits, index);
        return false;
      }
    } else {
      if (~m_bitmap[word] & mask) {
        change_bits<0>(taken_bits, index);
        return false;
      }
    }

    if constexpr (BitToChange == 0) {
      m_bitmap[word] |= mask;
    } else {
      m_bitmap[word] &= ~mask;
    }

    remaining_bits -= take;
    taken_bits += take;
    ++word;
  }
  return true;
};

std::pair<void*, size_t> noyxcore::memory::RAMManager::RAMRegion::allocate(std::size_t size) {
  if (size == 0) {
    return std::make_pair(nullptr, 0);
  }
  std::size_t aligned_size = (size + m_allocation_granularity - 1) / m_allocation_granularity;
  std::size_t count_of_bits_to_change = aligned_size;

  auto index_it = m_free_ranges.lower_bound(count_of_bits_to_change);
  if (index_it == m_free_ranges.end()) {
    return std::make_pair(nullptr, 0);
  }

  bool succeeded = change_bits<0>(count_of_bits_to_change, index_it->second);
  if (!succeeded) {
    return std::make_pair(nullptr, 0);
  }

  auto [old_size, old_index] = *index_it;
  m_free_ranges.erase(index_it);
  if (old_size - count_of_bits_to_change > 0) {
    m_free_ranges.emplace(
      std::pair<const size_t, size_t>(old_size - count_of_bits_to_change, old_index + count_of_bits_to_change));
  }

  m_allocated += count_of_bits_to_change;
  void* result_addr = ptr_from_index(old_index);
  return std::make_pair(result_addr, aligned_size);
}

void noyxcore::memory::RAMManager::RAMRegion::free(void* ptr, std::uint64_t size) {
  if (size % m_allocation_granularity != 0) {
    // err: misaligned size
    return;
  }

  const std::size_t index = index_from_ptr(ptr);
  const std::size_t count = size / m_allocation_granularity;

  if (!change_bits<1>(count, index)) {
    // err: double-free or corrupt bitmap
    return;
  }

  std::size_t merged_index = index;
  std::size_t merged_count = count;

  auto [left, right] = check_collisions(index, count);

  if (left) {
    // find the range that ends exactly at our start
    auto it = m_free_ranges.lower_bound(index);
    if (it != m_free_ranges.begin()) {
      --it;
      if (it->first + it->second == index) {
        // truly adjacent?
        merged_index = it->first;
        merged_count += it->second;
        m_free_ranges.erase(it);
      }
    }
  }

  if (right) {
    // find the range that starts exactly where we end
    auto it = m_free_ranges.find(index + count);
    if (it != m_free_ranges.end()) {
      // truly adjacent?
      merged_count += it->second;
      m_free_ranges.erase(it);
    }
  }

  m_free_ranges.emplace(merged_index, merged_count);
  m_allocated -= count;
}

noyxcore::memory::RAMManager::RAMManager(IVirtualMemoryAllocator& allocator,
                                         std::size_t initial_reserve,
                                         std::size_t allocation_granularity) : m_allocation_granularity(
                                                                                 allocation_granularity),
                                                                               m_allocator(std::addressof(allocator)) {
  std::size_t aligned_reserve_size = (initial_reserve + allocation_granularity - 1) / allocation_granularity;
  void* virtual_address = m_allocator->allocate(aligned_reserve_size);
  vas::CommitRequest request;
  request.base = virtual_address;
  request.size = aligned_reserve_size;
  request.offset = 0;
  request.protection = static_cast<std::uint32_t>(vas::Flag::PROT_NONE);
  request.alloc_flags = static_cast<std::uint32_t>(vas::Flag::PAGE_64KB);
  vas::CommitResponse response = vas::commit_pages(request, platform::SystemInfo::page_size);
  if (response.err != vas::Error::OK) return; //TODO: err

  RAMRegion* region = std::addressof(
    m_regions.emplace_back(RAMRegion(virtual_address, aligned_reserve_size, m_allocation_granularity)));
  m_free_by_size.emplace(std::make_pair(aligned_reserve_size, region));
  m_owner_by_ptr.emplace(std::make_pair(virtual_address, region));
}

void* noyxcore::memory::RAMManager::allocate(std::size_t size) {
  if (size == 0) {
    return nullptr;
  }
  m_free_by_size.erase(m_free_by_size.begin());
}

void noyxcore::memory::RAMManager::free(void* ptr) {}
void noyxcore::memory::RAMManager::reserve(std::size_t size) {}
void noyxcore::memory::RAMManager::release(std::size_t size) {}
std::size_t noyxcore::memory::RAMManager::total_allocated() const noexcept {}
std::size_t noyxcore::memory::RAMManager::total_reserved() const noexcept {}
std::size_t noyxcore::memory::RAMManager::alignment() const noexcept {}
