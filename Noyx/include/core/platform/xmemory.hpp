#pragma once

#include "platform.hpp"
#include "typedef.hpp"

#if defined(PLATFORM_WINDOWS)

#include <Windows.h>

void* PlatformVirtualAlloc(void* address, noyx::size_t size, noyx::uint32 flAllocationType, noyx::uint32 flProtect) {
  return ::VirtualAlloc(address, static_cast<SIZE_T>(size), static_cast<DWORD>(flAllocationType), static_cast<DWORD>(flProtect));
}

#elif defined(PLATFORM_LINUX)

#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>
#include <cstring> 
#include <iostream>

#ifndef MEM_COMMIT
constexpr unsigned int MEM_COMMIT = 0x00001000;
#endif
#ifndef MEM_RESERVE
constexpr unsigned int MEM_RESERVE = 0x00002000;
#endif

#ifndef PAGE_NOACCESS
constexpr unsigned int PAGE_NOACCESS = 0x01;
constexpr unsigned int PAGE_READONLY = 0x02;
constexpr unsigned int PAGE_READWRITE = 0x04;
constexpr unsigned int PAGE_WRITECOPY = 0x08;
constexpr unsigned int PAGE_EXECUTE = 0x10;
constexpr unsigned int PAGE_EXECUTE_READ = 0x20;
constexpr unsigned int PAGE_EXECUTE_READWRITE = 0x40;
#endif

static constexpr int windows_protect_to_posix(int winProtect) {
  int prot = 0;
  if (winProtect & PAGE_NOACCESS) {
    prot = PROT_NONE;
    return prot;
  }
  if (winProtect & (PAGE_READONLY | PAGE_READWRITE | PAGE_WRITECOPY | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE)) {
    prot |= PROT_READ;
  }
  if (winProtect & (PAGE_READWRITE | PAGE_WRITECOPY | PAGE_EXECUTE_READWRITE)) {
    prot |= PROT_WRITE;
  }
  if (winProtect & (PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE)) {
    prot |= PROT_EXEC;
  }
  // if nothing matched, default to PROT_READ|PROT_WRITE as a safe fallback
  if (prot == 0) prot = PROT_READ | PROT_WRITE;
  return prot;
}

void* PlatformVirtualAlloc(void* address, noyx::size_t size, noyx::uint32 flAllocationType, noyx::uint32 flProtect) {
  // page alignment
  const long page_size = sysconf(_SC_PAGESIZE);
  if (page_size <= 0) page_size = 4096;
  // align address down to page boundary if specified
  uintptr_t addr = reinterpret_cast<uintptr_t>(address);
  if (address != nullptr) {
    addr = (addr / page_size) * page_size;
  }

  // determine prot
  int prot = windows_protect_to_posix(static_cast<int>(flProtect));

  // default flags
  int flags = MAP_PRIVATE | MAP_ANONYMOUS;

  // If caller asked to reserve only -> PROT_NONE + MAP_NORESERVE (no physical commit)
  bool reserve_only = (flAllocationType & MEM_RESERVE) && !(flAllocationType & MEM_COMMIT);
  if (reserve_only) {
    prot = PROT_NONE;
#ifdef MAP_NORESERVE
    flags |= MAP_NORESERVE;
#endif
  }

  // If address provided, try to map at that address.
  void* result = mmap(
    address ? reinterpret_cast<void*>(addr) : nullptr,
    size,
    prot,
    flags | (address ? MAP_FIXED_NOREPLACE : 0),
    -1,
    0
  );

  if (result == MAP_FAILED) {
    // If MAP_FIXED_NOREPLACE unsupported/failed, try without it when address was provided.
    if (address) {
#ifdef MAP_FIXED_NOREPLACE
      // try fallback without MAP_FIXED_NOREPLACE (less safe)
      result = mmap(reinterpret_cast<void*>(addr), size, prot, flags, -1, 0);
      if (result == MAP_FAILED) {
        // fail
        //std::cerr << "mmap failed: " << strerror(errno) << "\n";
        return nullptr;
      }
#else
      // no MAP_FIXED_NOREPLACE on this system: try fallback
      result = mmap(reinterpret_cast<void*>(addr), size, prot, flags, -1, 0);
      if (result == MAP_FAILED) return nullptr;
#endif
    }
    else {
      return nullptr;
    }
  }

  // If caller asked for MEM_COMMIT (i.e. commit right now) and we reserved with PROT_NONE, make pages accessible
  if ((flAllocationType & MEM_COMMIT) && prot == PROT_NONE) {
    int real_prot = windows_protect_to_posix(static_cast<int>(flProtect));
    if (mprotect(result, size, real_prot) != 0) {
      // restore / unmap on failure
      munmap(result, size);
      return nullptr;
    }
  }

  return result;
}

#elif defined(PLATFORM_APPLE) || defined(PLATFORM_MAC)

// macOS has mmap too — simple wrapper similar to Linux
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>

// On macOS, ignore flAllocationType/flProtect or map minimal behavior
void* PlatformVirtualAlloc(void* address, noyx::size_t size, noyx::uint32 flAllocationType, noyx::uint32 flProtect) {
  int prot = PROT_READ | PROT_WRITE;
  int flags = MAP_PRIVATE | MAP_ANON;
  void* res = mmap(address, size, prot, flags, -1, 0);
  if (res == MAP_FAILED) return nullptr;
  return res;
}

#else

#error "Platform not supported"

#endif

#include <cstdint>
#include <type_traits>

#if defined(_MSC_VER)
#include <intrin.h>
#endif


/*
 * @brief Returns a sentinel value meaning "not found".
 *
 * Used as a special return value in bit-scan functions when no
 * bit of the desired type (0 or 1) exists.
 * For example, if the input is all zeros and we search for a 1.
 *
 * @return Always -1.
 */
static inline int not_found() noexcept { return -1; }

// ---------- uint32_t variants ----------

/**
 * @brief Finds the index (0–31) of the least significant set bit (1).
 *
 * @param x 32-bit unsigned integer to examine.
 * @return Index of the first 1-bit from LSB side, or -1 if @p x == 0.
 *
 * @note Uses compiler intrinsics for speed:
 * - GCC/Clang: `__builtin_ctz`
 * - MSVC: `_BitScanForward`
 * - Fallback: simple bitwise loop
 */
inline int first_one_lsb_u32(noyx::uint32 x) noexcept {
  if (x == 0) return not_found();
#if defined(__GNUC__) || defined(__clang__)
  return __builtin_ctz(x);
#elif defined(_MSC_VER)
  unsigned long idx;
  if (_BitScanForward(&idx, x)) return static_cast<int>(idx);
  return not_found();
#else
  int i = 0;
  while ((x & 1u) == 0u) { x >>= 1; ++i; }
  return i;
#endif
}

/**
 * @brief Finds the index (0–31) of the least significant zero bit.
 *
 * @param x 32-bit unsigned integer to examine.
 * @return Index of the first 0-bit from LSB side, or -1 if all bits are 1.
 */
inline int first_zero_lsb_u32(noyx::uint32 x) noexcept {
  uint32_t y = ~x;
  if (y == 0) return not_found();
#if defined(__GNUC__) || defined(__clang__)
  return __builtin_ctz(y);
#elif defined(_MSC_VER)
  unsigned long idx;
  if (_BitScanForward(&idx, y)) return static_cast<int>(idx);
  return not_found();
#else
  int i = 0;
  while ((y & 1u) == 0u) { y >>= 1; ++i; }
  return i;
#endif
}

/**
 * @brief Finds the index (0–31) of the most significant set bit (1).
 *
 * @param x 32-bit unsigned integer to examine.
 * @return Index of the highest 1-bit, or -1 if @p x == 0.
 */
inline int first_one_msb_u32(noyx::uint32 x) noexcept {
  if (x == 0) return not_found();
#if defined(__GNUC__) || defined(__clang__)
  return 31 - __builtin_clz(x);
#elif defined(_MSC_VER)
  unsigned long idx;
  if (_BitScanReverse(&idx, x)) return static_cast<int>(idx);
  return not_found();
#else
  int i = 31;
  while (((x >> i) & 1u) == 0u) --i;
  return i;
#endif
}

/**
 * @brief Finds the index (0–31) of the most significant zero bit.
 *
 * @param x 32-bit unsigned integer to examine.
 * @return Index of the highest 0-bit, or -1 if all bits are 1.
 */
inline int first_zero_msb_u32(noyx::uint32 x) noexcept {
  uint32_t y = ~x;
  if (y == 0) return not_found();
#if defined(__GNUC__) || defined(__clang__)
  return 31 - __builtin_clz(y);
#elif defined(_MSC_VER)
  unsigned long idx;
  if (_BitScanReverse(&idx, y)) return static_cast<int>(idx);
  return not_found();
#else
  int i = 31;
  while (((y >> i) & 1u) == 0u) --i;
  return i;
#endif
}

/**
 * @brief Counts the number of 1-bits in a 32-bit value.
 *
 * @param x 32-bit unsigned integer to count.
 * @return Number of bits set to 1 (0–32).
 */
inline unsigned int popcount_one_u32(noyx::uint32 x) noexcept {
#if defined(__GNUC__) || defined(__clang__)
  return __builtin_popcount(x);
#elif defined(_MSC_VER)
  return __popcnt(x);
#else
  uint32_t v = x;
  v = v - ((v >> 1) & 0x55555555);
  v = (v & 0x33333333) + ((v >> 2) & 0x33333333);
  v = (v + (v >> 4)) & 0x0F0F0F0F;
  v = v + (v >> 8);
  v = v + (v >> 16);
  return v & 0x3F;
#endif
}

/**
 * @brief Counts the number of 0-bits in a 32-bit value.
 *
 * @param x 32-bit unsigned integer to count.
 * @return Number of bits set to 0 (0–32).
 */
inline unsigned int popcount_zero_u32(noyx::uint32 x) noexcept {
  noyx::uint32 y = ~x;
#if defined(__GNUC__) || defined(__clang__)
  return __builtin_popcount(y);
#elif defined(_MSC_VER)
  return __popcnt(y);
#else
  uint32_t v = y;
  v = v - ((v >> 1) & 0x55555555);
  v = (v & 0x33333333) + ((v >> 2) & 0x33333333);
  v = (v + (v >> 4)) & 0x0F0F0F0F;
  v = v + (v >> 8);
  v = v + (v >> 16);
  return v & 0x3F;
#endif
}

#if defined(PLATFORM_CPU_X86_64_FAMILY)
// ---------- uint64_t variants ----------

/**
 * @brief Finds the index (0–63) of the least significant set bit (1) in a 64-bit value.
 *
 * @param x 64-bit unsigned integer to examine.
 * @return Index of the first 1-bit from LSB side, or -1 if @p x == 0.
 */
inline int first_one_lsb_u64(noyx::uint64 x) noexcept {
  if (x == 0) return not_found();
#if defined(__GNUC__) || defined(__clang__)
  return __builtin_ctzll(x);
#elif defined(_MSC_VER)
  unsigned long idx;
  if (_BitScanForward64(&idx, x)) return static_cast<int>(idx);
  return not_found();
#else
  int i = 0;
  while ((x & 1ull) == 0ull) { x >>= 1; ++i; }
  return i;
#endif
}

/**
 * @brief Finds the index (0–63) of the least significant zero bit in a 64-bit value.
 *
 * @param x 64-bit unsigned integer to examine.
 * @return Index of the first 0-bit from LSB side, or -1 if all bits are 1.
 */
inline int first_zero_lsb_u64(noyx::uint64 x) noexcept {
  uint64_t y = ~x;
  if (y == 0) return not_found();
#if defined(__GNUC__) || defined(__clang__)
  return __builtin_ctzll(y);
#elif defined(_MSC_VER)
  unsigned long idx;
  if (_BitScanForward64(&idx, y)) return static_cast<int>(idx);
  return not_found();
#else
  int i = 0;
  while ((y & 1ull) == 0ull) { y >>= 1; ++i; }
  return i;
#endif
}

/**
 * @brief Finds the index (0–63) of the most significant set bit (1) in a 64-bit value.
 *
 * @param x 64-bit unsigned integer to examine.
 * @return Index of the highest 1-bit, or -1 if @p x == 0.
 */
inline int first_one_msb_u64(noyx::uint64 x) noexcept {
  if (x == 0) return not_found();
#if defined(__GNUC__) || defined(__clang__)
  return 63 - __builtin_clzll(x);
#elif defined(_MSC_VER)
  unsigned long idx;
  if (_BitScanReverse64(&idx, x)) return static_cast<int>(idx);
  return not_found();
#else
  int i = 63;
  while (((x >> i) & 1ull) == 0ull) --i;
  return i;
#endif
}

/**
 * @brief Finds the index (0–63) of the most significant zero bit in a 64-bit value.
 *
 * @param x 64-bit unsigned integer to examine.
 * @return Index of the highest 0-bit, or -1 if all bits are 1.
 */
inline int first_zero_msb_u64(noyx::uint64 x) noexcept {
  uint64_t y = ~x;
  if (y == 0) return not_found();
#if defined(__GNUC__) || defined(__clang__)
  return 63 - __builtin_clzll(y);
#elif defined(_MSC_VER)
  unsigned long idx;
  if (_BitScanReverse64(&idx, y)) return static_cast<int>(idx);
  return not_found();
#else
  int i = 63;
  while (((y >> i) & 1ull) == 0ull) --i;
  return i;
#endif
}

/**
 * @brief Counts the number of 1-bits in a 64-bit value.
 *
 * @param x 64-bit unsigned integer to count.
 * @return Number of bits set to 1 (0–64).
 */
inline noyx::uint64 popcount_one_u64(noyx::uint64 x) noexcept {
#if defined(__GNUC__) || defined(__clang__)
  return __builtin_popcountll(x);
#elif defined(_MSC_VER)
  return __popcnt64(x);
#else
  uint64_t v = x;
  v = v - ((v >> 1) & 0x5555555555555555ULL);
  v = (v & 0x3333333333333333ULL) + ((v >> 2) & 0x3333333333333333ULL);
  v = (v + (v >> 4)) & 0x0F0F0F0F0F0F0F0FULL;
  v = v + (v >> 8);
  v = v + (v >> 16);
  v = v + (v >> 32);
  return v & 0x7F;
#endif
}

/**
 * @brief Counts the number of 0-bits in a 64-bit value.
 *
 * @param x 64-bit unsigned integer to count.
 * @return Number of bits set to 0 (0–64).
 */
inline noyx::uint64 popcount_zero_u64(noyx::uint64 x) noexcept {
  noyx::uint64 y = ~x;
#if defined(__GNUC__) || defined(__clang__)
  return __builtin_popcountll(y);
#elif defined(_MSC_VER)
  return __popcnt64(y);
#else
  uint64_t v = y;
  v = v - ((v >> 1) & 0x5555555555555555ULL);
  v = (v & 0x3333333333333333ULL) + ((v >> 2) & 0x3333333333333333ULL);
  v = (v + (v >> 4)) & 0x0F0F0F0F0F0F0F0FULL;
  v = v + (v >> 8);
  v = v + (v >> 16);
  v = v + (v >> 32);
  return v & 0x7F;
#endif
}
#endif // PLATFORM_CPU_X86_64_FAMILY

// TODO:SIMD instructions