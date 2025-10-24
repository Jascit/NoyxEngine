#pragma once
#include "container_internals.hpp"

namespace noyx {
  namespace containers {
    namespace policy {
      namespace traits {
        using containers::internals::SlotPolicyCommonTraits;
        using containers::internals::ContainerPolicyTraits;

        // =========================
        // Slot traits for hashmap
        // Slot-specific operations for the hash map (get key from slot, etc.).
        // Inherits common slot options from SlotPolicyCommon.
        template<typename Policy>
        struct SlotHashMapPolicyTraits : SlotPolicyCommonTraits<Policy> {};

        // =========================
        // Policy traits for hashmap pairs
        // Wrapper through which the core table works with elements/slots.
        // Responsibility: typedefs (key_type, value_type, slot_type) and operations
        // construct/destroy/element/key_from_value used by the core.
        template<typename Policy>
        struct HashMapPolicyTraits : ContainerPolicyTraits<Policy> {
          using key_type = typename Policy::key_type;
          using value_type = typename Policy::value_type;
          using slot_type = typename Policy::slot_type;
          using init_type = typename Policy::init_type;

          static value_type& element(slot_type* s);
          static const value_type& element(const slot_type* s);

          template<class Alloc, class... Args>
          static void construct(Alloc* alloc, slot_type* s, Args&&...);

          template<class Alloc>
          static void destroy(Alloc* alloc, slot_type* s) noexcept;

          static const key_type& key_from_value(const value_type& v) noexcept;
        };
      }
    }
  }
}