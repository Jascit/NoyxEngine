/* SPDX-License-Identifier: Apache-2.0 */

/**
 * @file     PhysicalPageManager_test.cpp
 * @brief     
 *
 * @author   MaksymRbkh (https://github.com/MaksymRbkh)
 * @date     18.02.2026
 *
 * @copyright Copyright (c) 2026 The Project Contributors
 */

#include <tests_details.h>
#include <memory/Physical/Block.hpp>
#include <memory/Physical/AllocationMap.hpp>
#include <memory/Physical/CommitBitmap.hpp>
#include <memory/Physical/StartMap.hpp>
#include <memory/Physical/HandleTable.hpp>
#include <memory/Physical/VirtualMemory.hpp>
#include <iostream>
#include <thread>
#include <vector>
#include <random>
#include <atomic>

using namespace noyxcore::memory;

// Тест CommitBitmap
NOYX_TEST(physical_test, commit_bitmap_basic) {
    CommitBitmap bitmap(64);

    NOYX_ASSERT_FALSE(bitmap.isSet(0));
    NOYX_ASSERT_FALSE(bitmap.isSet(10));

    bitmap.setRange(5, 10);
    for (size_t i = 5; i < 15; ++i) {
        NOYX_ASSERT_TRUE(bitmap.isSet(i));
    }
    NOYX_ASSERT_FALSE(bitmap.isSet(4));
    NOYX_ASSERT_FALSE(bitmap.isSet(15));

    bitmap.clearRange(7, 5);
    NOYX_ASSERT_TRUE(bitmap.isSet(5));
    NOYX_ASSERT_TRUE(bitmap.isSet(6));
    NOYX_ASSERT_FALSE(bitmap.isSet(7));
    NOYX_ASSERT_FALSE(bitmap.isSet(11));
    NOYX_ASSERT_TRUE(bitmap.isSet(12));
}

// Тест AllocationMap
NOYX_TEST(physical_test, allocation_map_basic) {
    AllocationMap allocMap(100);

    NOYX_ASSERT_TRUE(allocMap.isEmpty());

    auto result1 = allocMap.allocate(10);
    NOYX_ASSERT_TRUE(result1.has_value());
    NOYX_ASSERT_EQ(*result1, 0);

    auto result2 = allocMap.allocate(5);
    NOYX_ASSERT_TRUE(result2.has_value());
    NOYX_ASSERT_EQ(*result2, 10);

    NOYX_ASSERT_FALSE(allocMap.isEmpty());
    NOYX_ASSERT_FALSE(allocMap.isFull());

    allocMap.free(0);
    allocMap.free(10);

    NOYX_ASSERT_TRUE(allocMap.isEmpty());
}

// Тест AllocationMap coalescing
NOYX_TEST(physical_test, allocation_map_coalesce) {
    AllocationMap allocMap(100);

    auto h1 = allocMap.allocate(10);
    auto h2 = allocMap.allocate(10);
    auto h3 = allocMap.allocate(10);

    NOYX_ASSERT_TRUE(h1.has_value());
    NOYX_ASSERT_TRUE(h2.has_value());
    NOYX_ASSERT_TRUE(h3.has_value());

    allocMap.free(*h2); // звільняємо середній
    allocMap.free(*h1); // має злитись
    allocMap.free(*h3); // має злитись в один великий

    NOYX_ASSERT_TRUE(allocMap.isEmpty());

    auto big = allocMap.allocate(30);
    NOYX_ASSERT_TRUE(big.has_value());
    NOYX_ASSERT_EQ(*big, 0);
}

// Тест HandleTable
NOYX_TEST(physical_test, handle_table_basic) {
    size_t pageSize = OS::pageSize();
    void* addr = OS::Reserve(64 * pageSize);
    Block block(addr, 64, pageSize);

    HandleTable table;

    uint64_t h1 = table.allocate(&block, 0);
    NOYX_ASSERT_TRUE(h1 != 0);

    auto lookUp1 = table.lookUp(h1);
    NOYX_ASSERT_TRUE(lookUp1.has_value());
    NOYX_ASSERT_EQ(lookUp1->first, &block);
    NOYX_ASSERT_EQ(lookUp1->second, 0);

    table.free(h1);

    auto lookUp2 = table.lookUp(h1);
    NOYX_ASSERT_FALSE(lookUp2.has_value()); // handle протухлий

    OS::Release(addr, 64 * pageSize);
}

// Тест HandleTable generation
NOYX_TEST(physical_test, handle_table_generation) {
    size_t pageSize = OS::pageSize();
    void* addr = OS::Reserve(64 * pageSize);
    Block block(addr, 64, pageSize);

    HandleTable table;

    uint64_t h1 = table.allocate(&block, 10);
    table.free(h1);

    uint64_t h2 = table.allocate(&block, 20);

    // h1 протухлий
    auto old = table.lookUp(h1);
    NOYX_ASSERT_FALSE(old.has_value());

    // h2 валідний
    auto fresh = table.lookUp(h2);
    NOYX_ASSERT_TRUE(fresh.has_value());
    NOYX_ASSERT_EQ(fresh->second, 20);

    table.free(h2);
    OS::Release(addr, 64 * pageSize);
}

// Тест StartMap
NOYX_TEST(physical_test, start_map_basic) {
    size_t pageSize = OS::pageSize();
    void* addr1 = OS::Reserve(64 * pageSize);
    void* addr2 = OS::Reserve(64 * pageSize);

    Block block1(addr1, 64, pageSize);
    Block block2(addr2, 64, pageSize);

    StartMap smap;
    smap.sign(addr1, &block1);
    smap.sign(addr2, &block2);

    void* ptr1 = static_cast<char*>(addr1) + 10 * pageSize;
    Block* found1 = smap.find(ptr1);
    NOYX_ASSERT_EQ(found1, &block1);

    void* ptr2 = static_cast<char*>(addr2) + 5 * pageSize;
    Block* found2 = smap.find(ptr2);
    NOYX_ASSERT_EQ(found2, &block2);

    smap.unsign(addr1);
    Block* notfound = smap.find(ptr1);
    NOYX_ASSERT_TRUE(notfound == nullptr);

    OS::Release(addr1, 64 * pageSize);
    OS::Release(addr2, 64 * pageSize);
}

// Тест Block
NOYX_TEST(physical_test, block_basic) {
    size_t pageSize = OS::pageSize();
    size_t totalPages = 64;
    void* addr = OS::Reserve(totalPages * pageSize);
    NOYX_ASSERT_TRUE(addr != nullptr);

    Block block(addr, totalPages, pageSize);

    void* ptr1 = block.allocate(4);
    NOYX_ASSERT_TRUE(ptr1 != nullptr);

    void* ptr2 = block.allocate(8);
    NOYX_ASSERT_TRUE(ptr2 != nullptr);
    NOYX_ASSERT_TRUE(ptr2 != ptr1);

    NOYX_ASSERT_FALSE(block.isEmpty());

    block.free(ptr1);
    block.free(ptr2);

    NOYX_ASSERT_TRUE(block.isEmpty());

    OS::Release(addr, totalPages * pageSize);
}

// Стрес-тест Block
NOYX_TEST(physical_test, block_stress_singlethreaded) {
    size_t pageSize = OS::pageSize();
    size_t totalPages = 256;
    void* addr = OS::Reserve(totalPages * pageSize);
    NOYX_ASSERT_TRUE(addr != nullptr);

    Block block(addr, totalPages, pageSize);

    std::mt19937_64 rng(std::random_device{}());
    std::uniform_int_distribution<size_t> dist_size(1, 16);
    std::vector<void*> ptrs;

    for (size_t i = 0; i < 500; ++i) {
        if (ptrs.empty() || (rng() % 100) < 70) {
            size_t pages = dist_size(rng);
            void* ptr = block.allocate(pages);
            if (ptr) ptrs.push_back(ptr);
        } else {
            size_t idx = rng() % ptrs.size();
            block.free(ptrs[idx]);
            ptrs.erase(ptrs.begin() + idx);
        }
    }

    for (auto ptr : ptrs) {
        block.free(ptr);
    }

    NOYX_ASSERT_TRUE(block.isEmpty());

    OS::Release(addr, totalPages * pageSize);
}

// Стрес-тест Block multi-threaded
NOYX_TEST(physical_test, block_stress_multithreaded) {
    size_t pageSize = OS::pageSize();
    size_t totalPages = 512;
    void* addr = OS::Reserve(totalPages * pageSize);
    NOYX_ASSERT_TRUE(addr != nullptr);

    Block block(addr, totalPages, pageSize);

    unsigned hc = std::thread::hardware_concurrency();
    size_t threads = hc == 0 ? 4 : std::min<unsigned>(hc, 8);
    const size_t iters_per_thread = 200;

    std::atomic<bool> failed{false};

    auto worker = [&](unsigned seed) {
        std::mt19937_64 rng(seed);
        std::uniform_int_distribution<size_t> dist_size(1, 8);
        std::vector<void*> local_ptrs;

        for (size_t it = 0; it < iters_per_thread; ++it) {
            if (local_ptrs.empty() || (rng() % 100) < 70) {
                size_t pages = dist_size(rng);
                void* ptr = block.allocate(pages);
                if (ptr) local_ptrs.push_back(ptr);
            } else {
                size_t idx = rng() % local_ptrs.size();
                block.free(local_ptrs[idx]);
                local_ptrs.erase(local_ptrs.begin() + idx);
            }

            if (failed.load(std::memory_order_relaxed)) return;
        }

        for (auto ptr : local_ptrs) {
            block.free(ptr);
        }
    };

    std::vector<std::thread> ths;
    ths.reserve(threads);
    for (size_t t = 0; t < threads; ++t) {
        ths.emplace_back(worker, static_cast<unsigned>(std::random_device{}() ^ t));
    }
    for (auto& th : ths) th.join();

    OS::Release(addr, totalPages * pageSize);
}