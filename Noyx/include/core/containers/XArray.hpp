#pragma once

#include "xmap.hpp"
#include "traits.hpp"
#include <array>
#include <map>
#include <cstdint>
#include <memory>
#include <optional>
#include <shared_mutex>
#include <type_traits>
#include <iterator>
#include <functional>

namespace noyx {
  namespace containers {

    // XArray skeleton:
    // - K must be integral <= 64 bits (we treat index as 64-bit)
    // - BITS_PER_LEVEL controls fanout (default 6 -> fanout 64)
    template <
      typename Key,
      typename T,
      unsigned BITS_PER_LEVEL = 6,
      uint16_t LEVELS = (64 + BITS_PER_LEVEL - 1) / BITS_PER_LEVEL,
      typename Allocator = std::allocator<T>
    >
    class XArray {
    public:
      static_assert(std::is_integral_v<Key> && sizeof(Key) <= 8,
        "XArray key must be an integral type up to 64 bits.");
      static_assert(BITS_PER_LEVEL > 0 && BITS_PER_LEVEL < 64,
        "BITS_PER_LEVEL must be in (0,64).");

      using key_type = Key;               // logical index type
      using mapped_type = T;
      using value_type = mapped_type;
      using size_type = std::size_t;
      using difference_type = std::ptrdiff_t;
      using allocator_type = Allocator;
      using reference = value_type&;
      using const_reference = const value_type&;
      using pointer = typename std::allocator_traits<allocator_type>::pointer;
      using const_pointer = typename std::allocator_traits<allocator_type>::const_pointer;

      // constants
      static constexpr unsigned FANOUT = 1u << BITS_PER_LEVEL;

      using Node = noyx::detail::IXArrayNode<value_type>;
      using NodeToAllocate = noyx::detail::HXArrayNode<mapped_type>;

    private:
      // allocator rebound to Node
      using _NodeAlloc = typename std::allocator_traits<allocator_type>:: template rebind_alloc<NodeToAllocate>;
      //allocator for nodes with children
      allocator_type allocator_data_;
      _NodeAlloc alloc_nodes_;
      Node* root_ = nullptr;
      size_type size_;

      // concurrency primitive: shared lock for readers, unique for writers.
      // In production you'd probably want per-node locking or RCU; this is the simple approach.
      mutable std::shared_mutex _mtx;

    public:
      // iterator skeleton: iterate over present (index,value) pairs in ascending index order.

      using iterator = XArrayIterator<TMapTraits<Key, T, Allocator>>;
      using const_iterator = iterator;
      using reverse_iterator = std::reverse_iterator<iterator>;
      using const_reverse_iterator = reverse_iterator;

      // ctors / dtor / assignment
      XArray() {
        root_ = alloc_nodes_.allocate(FANOUT);
      };

      XArray(const allocator_type& alloc, const _NodeAlloc& alloc_nodes) : alloc_nodes(alloc_nodes), allocator_data_(alloc) {};

      XArray(const XArray& other) = delete;
      XArray& operator=(const XArray& other) = delete;
      XArray(XArray&& other) noexcept {

      };

      XArray& operator=(XArray&& other) noexcept {

      };

      Node& operator[](const key_type& key) noexcept {

      };

      ~XArray() {

      };

      // capacity
      bool empty() const noexcept {
        return size_ != 0;
      }

      size_type size() const noexcept {
        return size_;
      }

      // modifiers
      // insert or overwrite value at index
      void insert(const key_type& index, mapped_type value) {

      }
      // remove value at index; returns true if removed
      bool erase(const key_type& index);
      void clear() noexcept;

      // lookup
      //std::optional<mapped_type> lookup(const key_type& index) const;

      // collect range [start, end] inclusive
      //std::vector<std::pair<key_type, mapped_type>>
      //  collect_range(const key_type& start, const key_type& end) const {

      //};

      // iterators (begin/end) - declarations only
      iterator begin() noexcept {

      }

      const_iterator begin() const noexcept {

      }

      iterator end() noexcept {

      }

      const_iterator end() const noexcept {

      }

      reverse_iterator rbegin() noexcept {

      }

      const_reverse_iterator rbegin() const noexcept {

      }

      reverse_iterator rend() noexcept {

      }

      const_reverse_iterator rend() const noexcept {

      }

    private:
      // helpers

      // compute slot for given level (0 = topmost, LEVELS-1 = leaf level)
      static inline unsigned slot_for(uint64_t index, unsigned level) noexcept {
        // compute shift: number of bits to shift right to get the slice for this level
        int shift = static_cast<int>(64) - static_cast<int>(BITS_PER_LEVEL) * static_cast<int>(level + 1);
        if (shift < 0) shift = 0;
        return static_cast<unsigned>((index >> shift) & (FANOUT - 1));
      }

      // node allocation / destruction
      Node* _allocate_node();
      void _destroy_node(Node* n) noexcept;
      void _destroy_subtree(Node* n) noexcept;

      // unlocked internal helpers (caller must hold appropriate lock)
      size_type _size_unlocked(const Node* root) const noexcept;
      //std::optional<mapped_type> _lookup_unlocked(const Node* root, const key_type& index) const noexcept;
      bool _insert_unlocked(const key_type& index, mapped_type&& value);
      bool _erase_unlocked(const key_type& index);
      void _collect_range_unlocked(const Node* root, const key_type& start, const key_type& end,
        std::vector<std::pair<std::pair<key_type, key_type>, mapped_type>>& out) const;

      // debug / utility
      bool _node_empty_unlocked(const Node* n) const noexcept;
    };

  } // namespace containers
} // namespace noyx
