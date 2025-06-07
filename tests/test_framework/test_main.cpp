#include <tests_details.hpp>
#include <chrono>

int main() {
  auto& registry = TestRegistry::instance().getRegistry();

  std::cout << "Running " << registry.size() << " tests:\n\n";
  
  for (auto& info : registry) {
    std::cout << info.suiteName << "." << info.testName << " ... ";
    try {
      auto start = std::chrono::steady_clock::now();
      info.testFunc();
      auto end = std::chrono::steady_clock::now();
      auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
      std::cout << "OK (" << ms << " ms)\n\n";

      TestingSystem::instance()->success();
    }
    catch (const std::exception& ex) {
      std::cout << "FAILED (" << ex.what() << ")\n";
      TestingSystem::instance()->fail(info, ex.what());
    }
    catch (...) {
      std::cout << "FAILED (unknown exception)\n";
      TestingSystem::instance()->fail(info, "FAILED (unknown exception)\n");
    }
  }

  return (TestingSystem::instance()->GetFailedCount() == 0 ? EXIT_SUCCESS : EXIT_FAILURE);
}