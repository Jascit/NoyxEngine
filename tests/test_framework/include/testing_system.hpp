#pragma once
#include <testings_data.hpp>
#include <iostream>
#include <vector>
#include <string>

class TestingSystem {
public:
  void fail(TestInfo& info, const char* message) {
    m_failedFunctionsNames.push_back(info.testName);
    m_failedFunctionsMessages.push_back(message);
    ++m_failed;
  }

  void success() {
    ++m_passed;
  }

  ~TestingSystem() {
    report();
  }

  size_t GetFailedCount() const {
    return m_failed;
  }

  size_t GetPassedCount() const {
    return m_passed;
  }

  static TestingSystem* instance() {
    static TestingSystem systemInstance;
    return &systemInstance;
  }

  TestingSystem(const TestingSystem&) = delete;
  TestingSystem& operator=(const TestingSystem&) = delete;

private:
  TestingSystem() : m_passed(0), m_failed(0) {}

  void report() {
    std::cout << "\n========== Test Summary ==========\n";
    std::cout << "Passed: " << m_passed << "\n";
    std::cout << "Failed: " << m_failed << "\n";

    if (!m_failedFunctionsNames.empty()) {
      std::cout << "\nFailed Tests:\n";
      for (size_t i = 0; i < m_failedFunctionsNames.size(); ++i) {
        std::cout << "  - " << m_failedFunctionsNames[i] << ": " << m_failedFunctionsMessages[i] << "\n";
      }
    }

    std::cout << "==================================\n";
  }

private:
  size_t m_passed;
  size_t m_failed;
  std::vector<std::string> m_failedFunctionsNames;
  std::vector<std::string> m_failedFunctionsMessages;
};
