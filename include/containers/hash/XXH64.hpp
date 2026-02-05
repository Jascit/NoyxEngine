/* SPDX-License-Identifier: Apache-2.0 */
/*
 * \file   xxHash.hpp
 * \brief  
 *
 * Copyright (c) 2026 Project Contributors
 * \author Jascit <https://github.com/Jascit>
 * \date   03.02.2026
 * \note   
 */
#pragma once
#include <type_traits>
#include <bit>

#include "platform/debug.h"

namespace noyxcore::containers {
  namespace details {
    static inline constexpr uint64_t PRIME64_1 = 11400714785074694791ULL;
    static inline constexpr uint64_t PRIME64_2 = 14029467366897019727ULL;
    static inline constexpr uint64_t PRIME64_3 = 1609587929392839161ULL;
    static inline constexpr uint64_t PRIME64_4 = 9650029242287828579ULL;
    static inline constexpr uint64_t PRIME64_5 = 2870177450012600261ULL;
    using hash64_t = std::uint64_t;
    using u64 = std::uint64_t;
    using u32 = std::uint32_t;
    using uptr = std::uintptr_t;

    // small portability helpers
#if defined(__GNUC__) || defined(__clang__)
#define DETAIL_BSWAP64(x) __builtin_bswap64(x)
#define DETAIL_BSWAP32(x) __builtin_bswap32(x)
#else
    inline std::uint64_t bswap64_u64(std::uint64_t x) {
      return ((x & 0xFFull) << 56) |
             ((x & 0xFF00ull) << 40) |
             ((x & 0xFF0000ull) << 24) |
             ((x & 0xFF000000ull) << 8) |
             ((x >> 8) & 0xFF000000ull) |
             ((x >> 24) & 0xFF0000ull) |
             ((x >> 40) & 0xFF00ull) |
             ((x >> 56) & 0xFFull);
    }

#define DETAIL_BSWAP64(x) bswap64_u64(x)

    inline std::uint32_t bswap32_u32(std::uint32_t x) {
      return ((x & 0xFFu) << 24) |
             ((x & 0xFF00u) << 8) |
             ((x >> 8) & 0xFF00u) |
             ((x >> 24) & 0xFFu);
    }

#define DETAIL_BSWAP32(x) bswap32_u32(x)
#endif

    FORCE_INLINE u32 read_word32_le(const void* ptr) noexcept {
      u32 v;
      memcpy(&v, ptr, sizeof(u32));
      if constexpr (std::endian::native == std::endian::big) {
        v = DETAIL_BSWAP32(v);
      }
      return v;
    };

    FORCE_INLINE u64 read_word64_le(const void* ptr) noexcept {
      u64 v;
      memcpy(&v, ptr, sizeof(u32));
      if constexpr (std::endian::native == std::endian::big) {
        v = DETAIL_BSWAP64(v);
      }
      return v;
    };

    FORCE_INLINE u64 canonicalize_double(const double d) noexcept {
      u64 bits;
      std::memcpy(&bits, &d, sizeof(bits));
      // +0.0 and -0.0 -> +0.0
      if ((bits & 0x7fffffffffffffffULL) == 0) {
        bits = 0;
      }
      // detect NaN: exponent all 1s and mantissa != 0
      if ((bits & 0x7ff0000000000000ULL) == 0x7ff0000000000000ULL &&
          (bits & 0x000fffffffffffffULL) != 0) {
        // canonical quiet NaN (implementation choice)
        bits = 0x7ff8000000000000ULL;
      }
      // ensure little-endian representation when hashing bytes
      if constexpr (std::endian::native == std::endian::big) {
        bits = DETAIL_BSWAP64(bits);
      }
      return bits;
    }

    FORCE_INLINE u32 canonicalize_float(const float f) noexcept {
      u32 bits;
      std::memcpy(&bits, &f, sizeof(bits));
      // +0/-0 -> +0
      if ((bits & 0x7fffffffu) == 0) {
        bits = 0u;
      }
      // NaN detection: exponent all 1s and mantissa != 0
      if ((bits & 0x7f800000u) == 0x7f800000u && (bits & 0x007fffffu) != 0) {
        bits = 0x7fc00000u; // canonical quiet NaN (32-bit)
      }
      if constexpr (std::endian::native == std::endian::big) {
        bits = DETAIL_BSWAP32(bits);
      }
      return bits;
    }

    // ---------- xxHash helpers (small, core snippets) ----------
    // For clarity: implement minimal round/merge helpers that mirror xxh64 arithmetic
    FORCE_INLINE u64 round64(u64 acc, u64 input) noexcept {
      acc += input * PRIME64_2;
      acc = std::rotl(acc, 31);
      acc *= PRIME64_1;
      return acc;
    };
    FORCE_INLINE u64 mergeRound(u64 acc, u64 val) noexcept {
      val = round64(0, val);
      acc ^= val;
      acc = acc * PRIME64_1 + PRIME64_4;
      return acc;
    };

    // minimal wrappers for hashing primitive-sized values (one-shot small helpers).
    // These compute something equivalent to the corresponding tail processing in xxh64.
    // seed parameter left so you can pass non-zero if needed.
    [[nodiscard]] FORCE_INLINE hash64_t XXH64_32bit(uint32_t val, uint64_t seed = 0) noexcept {
      u64 h = seed + PRIME64_5 + sizeof(u32);
      u64 k1 = static_cast<u64>(val) * PRIME64_1;
      k1 = std::rotl(k1, 23) * PRIME64_2 + PRIME64_3;
      h ^= k1;
      // avalanche
      h ^= h >> 33;
      h *= PRIME64_2;
      h ^= h >> 29;
      h *= PRIME64_3;
      h ^= h >> 32;
      return h;
    };

    [[nodiscard]] FORCE_INLINE hash64_t XXH64_64bit(uint64_t val, uint64_t seed = 0) noexcept {
      u64 h = seed + PRIME64_5 + sizeof(u64);
      const u64 k1 = round64(0, val);
      h ^= k1;
      h = std::rotl(h, 27) * PRIME64_1 + PRIME64_4;
      // avalanche
      h ^= h >> 33;
      h *= PRIME64_2;
      h ^= h >> 29;
      h *= PRIME64_3;
      h ^= h >> 32;
      return h;
    };

    [[nodiscard]] inline hash64_t XXH64(const void* bytes_array, uint64_t length, uint64_t seed = 0) noexcept {
      const uint8_t* ptr = static_cast<const uint8_t*>(bytes_array);
      const uint8_t* bytes_end = ptr + length;

      hash64_t h;
      if (length >= 32) {
        const uint8_t* limit = bytes_end - 32;
        u64 v1 = seed + PRIME64_1 + PRIME64_2;
        u64 v2 = seed + PRIME64_1;
        u64 v3 = 0;
        u64 v4 = seed - PRIME64_1;

        do {
          u64 w1 = read_word64_le(ptr);
          ptr += 8;
          u64 w2 = read_word64_le(ptr);
          ptr += 8;
          u64 w3 = read_word64_le(ptr);
          ptr += 8;
          u64 w4 = read_word64_le(ptr);
          ptr += 8;

          v1 = round64(v1, w1);
          v2 = round64(v2, w2);
          v3 = round64(v3, w3);
          v4 = round64(v4, w4);
        } while (ptr <= limit);

        h = std::rotl(v1, 1) + std::rotl(v2, 7) + std::rotl(v3, 12) + std::rotl(v4, 18);
        h = mergeRound(h, v1);
        h = mergeRound(h, v2);
        h = mergeRound(h, v3);
        h = mergeRound(h, v4);
      } else {
        h = seed + PRIME64_5;
      }

      h += length;

      // tail: 8-byte chunks
      while (ptr + 8 <= bytes_end) {
        u64 k1 = read_word64_le(ptr);
        k1 = round64(0, k1);
        h ^= k1;
        h = std::rotl(h, 27) * PRIME64_1 + PRIME64_4;
        ptr += 8;
      }

      while (ptr + 4 <= bytes_end) {
        const u64 k1 = static_cast<u64>(read_word64_le(ptr));
        h = k1 * PRIME64_1;
        h = std::rotl(h, 23) * PRIME64_2 + PRIME64_3;
        ptr += 4;
      }

      while (ptr < bytes_end) {
        h ^= static_cast<u64>(*ptr) * PRIME64_5;
        h = std::rotl(h, 11) * PRIME64_1;
        ++ptr;
      }

      // avalanche
      h ^= h >> 33;
      h *= PRIME64_2;
      h ^= h >> 29;
      h *= PRIME64_3;
      h ^= h >> 32;
      return h;
    };
  }

  // ---------------- Public policy wrapper ----------------
  // Hashable concept: no const/volatile, integral/fp/enum
  template<typename T>
  concept Hashable = !std::is_const_v<T> && !std::is_volatile_v<T> && (
                       std::is_integral_v<T> || std::is_floating_point_v<T> || std::is_enum_v<T>);
  // helper concept for sizes
  template<typename T>
  concept Is32Bit = sizeof(T) == sizeof(std::uint32_t);
  template<typename T>
  concept Is64Bit = sizeof(T) == sizeof(std::uint64_t);

  namespace details {
    template<typename T>
    FORCE_INLINE hash64_t to_hash_bits(const T& val) noexcept {
      if constexpr (std::is_enum_v<T>) {
        using U = std::underlying_type_t<T>;
        if constexpr (Is64Bit<U>) {
          const u64 bits = static_cast<u64>(val);
          return XXH64_64bit(bits, 0);
        } else if constexpr (Is32Bit<U>) {
          const u32 bits = static_cast<u32>(val);
          return XXH64_32bit(bits, 0);
        } else {
          constexpr u64 bits = 0;
          memcpy(&bits, &val, sizeof(U));
          return XXH64_64bit(bits, 0);
        }
      }
      if constexpr (std::is_integral_v<T>) {
        if constexpr (sizeof(T) <= 4) {
          const u32 bits = static_cast<u32>(val);
          return XXH64_32bit(bits, 0);
        } else {
          const u64 bits = static_cast<u64>(val);
          return XXH64_64bit(bits, 0);
        }
      }
      if constexpr (std::is_floating_point_v<T>) {
        if constexpr (sizeof(T) == 4) {
          const u32 bits = canonicalize_double(static_cast<float>(val));
          return XXH64_32bit(bits, 0);
        } else {
          const u64 bits = canonicalize_double(static_cast<double>(val));
          return XXH64_64bit(bits, 0);
        }
      }else {
        //not ideal
        return XXH64(std::addressof(val), sizeof(T), 0);
      }
    };
  }

  template<Hashable T>
  struct XXH64 {
    static constexpr size_t operator()(T const& val, size_t length) {
      return details::to_hash_bits(val);
    };
  };

  template<typename T>
  struct XXH64<T*> {};
}
