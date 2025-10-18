#pragma once
#include <cstdint>
#include <cassert>

namespace noyx {
  namespace detail {
    struct XArrayNodeLeaf {
      XArrayNodeLeaf() = default;
    };

    struct XArrayNodeBottom {
      XArrayNodeBottom() = default;
    };

    template<typename T, typename Flag>
    struct HXArrayNode {
      static_assert(std::_Always_false<T>, "HXArrayNode was created without the correct flag!");
    };

    template<typename T>
    struct IXArrayNode {
      bool has_children() const noexcept {
        // cast to const-derived
        auto const* d = static_cast<HXArrayNode<T, XArrayNodeLeaf> const*>(this);
        return d->_has_children();
      }

      // повертаємо reference на pointer (позначено як ризикований інтерфейс)
      IXArrayNode<T>*& get_children() {
        auto* d = static_cast<HXArrayNode<T, XArrayNodeLeaf>*>(this);
        return d->_get_children();
      }

      bool has_values() const noexcept {
        auto const* d = static_cast<HXArrayNode<T, XArrayNodeBottom> const*>(this);
        return d->_has_values();
      }

      T*& get_values() {
        auto* d = static_cast<HXArrayNode<T, XArrayNodeBottom>*>(this);
        return d->_get_values();
      }
    };

    // Node: contains children (intermediate)
    template<typename T>
    struct HXArrayNode<T, XArrayNodeLeaf> : public IXArrayNode<T> {
      friend struct IXArrayNode<T>;
    public:
      HXArrayNode() noexcept : children_(nullptr) {}
      HXArrayNode(const IXArrayNode<T>* other) noexcept : children_(other) {}
    private:
      // pointer to a contiguous block (or nullptr)
      IXArrayNode<T>* children_;

      bool _has_children() const noexcept {
        return children_ != nullptr;
      }

      IXArrayNode<T>*& _get_children() {
        return children_;
      }
    };

    // Node: leaf (stores values)
    template<typename T>
    struct HXArrayNode<T, XArrayNodeBottom> : public IXArrayNode<T> {
      friend struct IXArrayNode<T>;
    public:
      HXArrayNode() noexcept : data_(nullptr) {}
      HXArrayNode(const IXArrayNode<T>* other) noexcept : data_(nullptr) {}
    private:
      T* data_; // pointer to contiguous T[FANOUT] or nullptr

      bool _has_values() const noexcept {
        return data_ != nullptr;
      }

      T*& _get_values() {
        return data_;
      }
    };
    //CursorImpl for different trees
    template<typename Node>
    struct CursorImpl {};
    // Specialisation for XArray-type trees
    template<typename Node, std::uint8_t LevelCount>
    struct XArrayCursor {
      using value_type = Node;
      using pointer = Node*;
      using reference = Node&;
      using key_type = std::uint64_t;

      XArrayCursor() noexcept
        : currentLevel_{ 0 }, currentKey_{ 0 }, at_end{ true }, levelCount_{ LevelCount } {
      }

      XArrayCursor(const XArrayCursor& other) noexcept
        : currentLevel_{ other.currentLevel_ },
        currentKey_{ other.currentKey_ },
        at_end{ other.at_end },
        levelCount_{ other.levelCount_ }
      {
        for (std::uint8_t i = 0; i < LevelCount; ++i) {
          nodeHistory_[i] = other.nodeHistory_[i];
          indicesHistory_[i] = other.indicesHistory_[i];
        }
      }

      XArrayCursor(XArrayCursor&& other) noexcept
        : currentLevel_{ other.currentLevel_ },
        currentKey_{ other.currentKey_ },
        at_end{ other.at_end },
        levelCount_{ other.levelCount_ }
      {
        for (std::uint8_t i = 0; i < levelCount_; ++i) {
          nodeHistory_[i] = other.nodeHistory_[i];
          indicesHistory_[i] = other.indicesHistory_[i];
          other.nodeHistory_[i] = nullptr;
          other.indicesHistory_[i] = 0;
        }
        other.currentLevel_ = 0;
        other.currentKey_ = 0;
        other.at_end = true;
      }
      std::uint8_t levelCount_;
      pointer nodeHistory_[levelCount_];
      std::uint8_t indicesHistory_[levelCount_];
      std::uint8_t currentLevel_;
      std::uint64_t currentKey_;
      bool at_end;

      void init_to_first(pointer root);
      void init_to_end();
      void advance();
      void retreat();
      key_type current_logical_key() const {
        return currentKey_;
      };

      reference current_value() const {
        if (currentLevel_ == LevelCount)
        {
          return
        }
      };
    };
  } // detail
  //do proxy later, something like IteratorBase for proxy initialisation(std-like variant but with slotmap)
  template<typename Traits, uint8_t LevelCount>
  struct XArrayIterator {
    using iterator_category = std::forward_iterator_tag;
    using value_type = Traits::value_type;
    using pointer = value_type*;
    using Node = Traits::Node;
    using difference_type = std::ptrdiff_t;
    using NodePointer = Node*;
    using reference = value_type&;

    iterator() noexcept : cur_() {};
    explicit iterator(NodePointer node, uint8_t currentLevel) noexcept : cur_() {};

    reference operator*() const;
    pointer operator->() const;
    XArrayIterator& operator++();
    XArrayIterator operator++(int);
    bool operator==(const XArrayIterator& other) const noexcept;
    bool operator!=(const XArrayIterator& other) const noexcept;

  private:
    // internal cursor state: stack of nodes + slot indexes for traversal
    XArrayCursor<Node, LevelCount> cur_;
  };
} // noyx
