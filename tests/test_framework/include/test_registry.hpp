#pragma once
#include <testings_data.hpp>
class TestRegistry {
public:
  inline std::vector<TestInfo>& getRegistry() {
    return m_registry;
  }

private:
  std::vector<TestInfo> m_registry;
public:
  TestRegistry operator=(const TestRegistry&) = delete;
  static inline TestRegistry& instance() {
    static TestRegistry registryInstance;
    return registryInstance;
  }
private:
  TestRegistry() = default;
};
