//
// Created by Maksym Riabykh on 09.02.2026.
//

#include <cstddef>
#include <cstdint>
#include <vector>

#include "platform/assert.hpp"

namespace noyxcore::memory {
  class CommitBitmap {
  public:
    CommitBitmap(std::size_t totalPages)
      : m_totalPages(totalPages),
        m_words((totalPages + 63) / 64, 0) {}

    void SetRange(std::size_t start, std::size_t count) {
      if (count == 0) return;

      size_t end = start + count;

      size_t firstWord = start / 64;
      size_t lastWord = (end - 1) / 64;

      size_t firstBit = start % 64;
      size_t lastBit = (end - 1) % 64;

      if (firstWord == lastWord) {
        uint64_t mask =
            ((~0ULL >> (63 - (lastBit - firstBit))) << firstBit);

        m_words[firstWord] |= mask;
        return;
      }

      if (firstBit != 0) {
        uint64_t mask = ~0ULL << firstBit;
        m_words[firstWord] |= mask;
        firstWord++;
      }

      for (size_t w = firstWord; w < lastWord; ++w) {
        m_words[w] = ~0ULL;
      }

      uint64_t mask = (lastBit == 63) ? ~0ULL : (1ULL << (lastBit + 1)) - 1;
      m_words[lastWord] |= mask;
    }

    void ClearRange(std::size_t start, std::size_t count) {
      if (count == 0) return;

      size_t end = start + count;

      size_t firstWord = start / 64;
      size_t lastWord = (end - 1) / 64;

      size_t firstBit = start % 64;
      size_t lastBit = (end - 1) % 64;

      if (firstWord == lastWord) {
        uint64_t mask =
            ((~0ULL >> (63 - (lastBit - firstBit))) << firstBit);

        m_words[firstWord] &= ~mask;
        return;
      }

      if (firstBit != 0) {
        uint64_t mask = ~0ULL << firstBit;
        m_words[firstWord] &= ~mask;
        firstWord++;
      }

      for (size_t w = firstWord; w < lastWord; ++w) {
        m_words[w] = 0;
      }

      uint64_t mask = (lastBit == 63) ? ~0ULL : (1ULL << (lastBit + 1)) - 1;
      m_words[lastWord] &= ~mask;
    }

    bool IsSet(std::size_t page) const {
      std::size_t word = page / 64;
      std::size_t bit = page % 64;

      return (m_words[word] >> bit) & 1ULL;
    }

    template<typename Fn>
    void ForEachClear(std::size_t start, std::size_t count, Fn&& fn) {
      std::size_t end = start + count;
      std::size_t i = start;
      while (i < end) {
        if (IsSet(i)) {
          ++i;
          continue;
        }
      }
      std::size_t rangeStart = i;
      while (i < end && !IsSet(i)) {
        ++i;
      }
      fn(rangeStart, i - rangeStart);
    }

  private:
    std::size_t m_totalPages;
    std::vector<uint64_t> m_words;
  };
}
