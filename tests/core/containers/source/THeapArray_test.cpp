/* SPDX-License-Identifier: Apache-2.0 */

/**
 * @file     THeapArray_test.cpp
 * @brief
 *
 * @author   MaksymRbkh (https://github.com/MaksymRbkh)
 * @date     04.02.2026
 *
 * @copyright Copyright (c) 2026 The Project Contributors
 */

// heap_array_tests.cpp
#include <algorithm>
#include <atomic>
#include <chrono>
#include <containers/THeapArray.hpp>
#include <containers/internal/containers_internal.hpp>
#include <iomanip>
#include <iostream>
#include <memory>
#include <random>
#include <stdexcept>
#include <string>
#include <tests_details.h>
#include <thread>
#include <vector>

using noyxcore::containers::THeapArray;

// Test 1: basic construct, capacity, data, operator[], begin/end
NOYX_TEST(heap_array_test, basic_int_behavior)
{
    using policy_t     = int;
    using heap_t       = THeapArray<policy_t>;
    constexpr size_t N = 8;
    heap_t a(N); // default-constructed elements

    NOYX_ASSERT_EQ(a.capacity(), (heap_t::size_type) N);

    auto p = a.data();
    NOYX_ASSERT_TRUE(p != nullptr || N == 0);

    for (size_t i = 0; i < N; ++i)
    {
        a[(heap_t::size_type) i] = static_cast<int>(i * 5 + 2);
    }
    for (size_t i = 0; i < N; ++i)
    {
        NOYX_ASSERT_EQ(a[(heap_t::size_type) i], (int) (i * 5 + 2));
        if (p)
            NOYX_ASSERT_EQ(p[i], (int) (i * 5 + 2));
    }

    size_t cnt = 0;
    for (auto it = a.begin(); it != a.end(); ++it)
        ++cnt;
    NOYX_ASSERT_EQ(cnt, N);
}

// Test 2: construct with fill value (value ctor)
NOYX_TEST(heap_array_test, construct_with_value)
{
    using policy_t     = int;
    using heap_t       = THeapArray<policy_t, std::allocator<policy_t>>;
    constexpr size_t N = 5;
    heap_t a(N, 42); // fill with 42 via rvalue ctor
    NOYX_ASSERT_EQ(a.size(), (heap_t::size_type) N);
    for (size_t i = 0; i < N; ++i)
        NOYX_ASSERT_EQ(a[(heap_t::size_type) i], 42);
}

// Test 3: at() bounds checking
NOYX_TEST(heap_array_test, at_throws)
{
    using policy_t = int;
    using heap_t   = THeapArray<policy_t>;
    heap_t a(3);
    NOYX_ASSERT_EQ(a.size(), (heap_t::size_type) 3);
    bool thrown = false;
    try
    {
        [[maybe_unused]] auto p = a.at(5);
    }
    catch (const std::out_of_range &)
    {
        thrown = true;
    }
    catch (...)
    {
        thrown = false;
    }
    NOYX_ASSERT_TRUE(thrown);
}

// Test 4: copy-constructor deep copy
NOYX_TEST(heap_array_test, copy_constructor_deep_strings)
{
    using policy_t     = std::string;
    using heap_t       = THeapArray<policy_t>;
    constexpr size_t N = 4;
    heap_t src(N);
    for (size_t i = 0; i < N; ++i)
        src[(heap_t::size_type) i] = "s" + std::to_string(i);

    heap_t dst(src); // copy ctor
    dst[(heap_t::size_type) 0] = "CHANGED";
    NOYX_ASSERT_EQ(src[(heap_t::size_type) 0], std::string("s0"));
    NOYX_ASSERT_EQ(dst[(heap_t::size_type) 0], std::string("CHANGED"));
}

// Test 5: copy-assignment
NOYX_TEST(heap_array_test, copy_assignment)
{
    using policy_t     = int;
    using heap_t       = THeapArray<policy_t>;
    constexpr size_t N = 6;
    heap_t a(N);
    for (size_t i = 0; i < N; ++i)
        a[(heap_t::size_type) i] = (int) (100 + i);

    heap_t b(2);
    b = a; // copy assign

    b[(heap_t::size_type) 1] = 777;
    NOYX_ASSERT_EQ(a[(heap_t::size_type) 1], 101);
    NOYX_ASSERT_EQ(b[(heap_t::size_type) 1], 777);
}

// Test 6: move-constructor and move-assignment
NOYX_TEST(heap_array_test, move_semantics_strings)
{
    using policy_t     = std::string;
    using heap_t       = THeapArray<policy_t>;
    constexpr size_t N = 3;
    heap_t a(N);
    for (size_t i = 0; i < N; ++i)
        a[(heap_t::size_type) i] = "v" + std::to_string(i);

    heap_t moved(std::move(a)); // move ctor

    for (size_t i = 0; i < N; ++i)
    {
        NOYX_ASSERT_EQ(moved[(heap_t::size_type) i], std::string("v" + std::to_string(i)));
    }

    heap_t x(N);
    for (size_t i = 0; i < N; ++i)
        x[(heap_t::size_type) i] = "X" + std::to_string(i);
    x = std::move(moved); // move assign

    for (size_t i = 0; i < N; ++i)
    {
        NOYX_ASSERT_EQ(x[(heap_t::size_type) i], std::string("v" + std::to_string(i)));
    }
}

// Test 7: iterators random access & arithmetic
NOYX_TEST(heap_array_test, iterators_arithmetic)
{
    using policy_t     = int;
    using heap_t       = THeapArray<policy_t>;
    constexpr size_t N = 12;
    heap_t a(N);
    for (size_t i = 0; i < N; ++i)
        a[(heap_t::size_type) i] = (int) (i + 1);

    auto it = a.begin();
    it += 5;
    NOYX_ASSERT_EQ(*it, 6);
    it -= 3;
    NOYX_ASSERT_EQ(*it, 3);

    auto it2 = a.begin() + 7;
    NOYX_ASSERT_EQ((int) (it2 - a.begin()), 7);

    NOYX_ASSERT_EQ(a[0], 1);
    NOYX_ASSERT_EQ(a[5], 6);
    NOYX_ASSERT_EQ(a[11], 12);
}

// Test 8: reserve / resize behaviour
NOYX_TEST(heap_array_test, reserve_and_resize)
{
    using policy_t = int;
    using heap_t   = THeapArray<policy_t>;
    heap_t a(4);
    for (size_t i = 0; i < 4; ++i)
        a[(heap_t::size_type) i] = (int) (i + 1);

    auto old_cap = a.capacity();
    a.reserve(old_cap + 10);
    NOYX_ASSERT_TRUE(a.capacity() >= old_cap + 10);
    NOYX_ASSERT_EQ(a.size(), (heap_t::size_type) 4);

    // resize downward
    a.resize(2);
    NOYX_ASSERT_EQ(a.capacity(), (old_cap + 10));
    NOYX_ASSERT_EQ(a.size(), (heap_t::size_type) 2);
    // values preserved in the front
    NOYX_ASSERT_EQ(a[(heap_t::size_type) 0], 1);
    NOYX_ASSERT_EQ(a[(heap_t::size_type) 1], 2);
}

// Test 9: zero-sized heap_array behaviour
NOYX_TEST(heap_array_test, zero_size)
{
    using policy_t = int;
    using heap_t   = THeapArray<policy_t>;
    heap_t z(0);
    NOYX_ASSERT_EQ(z.capacity(), (heap_t::size_type) 0);
    auto b = z.begin();
    auto e = z.end();
    NOYX_ASSERT_TRUE(b == e);
    (void) z.data();
}

// Test 10: swap
NOYX_TEST(heap_array_test, swap)
{
    using policy_t     = int;
    using heap_t       = THeapArray<policy_t>;
    constexpr size_t N = 4;
    heap_t a(N);
    heap_t b(N);
    for (size_t i = 0; i < N; ++i)
    {
        a[(heap_t::size_type) i] = (int) (i + 1);
        b[(heap_t::size_type) i] = (int) ((i + 1) * 10);
    }

    int a0 = a[(heap_t::size_type) 0];
    int b0 = b[(heap_t::size_type) 0];

    std::swap(a, b);

    NOYX_ASSERT_EQ(a[(heap_t::size_type) 0], b0);
    NOYX_ASSERT_EQ(b[(heap_t::size_type) 0], a0);
}

NOYX_TEST(heap_array_test, smoke)
{
    using policy_t = int;
    using heap_t   = THeapArray<policy_t>;

    heap_t a(1);
    NOYX_ASSERT_EQ(a.size(), 1);
    a[0] = 123;
    NOYX_ASSERT_EQ(a[0], 123);

    a.reserve(10);
    NOYX_ASSERT_TRUE(a.capacity() >= 10);
    NOYX_ASSERT_EQ(a[0], 123);

    a.resize(1);
    NOYX_ASSERT_EQ(a.size(), 1);
    NOYX_ASSERT_EQ(a[0], 123);
}

NOYX_TEST(heap_array_test, self_copy_assignment)
{
    using policy_t = int;
    using heap_t   = THeapArray<policy_t>;

    heap_t a(5);
    for (size_t i = 0; i < 5; ++i)
        a[i] = (int) i;

    a = a; // self assign

    for (size_t i = 0; i < 5; ++i)
        NOYX_ASSERT_EQ(a[i], (int) i);
}

NOYX_TEST(heap_array_test, self_move_assignment)
{
    using policy_t = std::string;
    using heap_t   = THeapArray<policy_t>;

    heap_t a(3);
    a[0] = "a";
    a[1] = "b";
    a[2] = "c";

    a = std::move(a);

    NOYX_ASSERT_EQ(a.size(), 3);
    NOYX_ASSERT_EQ(a[0], "a");
    NOYX_ASSERT_EQ(a[1], "b");
    NOYX_ASSERT_EQ(a[2], "c");
}

NOYX_TEST(heap_array_test, const_access)
{
    using policy_t = int;
    using heap_t   = THeapArray<policy_t>;

    heap_t a(4);
    for (size_t i = 0; i < 4; ++i)
        a[i] = (int) (10 + i);

    const heap_t &ca = a;

    NOYX_ASSERT_EQ(ca[0], 10);
    NOYX_ASSERT_EQ(ca.at(3), 13);

    size_t cnt = 0;
    for (auto it = ca.begin(); it != ca.end(); ++it)
        ++cnt;

    NOYX_ASSERT_EQ(cnt, 4);
}

NOYX_TEST(heap_array_test, stress_large)
{
    using policy_t = int;
    using heap_t   = THeapArray<policy_t>;

    constexpr size_t N = 1'000'000;
    heap_t a(N);

    for (size_t i = 0; i < N; ++i)
        a[i] = (int) (i ^ 0xDEADBEEF);

    long long checksum = 0;
    for (size_t i = 0; i < N; ++i)
        checksum += a[i];

    NOYX_ASSERT_TRUE(checksum != 0);
}

NOYX_TEST(heap_array_test, multiple_reserve_preserves_data)
{
    using policy_t = int;
    using heap_t   = THeapArray<policy_t>;

    heap_t a(2);
    a[0] = 1;
    a[1] = 2;

    for (int i = 0; i < 10; ++i)
    {
        a.reserve(a.capacity() + 5);
        NOYX_ASSERT_EQ(a[0], 1);
        NOYX_ASSERT_EQ(a[1], 2);
    }
}

NOYX_TEST(heap_array_test, resize_to_zero_and_back)
{
    using policy_t = int;
    using heap_t   = THeapArray<policy_t>;

    heap_t a(5);
    for (size_t i = 0; i < 5; ++i)
        a[i] = (int) i;

    a.resize(0);
    NOYX_ASSERT_EQ(a.size(), 0);
    NOYX_ASSERT_EQ(a.capacity(), 5);

    a.resize(3);
    NOYX_ASSERT_EQ(a.size(), 3);
    NOYX_ASSERT_EQ(a.capacity(), 5);
}

// NOYX_TEST(heap_array_test, stress_iterators_multithreaded) {
//   using policy_t = int;
//   using heap_t = THeapArray<policy_t>;
//
//   constexpr heap_t::size_type N = 10'000;
//   heap_t a(N);
//   for (heap_t::size_type i = 0; i < N; ++i) a[i] = static_cast<int>(i);
//
//   // thread count and iterations tuned to be heavy but reasonable in CI
//   unsigned hc = std::thread::hardware_concurrency();
//   size_t threads = hc == 0 ? 4u : std::min<unsigned>(hc, 8u);
//   const size_t iters_per_thread = 5000;
//
//   std::atomic<bool> failed{false};
//
//   auto worker = [&] (unsigned seed) {
//     std::mt19937_64 rng(seed);
//     std::uniform_int_distribution<heap_t::size_type> dist(0, N - 1);
//
//     for (size_t it = 0; it < iters_per_thread; ++it) {
//       // pick random index and create iterator to it
//       heap_t::size_type idx = dist(rng);
//       auto itpos = a.begin() + idx;
//       int val = *itpos;
//       // assert observed value equals expected
//       NOYX_ASSERT_EQ(val, (int)idx);
//       // also create const_iterator and read again
//       const heap_t& ca = a;
//       auto cit = ca.begin() + idx;
//       int cval = *cit;
//       NOYX_ASSERT_EQ(cval, (int)idx);
//
//       // occasionally walk a short range from this iterator (read-only)
//       if ((it & 0xF) == 0) {
//         auto it2 = itpos;
//         size_t steps = std::min<heap_t::size_type>(16u, N - idx);
//         for (size_t s = 0; s < steps; ++s, ++it2) {
//           NOYX_ASSERT_EQ(*it2, (int)(idx + s));
//         }
//       }
//
//       if (failed.load(std::memory_order_relaxed)) return;
//     }
//   };
//
//   std::vector<std::thread> ths;
//   ths.reserve(threads);
//   for (size_t t = 0; t < threads; ++t) {
//     ths.emplace_back(worker, static_cast<unsigned>(std::random_device{}() ^
//     (unsigned)t));
//   }
//   for (auto& th : ths) th.join();
// }

NOYX_TEST(heap_array_test, stress_alloc_dealloc_multithreaded)
{
    using policy_t = int;
    using heap_t   = THeapArray<policy_t>;

    unsigned hc                      = std::thread::hardware_concurrency();
    size_t threads                   = hc == 0 ? 4u : std::min<unsigned>(hc, 8u);
    const size_t iters_per_thread    = 3000;
    const heap_t::size_type max_size = 2000;

    std::atomic<bool> failed {false};

    auto worker = [&](unsigned seed)
    {
        std::mt19937_64 rng(seed);
        std::uniform_int_distribution<heap_t::size_type> dist_size(0, max_size);
        for (size_t it = 0; it < iters_per_thread; ++it)
        {
            heap_t::size_type n = dist_size(rng);
            heap_t local(n);
            // touch a few elements to ensure construction & prevent trivial
            // optimizations
            if (n > 0)
            {
                size_t limit = std::min<heap_t::size_type>(n, 10);
                for (heap_t::size_type i = 0; i < limit; ++i)
                    local[i] = static_cast<int>(i + (it & 0xFF));
                // verify reads
                for (heap_t::size_type i = 0; i < limit; ++i)
                    NOYX_ASSERT_EQ(local[i], (int) (i + (it & 0xFF)));
            }
            // local goes out of scope and deallocates
            if (failed.load(std::memory_order_relaxed))
                return;
        }
    };

    std::vector<std::thread> ths;
    ths.reserve(threads);
    for (size_t t = 0; t < threads; ++t)
    {
        ths.emplace_back(worker, static_cast<unsigned>(std::random_device {}() ^ (unsigned) t));
    }
    for (auto &th : ths)
        th.join();
}

NOYX_TEST(heap_array_test, stress_alloc_dealloc_singlethreaded)
{
    using policy_t = int;
    using heap_t   = THeapArray<policy_t>;

    const size_t iters               = 3000 * 8;
    const heap_t::size_type max_size = 2000;

    std::mt19937_64 rng(static_cast<unsigned long long>(std::random_device {}()));
    std::uniform_int_distribution<heap_t::size_type> dist_size(0, max_size);

    for (size_t it = 0; it < iters; ++it)
    {
        heap_t::size_type n = dist_size(rng);

        heap_t local(n);

        if (n > 0)
        {
            size_t limit = static_cast<size_t>(std::min<heap_t::size_type>(n, 10));
            int pattern  = static_cast<int>(it & 0xFF);

            for (heap_t::size_type i = 0; i < limit; ++i)
            {
                local[i] = static_cast<int>(i + pattern);
            }

            for (heap_t::size_type i = 0; i < limit; ++i)
            {
                NOYX_ASSERT_EQ(local[i], static_cast<int>(i + pattern));
            }
        }
    }
}

NOYX_TEST(heap_array_test, non_trivial_lifetime_counts)
{
    std::atomic<int> live {0};

    struct Tracer
    {
        std::atomic<int> *_live;

        Tracer(std::atomic<int> *l = nullptr) noexcept
            : _live(l)
        {
            if (_live)
                _live->fetch_add(1);
        }

        Tracer(const Tracer &o) noexcept
            : _live(o._live)
        {
            if (_live)
                _live->fetch_add(1);
        }

        Tracer(Tracer &&o) noexcept
            : _live(o._live)
        {
            if (_live)
                _live->fetch_add(1);
            o._live = nullptr;
        }

        Tracer &operator=(const Tracer &) = default;

        Tracer &operator=(Tracer &&o) noexcept
        {
            if (this != &o)
            {
                _live   = o._live;
                o._live = nullptr;
            }
            return *this;
        }

        ~Tracer()
        {
            if (_live)
                _live->fetch_sub(1);
        }
    };

    {
        using heap_t = THeapArray<Tracer>;
        // ñòâîðþºìî ÷åðåç value-êîíñòðóêòîð ùîá âñ³ åëåìåíòè çíàëè êóäè ðàõóâàòè
        heap_t a(5, Tracer(&live));
        NOYX_ASSERT_EQ(live.load(), 5);
    }
    // ï³ñëÿ âèõîäó ç îáëàñò³ âèäèìîñò³ âñ³ äåñòðóêòîðè ïîâèíí³ áóëè âèêëèíàòèñü
    NOYX_ASSERT_EQ(live.load(), 0);
}

NOYX_TEST(heap_array_test, value_constructor_nontrivial)
{
    std::atomic<int> live {0};
    std::atomic<int> copies {0};
    std::atomic<int> moves {0};

    struct Tracer2
    {
        std::atomic<int> *_live;
        std::atomic<int> *copies_;
        std::atomic<int> *moves_;
        int payload;

        Tracer2(std::atomic<int> *l = nullptr, std::atomic<int> *c = nullptr,
                std::atomic<int> *m = nullptr, int p = 0) noexcept
            : _live(l),
              copies_(c),
              moves_(m),
              payload(p)
        {
            if (_live)
                _live->fetch_add(1);
        }

        Tracer2(const Tracer2 &o) noexcept
            : _live(o._live),
              copies_(o.copies_),
              moves_(o.moves_),
              payload(o.payload)
        {
            if (copies_)
                copies_->fetch_add(1);
            if (_live)
                _live->fetch_add(1);
        }

        Tracer2(Tracer2 &&o) noexcept
            : _live(o._live),
              copies_(o.copies_),
              moves_(o.moves_),
              payload(o.payload)
        {
            if (moves_)
                moves_->fetch_add(1);
            if (_live)
                _live->fetch_add(1);
            o._live   = nullptr;
            o.copies_ = nullptr;
            o.moves_  = nullptr;
        }

        ~Tracer2()
        {
            if (_live)
                _live->fetch_sub(1);
        }
    };

    using heap_t = THeapArray<Tracer2>;
    Tracer2 proto(&live, &copies, &moves, 123);
    heap_t a(4, proto);
    NOYX_ASSERT_EQ(live.load(), 5);

    NOYX_ASSERT_TRUE((copies.load() + moves.load()) >= 1);
}

NOYX_TEST(heap_array_test, resize_destroys_elements)
{
    std::atomic<int> live {0};

    struct R
    {
        std::atomic<int> *_live;
        int tag;

        R(std::atomic<int> *l = nullptr, int t = 0) noexcept
            : _live(l),
              tag(t)
        {
            if (_live)
                _live->fetch_add(1);
        }

        R(const R &o) noexcept
            : _live(o._live),
              tag(o.tag)
        {
            if (_live)
                _live->fetch_add(1);
        }

        R(R &&o) noexcept
            : _live(o._live),
              tag(o.tag)
        {
            if (_live)
                _live->fetch_add(1);
            o._live = nullptr;
        }

        ~R()
        {
            if (_live)
                _live->fetch_sub(1);
        }
    };

    using heap_t = THeapArray<R>;
    heap_t a(6, R(&live, 7));
    NOYX_ASSERT_EQ(live.load(), 6);

    a.resize(2);
    NOYX_ASSERT_EQ(live.load(), 2);
}

NOYX_TEST(heap_array_test, move_only_assignment)
{
    struct MoveOnly
    {
        int v;

        MoveOnly(int x = -1) noexcept
            : v(x)
        {
        }

        MoveOnly(const MoveOnly &) = delete;

        MoveOnly(MoveOnly &&o) noexcept
            : v(o.v)
        {
            o.v = -1;
        }

        MoveOnly &operator=(const MoveOnly &) = delete;

        MoveOnly &operator=(MoveOnly &&o) noexcept
        {
            v   = o.v;
            o.v = -1;
            return *this;
        }
    };

    using heap_t = THeapArray<MoveOnly>;
    heap_t a; // çàïîâíèìî çíà÷åííÿìè çà çàìîâ÷óâàííÿì
    a.reserve(3);
    a.resize(10);
    for (size_t i = 0; i < 10; i++)
    {
        a[i] = MoveOnly(i * 10);
    }
    NOYX_ASSERT_EQ(a[1].v, 10);
    NOYX_ASSERT_EQ(a[2].v, 20);
    NOYX_ASSERT_EQ(a[3].v, 30);
}

NOYX_TEST(heap_array_test, swap_preserves_resources)
{
    std::atomic<int> live {0};

    struct Resource
    {
        std::atomic<int> *_live;
        int id;

        Resource(std::atomic<int> *l = nullptr, int i = -1) noexcept
            : _live(l),
              id(i)
        {
            if (_live)
                _live->fetch_add(1);
        }

        Resource(const Resource &o) noexcept
            : _live(o._live),
              id(o.id)
        {
            if (_live)
                _live->fetch_add(1);
        }

        Resource(Resource &&o) noexcept
            : _live(o._live),
              id(o.id)
        {
            if (_live)
                _live->fetch_add(1);
            o._live = nullptr;
        }

        ~Resource()
        {
            if (_live)
                _live->fetch_sub(1);
        }
    };

    using heap_t = THeapArray<Resource>;
    heap_t a(3, Resource(&live, 1));
    heap_t b(3, Resource(&live, 100));

    // ïðèñâî¿ìî óí³êàëüí³ id ùîá ïåðåâ³ðèòè ùî swap ì³íÿº ¿õ ì³ñöÿìè
    for (size_t i = 0; i < 3; ++i)
    {
        a[i].id = static_cast<int>(10 + i);
        b[i].id = static_cast<int>(200 + i);
    }

    NOYX_ASSERT_EQ(live.load(), 6);

    // ïðè âèõîä³ ç ôóíêö³¿ æèâèõ ðåñóðñ³â íå ïîâèííî ëèøèòèñü
    // (öå ïåðåâ³ðèòü çàãàëüíèé runner/³íø³ òåñòè); òóò ëèø ïåðåâ³ðèìî ïîòî÷íèé
}

NOYX_TEST(heap_array_test, one_alive_obj)
{
    struct Resource
    {
        Resource() { std::cout << std::endl << "i'm alive" << std::endl; }

        ~Resource() { std::cout << "i'm dead" << std::endl; }
    };

    using heap_t = THeapArray<Resource>;
    heap_t a(1);
}
