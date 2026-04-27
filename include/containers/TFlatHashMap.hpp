/* SPDX-License-Identifier: Apache-2.0 */

/**
 * @file     TFlatHashMap.hpp
 * @brief     
 *
 * @date     09.01.2026
 *
 * @copyright Copyright (c) 2026 The Project Contributors
 */

#include <memory/allocators/traits.hpp>
#include <containers/internal/containers_traits.hpp>

namespace noyxcore::containers {
  namespace details {
    enum ctrl_t : uint8_t {
      kDeleted = 0b11111111,
      kEmpty = 0b10000000
    };

    template<typename Key, typename T>
    union SlotFlatMapType {
      using mutable_key_type = std::remove_const_t<Key>;
      using value_type = std::pair<const mutable_key_type, T>;
      using mutable_value_type = std::pair<mutable_key_type, std::remove_const_t<T>>;

      constexpr SlotFlatMapType() noexcept {};
      constexpr ~SlotFlatMapType() {};

      mutable_value_type mutable_value;
      value_type value;
      mutable_key_type key;
    };

    template<typename Key, typename T>
    class SlotFlatHashMapPolicies {
    public:
      using slot_type = SlotFlatMapType<Key, T>;
      using value_type = slot_type::value_type;
      using mutable_value_type = slot_type::mutable_value_type;
      using key_type = std::remove_const_t<Key>;
      using const_key_type = const key_type;
      static constexpr bool bLayoutCompatible = internal::is_layout_compatible_v<Key, T>;

      FORCE_INLINE constexpr key_type& mutable_key(slot_type* slot) const noexcept {
        if constexpr (bLayoutCompatible) {
          return slot->key;
        } else {
          return *std::launder(const_cast<Key*>(std::addressof(slot->value.first)));
        }
      }

      FORCE_INLINE constexpr const_key_type& key(slot_type* slot) const noexcept {
        if constexpr (bLayoutCompatible) {
          return slot->key;
        } else {
          return slot->value.first;
        }
      }

      template<typename Allocator, typename... Args>
      constexpr void construct(Allocator& alloc, slot_type* slot, Args&&... args)
        noexcept(std::is_nothrow_constructible_v<value_type, Args...>) {
        using target_type = std::conditional_t<bLayoutCompatible, typename slot_type::mutable_value_type, typename
          slot_type::value_type>;

        target_type* ptr = [&] {
          if constexpr (bLayoutCompatible)
            return &slot->mutable_value;
          else
            return &slot->value;
        }();

        emplace(slot);
        if constexpr (memory::allocators::has_construct_v<Allocator, value_type*, Args...>) {
          alloc.construct(ptr, std::forward<Args>(args)...);
        } else {
          std::construct_at(ptr, std::forward<Args>(args)...);
        }
      }

      template<typename Allocator>
      constexpr void destroy(Allocator& alloc, slot_type* slot)
        noexcept(std::is_nothrow_destructible_v<value_type>) {
        using target_type = std::conditional_t<bLayoutCompatible, typename slot_type::mutable_value_type, typename
          slot_type::value_type>;

        target_type* ptr = [&] {
          if constexpr (bLayoutCompatible)
            return &slot->mutable_value;
          else
            return &slot->value;
        }();

        if constexpr (memory::allocators::has_destroy_v<Allocator, value_type*>) {
          alloc.destroy(ptr);
        } else {
          std::destroy_at(ptr);
        }
      }

    private:
      FORCE_INLINE constexpr void emplace(slot_type* slot) noexcept {
        new(slot) slot_type;
      }
    };

    //Count should be 2^k(8/16/32/64 Byte, aligned to SIMD) & > 8
    template<typename Policy, size_t Count = 8, typename Allocator = std::allocator<typename
      Policy::slot_type::value_type>>
    class SwissTable {
    public:
      using size_type = Policy::size_type;
      using slot_type = Policy::slot_type;

      static_assert(Count != 0 && (Count & (Count - 1)) == 0);
      static_assert(Count >= alignof(ctrl_t));

      constexpr SwissTable() noexcept;
      constexpr SwissTable(const SwissTable& other) noexcept;
      constexpr SwissTable(SwissTable&& other) noexcept;
      constexpr SwissTable& operator=(const SwissTable& other) noexcept;
      constexpr SwissTable& operator=(SwissTable&& other) noexcept;
      //constexpr void steal_from(storage_& other) noexcept;
    private:
      struct alignas(Count) aligned_storage {
        ctrl_t data[Count];
      };

      aligned_storage* metadata_;
      size_type size_;

      union {
        slot_type* data_;
      };
    };

    struct quadratic;
    struct linear;

    template<typename Tag>
    class ProbeSeq;

    template<>
    class ProbeSeq<linear> {
    public:
      constexpr ProbeSeq(size_t capacity, size_t start, size_t step) noexcept :
        capacity_(capacity), current_(start), step_(step) {}

      constexpr inline void next() noexcept {
        current_ = (current_ + step_) % capacity_;
      };

      [[nodiscard]] inline constexpr size_t current() const noexcept {
        return current_;
      }
    private:
      size_t capacity_;
      size_t current_;
      const size_t step_;
    };

    template<>
    class ProbeSeq<quadratic> {
    public:
      constexpr ProbeSeq(size_t capacity, size_t start, size_t step) noexcept : mask_(capacity - 1),
        current_(start), step_(step) {
#ifdef NOYX_CORE_DEBUG
        NOYX_CORE_ASSERT_ABORT((capacity & (capacity - 1)) == 0, "capacity must be a power of two");
#endif
      };

      constexpr inline void next() noexcept {
        const size_t i = current_;
        current_ = (step_ * (i * i + i) / 2 + i) & mask_;
      };

      [[nodiscard]] inline constexpr size_t current() const noexcept {
        return current_;
      }

    private:
      size_t mask_;
      size_t current_;
      const size_t step_;
    };

    class SwissTableSize {
    public:
      constexpr SwissTableSize(const SwissTableSize& other) {  }
      constexpr SwissTableSize(SwissTableSize&& other) noexcept {  }
      constexpr ~SwissTableSize() = default;

      constexpr FORCE_INLINE void on_insert(bool into_tombstone) noexcept;
      [[nodiscard]] constexpr FORCE_INLINE bool need_grow() const noexcept {
        return size_ < ;
      };
      constexpr FORCE_INLINE void on_erase() noexcept;
      constexpr FORCE_INLINE void reset_after_rehash(/*KA*/) noexcept;
    private:
      size_t capacity_;
      size_t size_;
      size_t growth_threshold_;
    };
  }

  template<typename Key, typename T, typename Hasher = std::hash<Key>, typename Key_eq = std::equal_to<Key>, typename
    Allocator = std::allocator<std::pair<const Key, T>>>
  class TFlatHashMap {
  public:
    using allocator_type = Allocator;
    using allocator_traits = memory::allocators::allocator_traits<allocator_type>;
    using policy = details::SlotFlatHashMapPolicies<Key, T>;
    using value_type = policy::value_type;
    using pointer = value_type*;
    using size_type = std::size_t;
    using reference = T&;
    using const_reference = const T&;
    using key_type = std::remove_const_t<Key>;
    //
    using iterator = pointer;
    using const_iterator = const pointer;

  private:

  private:
    [[no_unique_address]] allocator_type allocator_;
    size_type capacity_;
  };
} // noyxcore::containers

#include "TFlatHashMap-inl.hpp"
