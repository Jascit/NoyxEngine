#pragma once
#include <testing_system.hpp>
#include <test_registry.hpp>
#include <makros.hpp>
struct TestRegistrar {
  TestRegistrar(const char* suite, const char* name, TestFunc func, const char* file, int line) {
    TestRegistry::instance().getRegistry().push_back({ suite, name, func, SUCCESSED, file, line });
  }
  
};

