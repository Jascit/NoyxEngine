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
  const char* units[] = { "ns", "us", "ms", "s", "min", "h" };
  int idx = 0;

  while (time > 10000 && idx < 4) { // якщо > 10000, переходимо до наступної одиниці
    if (idx == 0)      time /= 1000;     // ns -> us
    else if (idx == 1) time /= 1000;     // us -> ms
    else if (idx == 2) time /= 1000;     // ms -> s
    else if (idx == 3) time /= 60;       // s -> min
    else if (idx == 4) time /= 60;       // min -> h
    ++idx;
  }

  char buffer[32];
  if (time < 10)
    std::snprintf(buffer, sizeof(buffer), "%.3f %s", time, units[idx]);
  else if (time < 100)
    std::snprintf(buffer, sizeof(buffer), "%.2f %s", time, units[idx]);
  else
    std::snprintf(buffer, sizeof(buffer), "%.0f %s", time, units[idx]);

  return std::string(buffer);
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
    }
    else {
      std::cout << "FAILED\n";
    }

    ++TestRegistry::instance();
  }

  return (TestingSystem::instance()->GetFailedCount() == 0 ? EXIT_SUCCESS : EXIT_FAILURE);
}