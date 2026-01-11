/* SPDX-License-Identifier: Apache-2.0 */
/* 
 * \file   test_registry.hpp
 * \brief  
 * 
 * Copyright (c) 2026 Project Contributors 
 * \author Jascit<https://github.com/Jascit>
 * \date   04.01.2026
 */
#pragma once
#include <testings_data.hpp>
class TestRegistry {
public:
  inline std::vector<TestInfo>& getRegistry() {
    return m_registry;
  }

  auto begin() {
    return m_registry.begin();
  }

  auto end() {
    return m_registry.end();
  }
  
  TestRegistry& operator++() {
    m_currentRegistry++;
    return *(this);
  }   

  size_t GetCurrentRegistry() {
    return m_currentRegistry;
  }
  
  TestInfo& GetCurrentTestInfo() {
    return m_registry[m_currentRegistry];
  }

private:
  std::vector<TestInfo> m_registry;
  size_t m_currentRegistry;

public:
  TestRegistry operator=(const TestRegistry&) = delete;
  static inline TestRegistry& instance() {
    static TestRegistry registryInstance;
    return registryInstance;
  }

private:
  TestRegistry() : m_currentRegistry(0) {};
};
