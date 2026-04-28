/* SPDX-License-Identifier: Apache-2.0 */

/**
 * @file     test_registry.hpp
 * @brief     
 *
 * @author   Jascit (https://github.com/Jascit)
 * @date     04.01.2026
 *
 * @copyright Copyright (c) 2026 The Project Contributors
 */

#pragma once
#include <testings_data.hpp>
class TestRegistry {
public:
  inline std::vector<TestInfo>& get_registry() {
    return m_registry;
  }

  auto begin() {
    return m_registry.begin();
  }

  auto end() {
    return m_registry.end();
  }
  
  TestRegistry& operator++() {
    m_current_registry++;
    return *(this);
  }   

  size_t get_current_registry() const {
    return m_current_registry;
  }
  
  TestInfo& get_current_test_info() {
    return m_registry[m_current_registry];
  }

private:
  std::vector<TestInfo> m_registry;
  size_t m_current_registry;

public:
  TestRegistry operator=(const TestRegistry&) = delete;
  static inline TestRegistry& instance() {
    static TestRegistry registry_instance;
    return registry_instance;
  }

private:
  TestRegistry() : m_current_registry(0) {};
};
