/* SPDX-License-Identifier: Apache-2.0 */

/**
 * @file     AllocationMap.hpp
 * @brief     
 *
 * @date     14.01.2026
 *
 * @copyright Copyright (c) 2026 The Project Contributors
 */

#pragma once
#include <map>
#include <optional>

namespace noyxcore::memory {
  class AllocationMap {
  public:
    AllocationMap(std::size_t totalPages) {
      Span initial;
      initial.start_ = 0;
      initial.length_ = totalPages;
      initial.free_ = true;
      auto it = spans_.emplace(0, initial).first;
      insertFree(it);
    }

    std::optional<size_t> allocate(size_t pages) {
      if (pages == 0) return std::nullopt;
      auto fit = free_by_size_.lower_bound(pages);
      if (fit == free_by_size_.end()) return std::nullopt;

      auto spanIt = fit->second;
      removeFree(spanIt);

      Span& span = spanIt->second;
      std::size_t start_ = span.start_;
      if (span.length_ > pages) {
        Span newSpan{span.start_ + pages, span.length_ - pages, true};
        span.length_ = pages;
        span.free_ = false;
        auto newIt = spans_.emplace(newSpan.start_, newSpan).first;
        insertFree(newIt);
      } else {
        span.free_ = false;
      }
      return start_;
    }

    void free(size_t start_Page) {
      auto it = spans_.find(start_Page);
      if (it == spans_.end()) return;
      if (it->second.free_) return;

      it->second.free_ = true;
      it = Merge(it);
      insertFree(it);
    }

    bool isEmpty() const {
      return (spans_.size() == 1 && spans_.begin()->second.free_);
    }

    bool isFull() const {
      return free_by_size_.empty();
    }

  private:
    struct Span {
      std::size_t start_  = 0;
      std::size_t length_ = 0;
      bool free_ = false;

      std::multimap<
        std::size_t,
        std::map<std::size_t, Span>::iterator
      >::iterator freeIt;
    };

    void insertFree(auto it) {
      auto freeIt = free_by_size_.emplace(it->second.length_, it);
      it->second.freeIt = freeIt;
    }

    void removeFree(auto it) {
      free_by_size_.erase(it->second.freeIt);
    }

    std::map<size_t, Span>::iterator Merge(std::map<size_t, Span>::iterator it) {
      if (it != spans_.begin()) {
        auto prev = std::prev(it);
        if (prev->second.free_) {
          removeFree(prev);
          prev->second.length_ += it->second.length_;
          spans_.erase(it);
          it = prev;
        }
      }

      auto next = std::next(it);
      if (next != spans_.end() && next->second.free_) {
        removeFree(next);
        it->second.length_ += next->second.length_;
        spans_.erase(next);
      }

      return it;
    }

  private:
    std::map<size_t, Span> spans_;
    std::multimap<size_t, std::map<size_t, Span>::iterator> free_by_size_;
  };
}
