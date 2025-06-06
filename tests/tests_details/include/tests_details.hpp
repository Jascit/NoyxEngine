#pragma once
#include <vector>
#include <functional>
using TestFunc = std::function<void()>;

struct TestRegistry {
  static std::vector<TestFunc>& get() {
    static std::vector<TestFunc> tests;
    return tests;
  }
  static void registerTest(TestFunc f) {
    get().push_back(f);
  }
};

struct TestRegistrar {
  TestRegistrar(TestFunc f) {
    TestRegistry::registerTest(f);
  }
};

#define REGISTER_TEST(fn) \
    static TestRegistrar _registrar_##fn(fn)
#define TEST_BEGIN()

#define TEST_END() 