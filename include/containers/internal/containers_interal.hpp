/* SPDX-License-Identifier: Apache-2.0 */
/* 
 * \file   containers_interal.hpp
 * \brief  internal helpers for containers
 * 
 * Copyright (c) 2026 Jascit 
 * \author Jascit<https://github.com/Jascit>
 * \date   07.01.2026
 */

#pragma once
#include <type_traits>
#include <memory>

namespace noyxcore {
  namespace containers {
    namespace internal {
      template<typename Alloc>
      class ConstructorHelper {
      public:
        using allocator_traits = std::allocator_traits<Alloc>;
        ConstructorHelper(Alloc& alloc, );
      };
    }
  }
}
