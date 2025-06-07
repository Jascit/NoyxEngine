#include <tests_details.hpp>
#include <iostream>

int main() {
  auto& registry = getTestRegistry();
  int passed = 0, failed = 0;

  std::cout << "Running " << registry.size() << " tests:\n\n";
  
  for (auto& info : registry) {
    std::cout << info.suiteName << "." << info.testName << " ... ";
    try {
      info.testFunc();
      std::cout << "OK\n\n";
      ++passed;
    }
    catch (const std::exception& ex) {
      std::cout << "FAILED (" << ex.what() << ")\n";
      ++failed;
    }
    catch (...) {
      std::cout << "FAILED (unknown exception)\n";
      ++failed;
    }
  }

  std::cout << "\nSummary: "
    << passed << " passed, "
    << failed << " failed.\n";

  return (failed == 0 ? EXIT_SUCCESS : EXIT_FAILURE);
}