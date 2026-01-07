/* SPDX-License-Identifier: Apache-2.0 */
/* 
 * \file   memory/allocators/traits.hpp
 * \brief  Traits for custom allocators and compile time helpers
 * 
 * Copyright (c) 2026 Jascit 
 * \author Jascit<https://github.com/Jascit>
 * \date   07.01.2026
 */
#pragma once
#include <memory>

namespace noyxcore::memory::allocators {
  // NOTE: alias to std::allocator_traits for now.
  // Can be replaced with custom traits when allocator API stabilizes.
  template<typename Alloc>
  using allocator_traits = std::allocator_traits<Alloc>;
} // noyxcore::memory::allocators
