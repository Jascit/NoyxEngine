/* SPDX-License-Identifier: Apache-2.0 */
/* 
 * \file   TFlatMap.hpp
 * \brief  
 * 
 * Copyright (c) 2026 Jascit 
 * \author Jascit<https://github.com/Jascit>
 * \date   09.01.2026
 */
#include <memory/allocators/traits.hpp>
#include <vector>
namespace noyxcore::containers {
  template<typename Key, typename T, typename Hasher = std::hash<Key>, typename Keyeq = std::equal_to<Key>, typename Alloc = std::allocator<std::pair<const Key, T>> /*= some_allocator<std::pair<const Key, T>>*/>
  class TFlatMap {
  public:
    using allocator_traits = memory::allocators::allocator_traits<Alloc>;
    using value_type = T;
    using pointer = T*;
  };
} // noyxcore::containers 
