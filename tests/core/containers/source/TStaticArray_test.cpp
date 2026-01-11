/* SPDX-License-Identifier: Apache-2.0 */
/* 
 * \file   TStaticArray_test.cpp
 * 
 * Copyright (c) 2026 Project Contributors
 * \author Jascit<https://github.com/Jascit>
 * \date   10.01.2026
 */

#include <tests_details.h>
#include <containers/TStaticArray.hpp>
#include <memory>
#include <iostream>

using noyxcore::containers::TStaticArray;

/* базовий int */
NOYX_TEST(tstaticarray_basic_int, unit_test) {
  TStaticArray<int, 8> a;
  NOYX_ASSERT_TRUE(a.empty());
  NOYX_ASSERT_EQ(a.size(), (TStaticArray<int, 8>::size_type)0);

  a.pushBack(10);
  a.pushBack(20);

  NOYX_ASSERT_EQ(a.size(), (TStaticArray<int, 8>::size_type)2);

  auto p = a.data();
  NOYX_ASSERT_TRUE(p != nullptr);

  NOYX_ASSERT_EQ(a[(TStaticArray<int, 8>::size_type)0], 10);
  NOYX_ASSERT_EQ(a[(TStaticArray<int, 8>::size_type)1], 20);
  if (p) {
    NOYX_ASSERT_EQ(p[0], 10);
    NOYX_ASSERT_EQ(p[1], 20);
  }
}

/* move-only тип */
NOYX_TEST(tstaticarray_move_only, unit_test) {
  using UP = std::unique_ptr<int>;
  TStaticArray<UP, 3> arr;
  arr.pushBack(std::make_unique<int>(5));
  arr.pushBack(std::make_unique<int>(7));

  NOYX_ASSERT_EQ(arr.size(), (TStaticArray<UP, 3>::size_type)2);
  NOYX_ASSERT_TRUE(arr[(TStaticArray<UP, 3>::size_type)0] && *arr[(TStaticArray<UP, 3>::size_type)0] == 5);
  NOYX_ASSERT_TRUE(arr[(TStaticArray<UP, 3>::size_type)1] && *arr[(TStaticArray<UP, 3>::size_type)1] == 7);
}
struct Count {
  inline static int constructed = 0;
  inline static int destroyed = 0;
  Count() { ++constructed; }
  Count(const Count&) { ++constructed; }
  Count(Count&&) noexcept { ++constructed; }
  ~Count() { ++destroyed; }
  int v = 0;
};

/* RAII: конструкції/деструкції */
NOYX_TEST(tstaticarray_raii, unit_test) {
  int& C = Count::constructed;
  int& D = Count::destroyed;
  C = 0; D = 0;

  {
    TStaticArray<Count, 5> arr;
    arr.emplaceBack();
    arr.emplaceBack();
    NOYX_ASSERT_EQ(C, 2);
    NOYX_ASSERT_EQ(arr.size(), (TStaticArray<Count, 5>::size_type)2);
  } // тут має викликатися деструктор елементів

  NOYX_ASSERT_EQ(D, 2);
}

/* data() / operator[] узгодженість */
NOYX_TEST(tstaticarray_data_vs_index, unit_test) {
  TStaticArray<int, 4> a;
  for (int i = 0; i < 4; ++i) a.pushBack(i * 11);
  NOYX_ASSERT_EQ(a.size(), (TStaticArray<int, 4>::size_type)4);

  int* d = a.data();
  NOYX_ASSERT_TRUE(d != nullptr);
  for (size_t i = 0; i < a.size(); ++i) {
    NOYX_ASSERT_EQ(a[(TStaticArray<int, 4>::size_type)i], d[i]);
  }
}

NOYX_TEST(tstaticarray_heavy_compute, perf_test) {
  constexpr size_t Iterations = 5'000'000;

  TStaticArray<int, 64> arr;
  for (int i = 0; i < 64; ++i) {
    arr.pushBack(i + 1);
  }

  volatile long long sink = 0;

  for (size_t it = 0; it < Iterations; ++it) {
    for (size_t i = 0; i < arr.size(); ++i) {
      sink += arr[i] * (int)(i + 1);
    }
  }

  NOYX_ASSERT_TRUE(sink != 0);
}
