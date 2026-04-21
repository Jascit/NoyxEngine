/* SPDX-License-Identifier: Apache-2.0 */

/**
 * @file     test_main.cpp
 * @brief     
 *
 * @author   Jascit (https://github.com/Jascit)
 * @date     04.01.2026
 *
 * @copyright Copyright (c) 2026 The Project Contributors
 */

#include <tests_details.h>
#include <chrono>
#if defined(__has_feature)
#  if __has_feature(address_sanitizer)
#    pragma message("ASAN: __has_feature(address_sanitizer) == 1")
#  else
#    pragma message("ASAN: __has_feature(address_sanitizer) == 0")
#  endif
#elif defined(__SANITIZE_ADDRESS__)
#  pragma message("ASAN: __SANITIZE_ADDRESS__ defined")
#else
#  pragma message("ASAN: NOT enabled")
#endif

std::string format_time(long long nanoseconds) {
  double time = static_cast<double>(nanoseconds);

  struct Unit {
    const char* name;
    double factor;
  };

  static constexpr Unit units[] = {
      Unit{"ns", 1000.0},
      Unit{"us", 1000.0},
      Unit{"ms", 1000.0},
      Unit{"s", 60.0},
      Unit{"min", 60.0},
      Unit{"h", 0.0}
  };

  size_t idx = 0;

  while (idx < 6 - 1 && time >= units[idx].factor) {
    time /= units[idx].factor;
    ++idx;
  }

  char buffer[32];
  if (time < 10)
    std::snprintf(buffer, sizeof(buffer), "%.3f %s", time, units[idx].name);
  else if (time < 100)
    std::snprintf(buffer, sizeof(buffer), "%.2f %s", time, units[idx].name);
  else
    std::snprintf(buffer, sizeof(buffer), "%.0f %s", time, units[idx].name);

  return buffer;
}

int main() {
  auto& registry = TestRegistry::instance().getRegistry();

  std::cout << "Running " << registry.size() << " tests:\n";

  for (auto& info : registry) {
    std::cout << info.suiteName << "." << info.testName << " ... ";
    auto start = std::chrono::steady_clock::now();
    info.testFunc();
    auto end = std::chrono::steady_clock::now();

    auto duration = end - start;
    double us = std::chrono::duration<double, std::nano>(duration).count();
    std::string timeStr = format_time(us);

    if (!(info.flag & FAILED)) {
      std::cout << "OK (" << timeStr << ")\n";
      TestingSystem::instance()->success();
    } else {
      std::cout << "FAILED\n";
    }

    ++TestRegistry::instance();
  }

  return (TestingSystem::instance()->GetFailedCount() == 0 ? EXIT_SUCCESS : EXIT_FAILURE);
}
