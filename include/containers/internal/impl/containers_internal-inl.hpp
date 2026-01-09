/* SPDX-License-Identifier: Apache-2.0 */
/*
 * \file   containers_internal-inl.hpp
 * \brief  inline implementation
 *
 * Copyright (c) 2026 Jascit
 * \author Jascit<https://github.com/Jascit>
 * \date   09.01.2026
 */

#include "./../containers_utility.hpp"

namespace noyxcore::containers::internal {
  template<typename Alloc, typename FwdIt>
  constexpr alloc_raw_ptr_t<Alloc> uninitialized_fill_n(FwdIt first, size_t count, alloc_val_t<Alloc>& val, Alloc& alloc)
    noexcept((can_use_memset_v<FwdIt> && !std::is_constant_evaluated()) || std::is_nothrow_copy_constructible_v<alloc_val_t<Alloc>>) {
    using value_type = alloc_val_t<Alloc>;
    static_assert(std::is_same_v<alloc_raw_ptr_t<Alloc>, iter_ptr_t<FwdIt>>, "Allocator pointer type must be the same as iterator pointer type");
    static_assert(is_contiguous_iterator_v<FwdIt>, "Iterator must be contiguous like");

    if constexpr (can_use_memset_v<FwdIt> && !std::is_constant_evaluated()) {
      iter_ptr_t<FwdIt> raw_first = std::to_address(first);
      size_t bytes = count * sizeof(value_type);
      memset(raw_first, val, bytes);
      return raw_first + count;
    }
    else
    {
      if constexpr (can_use_zero_memset_v<FwdIt> && !std::is_constant_evaluated()) {
        if (is_zero_bytes(val)) {
          iter_ptr_t<FwdIt> raw_first = std::to_address(first);
          size_t bytes = count * sizeof(value_type);
          memset(raw_first, 0, bytes);
          return raw_first + count;
        }
      }
      iter_ptr_t<FwdIt> raw_first = std::to_address(first);
      iter_ptr_t<FwdIt> raw_end = raw_first + count;
      ConstructionHelper<Alloc> constructor(alloc, raw_first);
      for (iter_ptr_t<FwdIt> it = raw_first; it < raw_end; it++)
      {
        constructor.constructOne(val);
      }
      constructor.release();
      return constructor.current();
    }
  }

  template<typename Alloc, typename FwdIt>
  constexpr alloc_raw_ptr_t<Alloc> uninitialized_copy_n(FwdIt first, size_t count, alloc_raw_ptr_t<Alloc> dest, Alloc& alloc)
    noexcept((can_use_memcpy_v<FwdIt> && !std::is_constant_evaluated()) || std::is_nothrow_copy_constructible_v<alloc_val_t<Alloc>>) {
    using value_type = alloc_val_t<Alloc>;
    static_assert(std::is_same_v<alloc_raw_ptr_t<Alloc>, iter_ptr_t<FwdIt>>, "Allocator pointer type must be the same as iterator pointer type");
    static_assert(is_contiguous_iterator_v<FwdIt>, "Iterator must be contiguous like");

    if constexpr (can_use_memcpy_v<FwdIt> && !std::is_constant_evaluated())
    {
      iter_ptr_t<FwdIt> raw_first = std::to_address(first);
      auto raw_dest = std::to_address(dest);
      size_t bytes = count * sizeof(value_type);
      memcpy(raw_dest, raw_first, bytes);
      return raw_dest + count;
    }
    else 
    {
      iter_ptr_t<FwdIt> raw_first = std::to_address(first);
      iter_ptr_t<FwdIt> raw_end = raw_first + count;
      ConstructionHelper<Alloc> constructor(alloc, dest);
      for (iter_ptr_t<FwdIt> it = raw_first; it < raw_end; it++)
      {
        constructor.constructOne(*it);
      }
      constructor.release();
      return constructor.current();
    }
  }

  template<typename Alloc, typename FwdIt>
  constexpr alloc_raw_ptr_t<Alloc> uninitialized_move_n(FwdIt first, size_t count, alloc_raw_ptr_t<Alloc> dest, Alloc& alloc) 
    noexcept((can_use_memcpy_v<FwdIt> && !std::is_constant_evaluated()) || std::is_nothrow_move_constructible_v<alloc_val_t<Alloc>>) {
    using value_type = alloc_val_t<Alloc>;
    static_assert(std::is_same_v<alloc_raw_ptr_t<Alloc>, iter_ptr_t<FwdIt>>, "Allocator pointer type must be the same as iterator pointer type");
    static_assert(is_contiguous_iterator_v<FwdIt>, "Iterator must be contiguous like");

    if constexpr (can_use_memcpy_v<FwdIt> && !std::is_constant_evaluated())
    {
      iter_ptr_t<FwdIt> raw_first = std::to_address(first);
      auto raw_dest = std::to_address(dest);
      size_t bytes = count * sizeof(value_type);
      memcpy(raw_dest, raw_first, bytes);
      return raw_dest + count;
    }
    else
    {
      iter_ptr_t<FwdIt> raw_first = std::to_address(first);
      iter_ptr_t<FwdIt> raw_end = raw_first + count;
      ConstructionHelper<Alloc> constructor(alloc, dest);
      for (iter_ptr_t<FwdIt> it = raw_first; it < raw_end; it++)
      {
        constructor.constructOne(std::move(*it));
      }
      constructor.release();
      return constructor.current();
    }
  }

  template<typename Alloc, typename FwdIt>

  constexpr alloc_raw_ptr_t<Alloc> uninitialized_copy(FwdIt first, FwdIt last, alloc_raw_ptr_t<Alloc> dest, Alloc& alloc)
    noexcept((can_use_memcpy_v<FwdIt> && !std::is_constant_evaluated()) || std::is_nothrow_copy_constructible_v<alloc_val_t<Alloc>>) {
      using value_type = alloc_val_t<Alloc>;
      static_assert(std::is_same_v<alloc_raw_ptr_t<Alloc>, iter_ptr_t<FwdIt>>, "Allocator pointer type must be the same as iterator pointer type");
      static_assert(is_contiguous_iterator_v<FwdIt>, "Iterator must be contiguous like");

      if constexpr (can_use_memcpy_v<FwdIt> && !std::is_constant_evaluated())
      {
        iter_ptr_t<FwdIt> raw_first = std::to_address(first);
        iter_ptr_t<FwdIt> raw_last = std::to_address(last);
        auto raw_dest = std::to_address(dest);
        size_t count = raw_last - raw_first;
        size_t bytes = count * sizeof(value_type);
        memcpy(raw_dest, raw_first, bytes);
        return raw_dest + count;
      }
      else
      {
        iter_ptr_t<FwdIt> raw_first = std::to_address(first);
        iter_ptr_t<FwdIt> raw_end = std::to_address(last);
        ConstructionHelper<Alloc> constructor(alloc, dest);
        for (iter_ptr_t<FwdIt> it = raw_first; it < raw_end; it++)
        {
          constructor.constructOne(*it);
        }
        constructor.release();
        return constructor.current();
      }
  }

  template<typename Alloc, typename FwdIt>
  constexpr alloc_raw_ptr_t<Alloc> uninitialized_move(FwdIt first, FwdIt last, alloc_raw_ptr_t<Alloc> dest, Alloc& alloc) 
    noexcept((can_use_memcpy_v<FwdIt> && !std::is_constant_evaluated()) || std::is_nothrow_move_constructible_v<alloc_val_t<Alloc>>) {
    using value_type = alloc_val_t<Alloc>;
    static_assert(std::is_same_v<alloc_raw_ptr_t<Alloc>, iter_ptr_t<FwdIt>>, "Allocator pointer type must be the same as iterator pointer type");
    static_assert(is_contiguous_iterator_v<FwdIt>, "Iterator must be contiguous like");

    if constexpr (can_use_memcpy_v<FwdIt> && !std::is_constant_evaluated())
    {
      iter_ptr_t<FwdIt> raw_first = std::to_address(first);
      iter_ptr_t<FwdIt> raw_last = std::to_address(last);
      auto raw_dest = std::to_address(dest);
      size_t count = raw_last - raw_first;
      size_t bytes = count * sizeof(value_type);
      memcpy(raw_dest, raw_first, bytes);
      return raw_dest + count;
    }
    else
    {
      iter_ptr_t<FwdIt> raw_first = std::to_address(first);
      iter_ptr_t<FwdIt> raw_end = std::to_address(last);
      ConstructionHelper<Alloc> constructor(alloc, dest);
      for (iter_ptr_t<FwdIt> it = raw_first; it < raw_end; it++)
      {
        constructor.constructOne(std::move(*it));
      }
      constructor.release();
      return constructor.current();
    }
  }

  template<typename Alloc, typename FwdIt>
  constexpr alloc_raw_ptr_t<Alloc> uninitialized_fill(FwdIt first, FwdIt last, alloc_val_t<Alloc>& val, Alloc& alloc)
    noexcept((can_use_memset_v<FwdIt> && !std::is_constant_evaluated()) || std::is_nothrow_copy_constructible_v<alloc_val_t<Alloc>>) {
    using value_type = alloc_val_t<Alloc>;
    static_assert(std::is_same_v<alloc_raw_ptr_t<Alloc>, iter_ptr_t<FwdIt>>, "Allocator pointer type must be the same as iterator pointer type");
    static_assert(is_contiguous_iterator_v<FwdIt>, "Iterator must be contiguous like");

    if constexpr (can_use_memset_v<FwdIt> && !std::is_constant_evaluated()) {
      iter_ptr_t<FwdIt> raw_first = std::to_address(first);
      iter_ptr_t<FwdIt> raw_last = std::to_address(last);
      size_t count = raw_last - raw_first;
      size_t bytes = count * sizeof(value_type);
      memset(raw_first, val, bytes);
      return raw_first + count;
    }
    else
    {
      if constexpr (can_use_zero_memset_v<FwdIt> && !std::is_constant_evaluated()) {
        if (is_zero_bytes(val)) {
          iter_ptr_t<FwdIt> raw_first = std::to_address(first);
          iter_ptr_t<FwdIt> raw_last = std::to_address(last);
          size_t count = raw_last - raw_first;
          size_t bytes = count * sizeof(value_type);
          memset(raw_ptr, 0, bytes);
          return raw_ptr + count;
        }
      }
      iter_ptr_t<FwdIt> raw_first = std::to_address(first);
      iter_ptr_t<FwdIt> raw_last = std::to_address(last);
      ConstructionHelper<Alloc> constructor(alloc, raw_first);
      for (iter_ptr_t<FwdIt> it = raw_first; it < raw_last; it++)
      {
        constructor.constructOne(val);
      }
      constructor.release();
      return constructor.current();
    }
  }

  template<typename Alloc, typename FwdIt>
  constexpr alloc_raw_ptr_t<Alloc> uninitialized_default_construct(FwdIt first, FwdIt last, Alloc& alloc)     
    noexcept((can_use_zero_memset_v<FwdIt> && !std::is_constant_evaluated()) || std::is_nothrow_default_constructible_v<alloc_val_t<Alloc>>) {
    using value_type = alloc_val_t<Alloc>;
    static_assert(std::is_same_v<alloc_raw_ptr_t<Alloc>, iter_ptr_t<FwdIt>>, "Allocator pointer type must be the same as iterator pointer type");
    static_assert(is_contiguous_iterator_v<FwdIt>, "Iterator must be contiguous like");

    if constexpr (can_use_zero_memset_v<FwdIt> && !std::is_constant_evaluated())
    {
      iter_ptr_t<FwdIt> raw_first = std::to_address(first);
      iter_ptr_t<FwdIt> raw_last = std::to_address(last);
      auto raw_dest = std::to_address(dest);
      size_t count = raw_last - raw_first;
      size_t bytes = count * sizeof(value_type);
      memset(raw_dest, 0, bytes);
      return raw_dest + count;
    }
    else
    {
      iter_ptr_t<FwdIt> raw_first = std::to_address(first);
      iter_ptr_t<FwdIt> raw_end = std::to_address(last);
      ConstructionHelper<Alloc> constructor(alloc, raw_first);
      for (iter_ptr_t<FwdIt> it = raw_first; it < raw_end; it++)
      {
        constructor.constructOne();
      }
      constructor.release();
      return constructor.current();
    }
  }

  template<typename FwdIt>
  constexpr iter_ptr_t<FwdIt> assign_move_n(FwdIt first, size_t count, FwdIt dest)
    noexcept((can_use_memcpy_v<FwdIt> && !std::is_constant_evaluated()) || std::is_nothrow_move_assignable_v<iter_val_t<FwdIt>>) {
      using value_type = iter_val_t<FwdIt>;
      static_assert(is_contiguous_iterator_v<FwdIt>, "Iterator must be contiguous like");

      if constexpr (can_use_memcpy_v<FwdIt> && !std::is_constant_evaluated())
      {
        iter_ptr_t<FwdIt> raw_first = std::to_address(first);
        iter_ptr_t<FwdIt> raw_dest = std::to_address(dest);
        size_t bytes = count * sizeof(value_type);
        memcpy(raw_dest, raw_first, bytes);
        return raw_dest + count;
      }
      else
      {
        iter_ptr_t<FwdIt> raw_first = std::to_address(first);
        iter_ptr_t<FwdIt> raw_end = raw_first + count;
        iter_ptr_t<FwdIt> raw_dest = std::to_address(dest);
        for (iter_ptr_t<FwdIt> it = raw_first; it < raw_end; it++)
        {
          *raw_dest = std::move(*it);
          ++raw_dest;
        }
        return raw_dest;
      }
  }

  template<typename FwdIt>
  constexpr iter_ptr_t<FwdIt> assign_copy_n(FwdIt first, size_t count, FwdIt dest) 
    noexcept((can_use_memcpy_v<FwdIt> && !std::is_constant_evaluated()) || std::is_nothrow_copy_assignable_v<iter_val_t<FwdIt>>) {
    using value_type = iter_val_t<FwdIt>;
    static_assert(is_contiguous_iterator_v<FwdIt>, "Iterator must be contiguous like");

    if constexpr (can_use_memcpy_v<FwdIt> && !std::is_constant_evaluated())
    {
      iter_ptr_t<FwdIt> raw_first = std::to_address(first);
      iter_ptr_t<FwdIt> raw_dest = std::to_address(dest);
      size_t bytes = count * sizeof(value_type);
      memcpy(raw_dest, raw_first, bytes);
      return raw_dest + count;
    }
    else
    {
      iter_ptr_t<FwdIt> raw_first = std::to_address(first);
      iter_ptr_t<FwdIt> raw_end = raw_first + count;
      iter_ptr_t<FwdIt> raw_dest = std::to_address(dest);
      for (iter_ptr_t<FwdIt> it = raw_first; it < raw_end; it++)
      {
        *raw_dest = *it;
        ++raw_dest;
      }
      return raw_dest;
    }
  }

  template<typename FwdIt>
  constexpr iter_ptr_t<FwdIt> assign_move(FwdIt first, FwdIt last, FwdIt dest) 
    noexcept((can_use_memcpy_v<FwdIt> && !std::is_constant_evaluated()) || std::is_nothrow_move_assignable_v<iter_val_t<FwdIt>>) {
    using value_type = iter_val_t<FwdIt>;
    static_assert(is_contiguous_iterator_v<FwdIt>, "Iterator must be contiguous like");

    if constexpr (can_use_memcpy_v<FwdIt> && !std::is_constant_evaluated())
    {
      iter_ptr_t<FwdIt> raw_first = std::to_address(first);
      iter_ptr_t<FwdIt> raw_last = std::to_address(last);
      iter_ptr_t<FwdIt> raw_dest = std::to_address(dest);
      size_t count = raw_last - raw_first;
      size_t bytes = count * sizeof(value_type);
      memcpy(raw_dest, raw_first, bytes);
      return raw_dest + count;
    }
    else
    {
      iter_ptr_t<FwdIt> raw_first = std::to_address(first);
      iter_ptr_t<FwdIt> raw_end = std::to_address(last);
      iter_ptr_t<FwdIt> raw_dest = std::to_address(dest);
      for (iter_ptr_t<FwdIt> it = raw_first; it < raw_end; it++)
      {
        *raw_dest = std::move(*it);
        ++raw_dest;
      }
      return raw_dest;
    }

  }

  template<typename FwdIt>
  constexpr iter_ptr_t<FwdIt> assign_copy(FwdIt first, FwdIt last, FwdIt dest) 
    noexcept((can_use_memcpy_v<FwdIt> && !std::is_constant_evaluated()) || std::is_nothrow_copy_assignable_v<iter_val_t<FwdIt>>) {
    using value_type = iter_val_t<FwdIt>;
    static_assert(is_contiguous_iterator_v<FwdIt>, "Iterator must be contiguous like");

    if constexpr (can_use_memcpy_v<FwdIt> && !std::is_constant_evaluated())
    {
      iter_ptr_t<FwdIt> raw_first = std::to_address(first);
      iter_ptr_t<FwdIt> raw_last = std::to_address(last);
      iter_ptr_t<FwdIt> raw_dest = std::to_address(dest);
      size_t count = raw_last - raw_first;
      size_t bytes = count * sizeof(value_type);
      memcpy(raw_dest, raw_first, bytes);
      return raw_dest + count;
    }
    else
    {
      iter_ptr_t<FwdIt> raw_first = std::to_address(first);
      iter_ptr_t<FwdIt> raw_end = std::to_address(last);
      iter_ptr_t<FwdIt> raw_dest = std::to_address(dest);
      for (iter_ptr_t<FwdIt> it = raw_first; it < raw_end; it++)
      {
        *raw_dest = *it;
        ++raw_dest;
      }
      return raw_dest;
    }
  }
}
