#pragma once

#include <../../memory/allocators/AllocatorTraits.hpp>
#include <../../platform/Typedef.hpp>
#include <../../utility/utility.hpp>
#include <type_traits>
#include "XMemory.hpp"
#include <vector>

namespace noyx {
  namespace containers {
    namespace detail {
      template<typename Types>
      class _VecVal {
      public:
        using value_type = typename Types::value_type;
        using pointer = typename Types::pointer;
        using const_pointer = typename Types::const_pointer;
        using size_type = typename Types::size_type;
        using difference_type = typename Types::difference_type;

        constexpr _VecVal() noexcept : first_(), last_(), end_() {};
        constexpr _VecVal(pointer first, pointer last, pointer end) noexcept : first_(first), last_(last), end_(end) {};
        constexpr _VecVal(_VecVal&& other) {
          noyx::utility::swap(first_, other.first_);
          noyx::utility::swap(last_, other.last_);
          noyx::utility::swap(end_, other.end_);
        };

        constexpr void SwapContent(_VecVal&& other) {
          noyx::utility::swap(first_, other.first_);
          noyx::utility::swap(last_, other.last_);
          noyx::utility::swap(end_, other.end_);
        };
        
        constexpr void TakeContent(_VecVal&& other) {
          first_ = other.first_;
          last_ = other.last_;
          end_ = other.end_;

          other.first_ = nullptr;
          other.last_ = nullptr;
          other.end_ = nullptr;
        };

      private:
        pointer first_; // pointer to beginning of array
        pointer last_; // pointer to current end of sequence
        pointer end_; // pointer to end of array
      };
    }

    template<typename T, typename Allocator>
    class TArray {
      using rebinded_alloc = noyx::memory::_rebind_alloc_t<Allocator, T>;
      using traits = noyx::memory::TAllocatorTraits<rebinded_alloc>;

    public:
      using value_type = T;
      using pointer = typename traits::pointer;
      using const_pointer = typename traits::const_pointer;
      using size_type = typename traits::size_type;
      using difference_type = typename traits::difference_type;
      using allocator_type = Allocator;

      static_assert(std::is_same_v<T, typename Allocator::value_type>,
        "T must be the same type as Allocator::value_type");

      static_assert(std::is_object_v<T>,
        "T must be an object type (not void, function, reference, or incomplete type)");

    private:
      TCompressedPair<Allocator, _VecVal<traits>> pair_;
    };
  }
}

using noyx::containers::TArray;
