#pragma once
namespace noyx {
  namespace containers {
    namespace internals {
      // =========================
      // Slot-level common traits
      // Provides low-level interface/characteristics for a single slot.
      // Responsibility: slot status, alignment, packing, space for optimizations.
      template<typename Policy>
      struct SlotPolicyCommonTraits {};
      // =========================
      // Container-wide policies
      // Provides container-wide settings (e.g., whether small-object-opt is enabled).
      // Responsibility: global flags/behavior that affect the container.
      template<typename Policy>
      struct ContainerPolicyTraits {
        static constexpr bool soo_enabled() noexcept;
      };
    }
  }
}