#pragma once
#include <cstddef>
#include <utility>
#include <vector>
#include <optional>
#include "xmap.hpp"
#include <platform/typedef.hpp>
#include <memory>
//TODO: 
namespace noyx {
  namespace containers {
    namespace detail {
      template <class Policy, class Hash, class Eq, class Allocator>
      class raw_hash_map;

      template <class K, class V>
      struct FlatHashMapPolicy;
    }
    template<typename Key, typename T, typename Hasher, typename Alloc>
    class RawFlatHashMap;
    template <class K, class T,
      class Hash = std::hash<K>,
      class Eq = std::equal_to<K>,
      class Allocator = std::allocator<std::pair<const K, T>>>
    class flat_hash_map
      : public detail::raw_hash_map<
      detail::FlatHashMapPolicy<K, T>, Hash, Eq, Allocator> {
      using Base = detail::raw_hash_map<
        detail::FlatHashMapPolicy<K, T>, Hash, Eq, Allocator>;

    public:
      using key_type = K;
      using mapped_type = T;
      using value_type = std::pair<const K, T>;
      using hasher = Hash;
      using key_equal = Eq;
      using allocator_type = Allocator;
      using size_type = noyx::size_t;
      using difference_type = noyx::ptrdiff_t;

      flat_hash_map() noexcept = default;
      using Base::Base;

      using Base::begin;
      using Base::cbegin;
      using Base::cend;
      using Base::end;

      using Base::capacity;
      using Base::empty;
      using Base::max_size;
      using Base::size;

      using Base::clear;
      using Base::erase;
      using Base::insert;
      using Base::insert_or_assign;
      using Base::emplace;
      using Base::emplace_hint;
      using Base::try_emplace;

      using Base::extract;
      using Base::merge;
      using Base::swap;

      using Base::rehash;
      using Base::reserve;

      using Base::at;
      using Base::contains;
      using Base::count;
      using Base::equal_range;
      using Base::find;
      using Base::operator[];

      using Base::bucket_count;
      using Base::load_factor;
      using Base::max_load_factor;

      using Base::get_allocator;
      using Base::hash_function;
      using Base::key_eq;
    };
  }
}