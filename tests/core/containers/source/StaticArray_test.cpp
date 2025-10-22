// tests.cpp
// Simple unit-test harness for noyx::containers::TStaticArray
// Drop into your project that contains StaticArray.h
#include <tests_details.hpp>
#include <containers/StaticArray.hpp>
#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <type_traits>

using noyx::containers::TStaticArray;
using noyx::utility::swap;

// Test 1: construct, operator[], at, front/back, data, size
void TStaticArray_test_basic_int_behavior() {
  const size_t N = 8;
  TStaticArray<int> a((TStaticArray<int>::size_type)N);
  // fill with 0..N-1
  for (size_t i = 0; i < N; ++i) a[(TStaticArray<int>::size_type)i] = (int)i;

  // check operator[] and at()
  for (size_t i = 0; i < N; ++i) {
    NOYX_ASSERT_EQ(a[(TStaticArray<int>::size_type)i], (int)i);
    NOYX_ASSERT_EQ(a.at((TStaticArray<int>::size_type)i), (int)i);
  }

  // front() should be 0
  NOYX_ASSERT_EQ(a.front(), 0);
  // back() — EXPECTATION: last element (N-1)
  // **If your implementation's back() is wrong (off-by-one), this will fail.**
  if (a.back() != (int)(N - 1)) {
    NOYX_FAIL_MESSAGE("BACK() MISMATCH: back() returned " << a.back() << " expected " << (N - 1));
  }

  // data() pointer check
  auto p = a.data();
  NOYX_ASSERT_TRUE(p != nullptr);
  for (size_t i = 0; i < N; ++i) {
    NOYX_ASSERT_EQ(p[i], (int)i);
  }
}

// Test 2: fill
void TStaticArray_test_fill() {
  const size_t N = 16;
  TStaticArray<int> a((TStaticArray<int>::size_type)N);
  a.fill(123);
  for (size_t i = 0; i < N; ++i) NOYX_ASSERT_EQ(a[(TStaticArray<int>::size_type)i], 123);
}

// Test 3: copy constructor produces deep copy (modifying copy does not change original)
void TStaticArray_test_copy_constructor_deep() {
  const size_t N = 6;
  TStaticArray<int> a((TStaticArray<int>::size_type)N);
  for (size_t i = 0; i < N; ++i) a[(TStaticArray<int>::size_type)i] = (int)(i + 1);

  TStaticArray<int> b(a); // copy ctor
  // modify b
  b[(TStaticArray<int>::size_type)0] = 9999;

  // original must remain unchanged
  NOYX_ASSERT_EQ(a[(TStaticArray<int>::size_type)0], 1);
  NOYX_ASSERT_EQ(b[(TStaticArray<int>::size_type)0], 9999);
}

// Test 4: copy assignment
void TStaticArray_test_copy_assignment() {
  const size_t N = 5;
  TStaticArray<int> a((TStaticArray<int>::size_type)N);
  for (size_t i = 0; i < N; ++i) a[(TStaticArray<int>::size_type)i] = (int)i + 10;
  TStaticArray<int> b((TStaticArray<int>::size_type)N);
  b = a;
  // modify b
  b[(TStaticArray<int>::size_type)1] = 77;
  NOYX_ASSERT_EQ(a[(TStaticArray<int>::size_type)1], 11);
}

// Test 5: move constructor and move assignment semantics (source.size() -> 0)
void TStaticArray_test_move_semantics() {
  const size_t N = 7;
  TStaticArray<int> a((TStaticArray<int>::size_type)N);
  for (size_t i = 0; i < N; ++i) a[(TStaticArray<int>::size_type)i] = (int)i * 2;

  TStaticArray<int> moved(std::move(a)); // move ctor
  // source should be zeroed size
  NOYX_ASSERT_EQ(a.size(), 0);

  // assume moved holds original content
  for (size_t i = 0; i < N; ++i) NOYX_ASSERT_EQ(moved[(TStaticArray<int>::size_type)i], (int)i * 2);

  // move assignment
  TStaticArray<int> x((TStaticArray<int>::size_type)3);
  for (size_t i = 0; i < 3; ++i) x[(TStaticArray<int>::size_type)i] = (int)(100 + i);
  x = std::move(moved);
  NOYX_ASSERT_EQ(moved.size(), 0);
  NOYX_ASSERT_EQ(x.size(), N);
  NOYX_ASSERT_EQ(x[(TStaticArray<int>::size_type)2], 4);
}

// Test 6: iterators and range-based for
void TStaticArray_test_iterators() {
  const size_t N = 10;
  TStaticArray<int> a((TStaticArray<int>::size_type)N);
  for (size_t i = 0; i < N; ++i) a[(TStaticArray<int>::size_type)i] = int(i + 1);

  // range-for requires begin/end to work
  size_t idx = 0;
  for (auto& v : a) { // this requires iterator to be compatible with range-for
    (void)v; // some compilers might need non-const begin() overloads
    ++idx;
  }
  // if range-for iterated N times -> ok
  NOYX_ASSERT_EQ((int)idx, (int)N);

  // explicit iterator arithmetic test (random access)
  auto it = a.begin();
  it += 5;
  NOYX_ASSERT_EQ(*it, 6);
  it -= 3;
  NOYX_ASSERT_EQ(*it, 3);
}

// Test 7: swap (member and utility swap)
void TStaticArray_test_swap() {
  const size_t N = 4;
  TStaticArray<int> a((TStaticArray<int>::size_type)N);
  TStaticArray<int> b((TStaticArray<int>::size_type)N);
  for (size_t i = 0; i < N; ++i) { a[(TStaticArray<int>::size_type)i] = int(i + 1); b[(TStaticArray<int>::size_type)i] = int((i + 1) * 10); }

  // keep original pointers to check that swap actually swapped underlying buffers
  auto pa = a.data();
  auto pb = b.data();

  // use member swap
  a.swap(b);
  NOYX_ASSERT_TRUE(a.data() == pb && b.data() == pa);
  // values swapped
  NOYX_ASSERT_EQ(a[(TStaticArray<int>::size_type)0], 10);
  // utility swap (ADL)
  noyx::utility::swap(a, b);
  NOYX_ASSERT_EQ(a[(TStaticArray<int>::size_type)0], 1);
}

// Test 8: type that is move-only (unique_ptr) — ensure storage and move of elements work
void TStaticArray_test_move_only_type() {
  using U = std::unique_ptr<int>;
  const size_t N = 3;
  TStaticArray<U> arr((TStaticArray<U>::size_type)N);
  // assign via move into operator[]
  arr[(TStaticArray<U>::size_type)0] = std::make_unique<int>(10);
  arr[(TStaticArray<U>::size_type)1] = std::make_unique<int>(20);
  arr[(TStaticArray<U>::size_type)2] = std::make_unique<int>(30);

  NOYX_ASSERT_TRUE(arr[(TStaticArray<U>::size_type)0] && *arr[(TStaticArray<U>::size_type)0] == 10);

  // move the whole array
  TStaticArray<U> moved(std::move(arr));
  NOYX_ASSERT_EQ(arr.size(), 0);
  NOYX_ASSERT_TRUE(moved[(TStaticArray<U>::size_type)2] && *moved[(TStaticArray<U>::size_type)2] == 30);

  // note: arr.fill(...) would not compile for unique_ptr (copy required) — this is expected
}

// Test 9: zero-sized array behavior
void TStaticArray_test_zero_size() {
  TStaticArray<int> z((TStaticArray<int>::size_type)0);
  NOYX_ASSERT_EQ(z.size(), 0);
  // data() may be nullptr or not depending on allocator; we only ensure no crash on begin/end
  auto b = z.begin();
  auto e = z.end();
  NOYX_ASSERT_TRUE(b == e);
}

// Test 10: destructor behavior / RAII smoke test (run many allocations)
void TStaticArray_test_stress_alloc_dealloc() {
  for (int iter = 0; iter < 20000; ++iter) {
    TStaticArray<int> a((TStaticArray<int>::size_type)(iter % 100));
    // write something if non-empty
    if (a.size() > 0) a[(TStaticArray<int>::size_type)0] = iter;
  }
  // if we reach here without leak/crash, good enough for basic test
}

// Entry point
NOYX_TEST(TStaticArray_test, unit_test) {
    TStaticArray_test_basic_int_behavior();
    TStaticArray_test_fill();
    TStaticArray_test_copy_constructor_deep();
    TStaticArray_test_copy_assignment();
    TStaticArray_test_move_semantics();
    TStaticArray_test_iterators();
    TStaticArray_test_swap();
    TStaticArray_test_move_only_type();
    TStaticArray_test_zero_size();
    TStaticArray_test_stress_alloc_dealloc();
  return;
}
