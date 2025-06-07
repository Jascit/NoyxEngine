#pragma once
#include <vector>
#include <functional>
#include <stdexcept>
#include <sstream>
#include <cstring>
#include <iostream>

using TestFunc = std::function<void()>;

struct TestInfo {
  const char* suiteName;
  const char* testName;
  TestFunc testFunc;
  const char* file;
  int line;
};
