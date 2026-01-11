/* SPDX-License-Identifier: Apache-2.0 */
/* 
 * \file   testings_data.hpp
 * \brief  
 * 
 * Copyright (c) 2026 Project Contributors 
 * \author Jascit<https://github.com/Jascit>
 * \date   04.01.2026
 */
#pragma once
#include <vector>
#include <functional>
#include <stdexcept>
#include <sstream>
#include <cstring>
#include <iostream>

enum TEST_FLAG : bool {
  SUCCESSED = 0,
  FAILED = 1
};

using TestFunc = std::function<void()>;

struct TestInfo {
  const char* suiteName;
  const char* testName;
  TestFunc testFunc;
  TEST_FLAG flag;
  const char* file;
  int line;
};
