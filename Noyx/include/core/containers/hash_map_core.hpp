#pragma once
#include "policy_traits.hpp"
namespace noyx {
  namespace containers {
    namespace core {
      using policy::traits::HashMapPolicyTraits;
      // =========================
      // Hash table core (control array, probing, rehash, memory mgmt)
      // Does not know value internals: operates via HashMapPolicy (construct/destroy/element).
      // Responsibility: ctrl bytes, probing algorithms, alloc/dealloc, rehash, statistics.
      template<typename Policy, typename Hash, typename Eq, typename Alloc>
      struct HashMapCore {
        using policy_traits = HashMapPolicyTraits<Policy>;
        using slot_type = typename policy_traits::slot_type;
        using value_type = typename policy_traits::value_type;
        using key_type = typename policy_traits::key_type;
      };

      // =========================
      // "Raw" map — adds API semantics on top of HashMapCore
      // Responsibility: operator[], try_emplace, insert_or_assign, extract, merge, etc.
      // Uses HashMapCore for low-level operations.
      template<typename Policy, typename Hash, typename Eq, typename Alloc>
      struct RawHashMap : HashMapCore<Policy, Hash, Eq, Alloc> {
        using base = HashMapCore<Policy, Hash, Eq, Alloc>;
      };
    }
  }
}