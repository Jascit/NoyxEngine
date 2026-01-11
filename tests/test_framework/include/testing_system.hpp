/* SPDX-License-Identifier: Apache-2.0 */
/* 
 * \file   testing_system.hpp
 * \brief  
 * 
 * Copyright (c) 2026 Project Contributors 
 * \author Jascit<https://github.com/Jascit>
 * \date   04.01.2026
 */
#pragma once
#include <testings_data.hpp>
#include <test_registry.hpp>
#include <iostream>
#include <vector>
#include <string>

class TestingSystem {
public:
  void fail(std::string message) {
    auto& info = TestRegistry::instance().GetCurrentTestInfo();
    if (!(info.flag & FAILED)) {
      info.flag = FAILED;
      m_failedFunctionsMessages.push_back(std::vector<std::string>{});
      m_failedMakrosCount.push_back(0);
      m_failedFunctionsNames.push_back(info.suiteName);
      ++m_failed;
    }
    m_failedFunctionsMessages[m_failed-1].push_back(message);
    m_failedMakrosCount[m_failed-1]++;
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

  TestingSystem(const TestingSystem&) = delete;
  TestingSystem& operator=(const TestingSystem&) = delete;
  static TestingSystem* instance() {
    static TestingSystem systemInstance;
    return &systemInstance;
  }

private:
  TestingSystem() : m_passed(0), m_failed(0) {}

  void report() {
    std::cout << "\n========== Test Summary ==========\n";
    std::cout << "Passed: " << m_passed << "\n";
    std::cout << "Failed: " << m_failed << "\n";

    if (!m_failedFunctionsNames.empty()) {
      std::cout << "\nFailed Tests:\n";
      for (int i = 0; i < m_failedFunctionsMessages.size(); i++) {
        std::cout << "-----" << m_failedFunctionsNames[i] << "-----" << "\n";
        std::cout << "Failed NOYX_MAKROS: " << std::to_string(m_failedMakrosCount[i]) << "\n";
        for (auto& message : m_failedFunctionsMessages[i]) {
            std::cout << "  - " << m_failedFunctionsNames[i] << ": " << message << "\n";
        }
      }
    }

    std::cout << "==================================\n";
  }

private:
  size_t m_passed;
  size_t m_failed;
  std::vector<std::string> m_failedFunctionsNames;
  std::vector<std::vector<std::string>> m_failedFunctionsMessages;
  std::vector<size_t> m_failedMakrosCount;
};
