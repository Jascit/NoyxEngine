/* SPDX-License-Identifier: Apache-2.0 */
/* 
 * \file   containers_traits.hpp
 * \brief  Internal type traits for containers and memory algorithms.
 * 
 * Copyright (c) 2026 Jascit 
 * \author Jascit<https://github.com/Jascit>
 * \date   08.01.2026
 */

#pragma once
#include <type_traits>
#include <memory/allocators/traits.hpp>
#include <platform/debug.hpp>

namespace noyxcore::containers::internal {
    template<typename It>
    using iter_val_t = std::iter_value_t<It>;

    template<typename It>
    using iter_ptr_t = decltype(std::to_address(std::declval<It&>()));;

    template<typename It>
    using iter_ref_t = std::iter_reference_t<It>;

    template<typename It>
    using is_contiguous_iterator = std::bool_constant<std::contiguous_iterator<It>>;
    template<typename It>
    inline constexpr bool is_contiguous_iterator_v = is_contiguous_iterator<It>::value;

    template<typename T>
    struct is_character : std::false_type {};
    template<>
    struct is_character<char> : std::true_type {};
    template<>
    struct is_character<char8_t> : std::true_type {};
    template<>
    struct is_character<uint8_t> : std::true_type {};
    template<>
    struct is_character<int8_t> : std::true_type {};
    
    template<typename T>
    struct is_boolean : std::false_type {};

    template<>
    struct is_boolean<bool> : std::true_type {};

    template <typename It>
    constexpr bool can_use_memset_v =
      std::conjunction_v<
      is_contiguous_iterator<It>,
      std::is_scalar<iter_val_t<It>>,
      std::disjunction<is_character<iter_val_t<It>>, is_boolean<iter_val_t<It>>>,
      std::negation<std::is_volatile<std::remove_reference_t<iter_ref_t<It>>>>,
      std::negation<std::is_member_pointer<iter_val_t<It>>>
      >;

    template <typename It>
    constexpr bool can_use_memcpy_v =
      std::conjunction_v<
      is_contiguous_iterator<It>,
      std::is_trivially_copyable<iter_val_t<It>>,
      std::negation<std::is_volatile<std::remove_reference_t<iter_ref_t<It>>>>
      >;

    template <typename It>
    constexpr bool can_use_memmove_v = can_use_memcpy_v<It>;

    template <typename It>
    constexpr bool can_use_zero_memset_v =
      std::conjunction_v<
      is_contiguous_iterator<It>,
      std::is_scalar<iter_val_t<It>>,
      std::negation<std::is_volatile<std::remove_reference_t<iter_ref_t<It>>>>,
      std::negation<std::is_member_pointer<iter_val_t<It>>>
      >;


    template<typename Alloc>
    using alloc_val_t = typename memory::allocators::allocator_traits<Alloc>::value_type;

    template<typename Alloc>
    using alloc_ptr_t = typename memory::allocators::allocator_traits<Alloc>::pointer;

    template<typename Alloc>
    using alloc_raw_ptr_t = decltype(std::to_address(std::declval<alloc_ptr_t<Alloc>>()));

    struct tag_move {};
    struct tag_copy {};

    template<typename T, typename = void>
    struct has_deref : std::false_type {};

    template<typename T>
    struct has_deref<T, std::void_t<decltype(*std::declval<T&>())>> : std::true_type {};

    template<typename T>
    struct is_reference_wrapper : std::false_type {};

    template<typename U>
    struct is_reference_wrapper<std::reference_wrapper<U>> : std::true_type {};

    template<typename...>
    static constexpr bool always_false = false;
}
