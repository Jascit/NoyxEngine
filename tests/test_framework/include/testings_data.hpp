/* SPDX-License-Identifier: Apache-2.0 */

/**
 * @file     testings_data.hpp
 * @brief     
 *
 * @author   Jascit (https://github.com/Jascit)
 * @date     04.01.2026
 *
 * @copyright Copyright (c) 2026 The Project Contributors
 */

#pragma once
#include <vector>
#include <functional>
#include <stdexcept>
#include <sstream>
#include <cstring>
#include <iostream>

enum TEST_FLAG : bool {
  SUCCEEDED = 0,
  FAILED = 1
};

using TestFunc = std::function<void()>;

struct TestInfo {
  const char* suite_name;
  const char* test_name;
  TestFunc test_func;
  TEST_FLAG flag;
  const char* file;
  int line;
};
