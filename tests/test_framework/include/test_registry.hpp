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
