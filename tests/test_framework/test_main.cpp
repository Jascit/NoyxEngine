#include <tests_details.hpp>
#include <chrono>

int main() {
  auto& registry = TestRegistry::instance().getRegistry();

  std::cout << "Running " << registry.size() << " tests:\n";
  
  for (auto& info : registry) {
    std::cout << info.suiteName << "." << info.testName << " ... ";
    auto start = std::chrono::steady_clock::now();
    info.testFunc();
    auto end = std::chrono::steady_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    if (!(info.flag & FAILED)) {
      std::cout << "OK (" << ms << " ms)\n";
      TestingSystem::instance()->success();
    }
    else {
      std::cout << "FAILED\n";
    }
    ++TestRegistry::instance();
  }

  return (TestingSystem::instance()->GetFailedCount() == 0 ? EXIT_SUCCESS : EXIT_FAILURE);
}