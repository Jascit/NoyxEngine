/* SPDX-License-Identifier: Apache-2.0 */

/**
 * @file     tests_details.h
 * @brief     
 *
 * @author   Jascit (https://github.com/Jascit)
 * @date     04.01.2026
 *
 * @copyright Copyright (c) 2026 The Project Contributors
 */

#pragma once
#include <makros.hpp>
#include <test_registry.hpp>
#include <testing_system.hpp>

struct TestRegistrar {
  TestRegistrar(const char* suite, const char* name, TestFunc func, const char* file, int line) {
    TestRegistry::instance().get_registry().push_back({ suite, name, func, SUCCEEDED, file, line });
  }
  
};

