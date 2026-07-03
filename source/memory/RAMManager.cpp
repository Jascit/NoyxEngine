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

using namespace noyxcore::memory;

RAMManager::RAMRegion::PageBitmap::PageBitmap(std::size_t granularity,
                                              std::size_t total_size) noexcept
    : m_granularity(granularity),
      m_bitmap((total_size / granularity))
{
}

bool RAMManager::RAMRegion::PageBitmap::is_range_free(void *base, void *ptr,
                                                      std::size_t size) const noexcept
{
    auto base_addr = reinterpret_cast<std::uintptr_t>(base);
    auto ptr_addr  = reinterpret_cast<std::uintptr_t>(ptr);

    std::size_t offset_in_bits = (ptr_addr - base_addr) / m_granularity;

    std::size_t left_bits = (size + m_granularity - 1) / m_granularity; // ceil-div

    std::size_t current_pos = offset_in_bits;

    std::size_t remaining = left_bits;

    while (remaining > 0)
    {
        std::size_t local_offset = current_pos % 64;
        std::size_t index        = current_pos / 64;

        std::size_t bits_to_scan = std::min<std::size_t>(64 - local_offset, remaining);

        std::uint64_t mask = (bits_to_scan == 64) ? ~0ULL : ((1ULL << bits_to_scan) - 1ULL);

        std::uint64_t num = (m_bitmap[index] >> local_offset) & mask;

        if (num != 0)
            return false;

        current_pos += bits_to_scan;
        remaining -= bits_to_scan;
    }

    return true;
}

void RAMManager::RAMRegion::PageBitmap::set_range(void *base, void *ptr, std::size_t size,
                                                  bool value) noexcept
{
    const auto base_addr = reinterpret_cast<std::uintptr_t>(base);
    const auto ptr_addr  = reinterpret_cast<std::uintptr_t>(ptr);

    std::size_t offset_in_bits = (ptr_addr - base_addr) / m_granularity;
    std::size_t left_bits      = (size + m_granularity - 1) / m_granularity; // ceil-div

    std::size_t current_pos = offset_in_bits;

    while (left_bits > 0)
    {
        std::size_t local_offset = current_pos % 64;
        std::size_t index        = current_pos / 64;

        std::size_t bits_to_change = std::min<std::size_t>(64 - local_offset, left_bits);

        std::uint64_t field_mask =
            (bits_to_change == 64) ? ~0ULL : ((1ULL << bits_to_change) - 1ULL);

        std::uint64_t mask = field_mask << local_offset;

        if (value)
            m_bitmap[index] |= mask;
        else
            m_bitmap[index] &= ~mask;

        current_pos += bits_to_change;
        left_bits -= bits_to_change;
    }
}