/* SPDX-License-Identifier: Apache-2.0 /
/*
 * \file   CommitBitmap.hpp
 * \brief
 *
 * Copyright (c) 2026 Project Contributors
 * \author MaksymRbkh <https://github.com/MaksymRbkh>
 * \date   14.01.2026
 */

#pragma once
#include <cstddef>
#include <cstdint>
#include <vector>

#include "platform/assert.hpp"

namespace noyxcore::memory {
  class CommitBitmap {
  public:
    CommitBitmap(std::size_t totalPages)
      : total_pages_(totalPages),
        words_((totalPages + 63) / 64, 0) {}

    void setRange(std::size_t start, std::size_t count) {
      if (count == 0) return;

      size_t end = start + count;

      size_t firstWord = start / 64;
      size_t lastWord = (end - 1) / 64;

      size_t firstBit = start % 64;
      size_t lastBit = (end - 1) % 64;

      if (firstWord == lastWord) {
        uint64_t mask =
            ((~0ULL >> (63 - (lastBit - firstBit))) << firstBit);

        words_[firstWord] |= mask;
        return;
      }

      if (firstBit != 0) {
        uint64_t mask = ~0ULL << firstBit;
        words_[firstWord] |= mask;
        firstWord++;
      }

      for (size_t w = firstWord; w < lastWord; ++w) {
        words_[w] = ~0ULL;
      }

      uint64_t mask = (lastBit == 63) ? ~0ULL : (1ULL << (lastBit + 1)) - 1;
      words_[lastWord] |= mask;
    }

    void clearRange(std::size_t start, std::size_t count) {
      if (count == 0) return;

      size_t end = start + count;

      size_t firstWord = start / 64;
      size_t lastWord = (end - 1) / 64;

      size_t firstBit = start % 64;
      size_t lastBit = (end - 1) % 64;

      if (firstWord == lastWord) {
        uint64_t mask =
            ((~0ULL >> (63 - (lastBit - firstBit))) << firstBit);

        words_[firstWord] &= ~mask;
        return;
      }

      if (firstBit != 0) {
        uint64_t mask = ~0ULL << firstBit;
        words_[firstWord] &= ~mask;
        firstWord++;
      }

      for (size_t w = firstWord; w < lastWord; ++w) {
        words_[w] = 0;
      }

      uint64_t mask = (lastBit == 63) ? ~0ULL : (1ULL << (lastBit + 1)) - 1;
      words_[lastWord] &= ~mask;
    }

    bool isSet(std::size_t page) const {
      std::size_t word = page / 64;
      std::size_t bit = page % 64;

      return (words_[word] >> bit) & 1ULL;
    }

    template<typename Fn>
void forEachClear(std::size_t start, std::size_t count, Fn&& fn) {
      std::size_t end = start + count;
      std::size_t i = start;

      while (i < end) {
        if (isSet(i)) {
          ++i;
          continue;
        }

        std::size_t rangeStart = i;
        while (i < end && !isSet(i)) {
          ++i;
        }
        fn(rangeStart, i - rangeStart);
      }
    }

  private:
    std::size_t total_pages_;
    std::vector<uint64_t> words_;
  };
}
