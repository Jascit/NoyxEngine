//
// Created by Maksym Riabykh on 10.02.2026.
//

#include <map>
#include <optional>

namespace noyxcore::memory {
  class AllocationMap {
  public:
    AllocationMap(std::size_t totalPages) {
      Span initial;
      initial.start = 0;
      initial.length = totalPages;
      initial.free = true;
      auto it = m_spans.emplace(0, initial).first;
      InsertFree(it);
    }

    std::optional<size_t> Allocate(size_t pages) {
      if (pages == 0) return std::nullopt;
      auto fit = m_freeBySize.lower_bound(pages);
      if (fit == m_freeBySize.end()) return std::nullopt;

      auto spanIt = fit->second;
      RemoveFree(spanIt);

      Span& span = spanIt->second;
      std::size_t start = span.start;
      if (span.length > pages) {
        Span newSpan{span.start + pages, span.length - pages, true};
        span.length = pages;
        span.free = false;
        auto newIt = m_spans.emplace(newSpan.start, newSpan).first;
        InsertFree(newIt);
      } else {
        span.free = false;
      }
      return start;
    }

    void Free(size_t startPage) {
      auto it = m_spans.find(startPage);
      if (it == m_spans.end()) return;
      if (it->second.free) return;

      it->second.free = true;
      it = Merge(it);
      InsertFree(it);
    }

    bool IsEmpty() const {
      return (m_spans.size() == 1 && m_spans.begin()->second.free);
    }

  private:
    struct Span {
      std::size_t start  = 0;
      std::size_t length = 0;
      bool free = false;

      std::multimap<
        std::size_t,
        std::map<std::size_t, Span>::iterator
      >::iterator freeIt;
    };

    void InsertFree(auto it) {
      auto freeIt = m_freeBySize.emplace(it->second.length, it);
      it->second.freeIt = freeIt;
    }

    void RemoveFree(auto it) {
      m_freeBySize.erase(it->second.freeIt);
    }

    std::map<size_t, Span>::iterator Merge(std::map<size_t, Span>::iterator it) {
      if (it != m_spans.begin()) {
        auto prev = std::prev(it);
        if (prev->second.free) {
          RemoveFree(prev);
          prev->second.length += it->second.length;
          m_spans.erase(it);
          it = prev;
        }
      }

      auto next = std::next(it);
      if (next != m_spans.end() && next->second.free) {
        RemoveFree(next);
        it->second.length += next->second.length;
        m_spans.erase(next);
      }
      return it;
    }

  private:
    std::map<size_t, Span> m_spans;
    std::multimap<size_t, std::map<size_t, Span>::iterator> m_freeBySize;
  };
}
