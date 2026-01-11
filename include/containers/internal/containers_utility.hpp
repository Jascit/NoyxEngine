/* SPDX-License-Identifier: Apache-2.0 */
/* 
 * \file   containers_utility.hpp
 * \brief  Internal utilities for containers and memory algorithms.
 * 
 * Copyright (c) 2026 Project Contributors
 * \author Jascit<https://github.com/Jascit>
 * \date   08.01.2026
 */

#include "containers_traits.hpp"

namespace noyxcore::containers::internal {
  template<typename Ptr>
  constexpr Ptr* unfancy(Ptr* p) noexcept {
    return p;
  }

  template<typename Ptr>
  constexpr auto unfancy(Ptr p) noexcept {
    return std::addressof(*p);
  }

  template<typename Wrapper>
  constexpr auto unwrap(Wrapper&& wrap) noexcept -> decltype(auto) {
    if constexpr (is_reference_wrapper<Wrapper>::value)
    {
      return std::addressof(wrap.get());
    }
    else if constexpr (std::is_pointer<Wrapper>::value) {
      return wrap;
    }
    else if constexpr (has_deref<Wrapper>::value) {
      return std::addressof(*wrap);
    }
    else {
      static_assert(always_false<Wrapper>, "unwrap: unsupported wrapper type");
    }
  }

  template<typename T, typename...>
  constexpr auto first_arg(T&& arg, ...) noexcept -> decltype(auto) {
    return std::forward<T>(arg);
  }

  template<std::size_t N, typename... Args>
  constexpr decltype(auto) nth_arg(Args&&... args) {
    return std::get<N>(std::forward_as_tuple(std::forward<Args>(args)...));
  }
  
  template<typename T>
  constexpr bool is_zero_bytes(const T& val) noexcept {
    static constexpr T zero{};
    return std::memcmp(&zero, &val, sizeof(T)) == 0;
  }
}
