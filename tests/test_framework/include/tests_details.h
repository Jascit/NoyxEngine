/* SPDX-License-Identifier: Apache-2.0 */
/* 
 * \file   tests_details.h
 * \brief  
 * 
 * Copyright (c) 2026 Project Contributors 
 * \author Jascit<https://github.com/Jascit>
 * \date   04.01.2026
 */
#pragma once
#include <testing_system.hpp>
#include <test_registry.hpp>
#include <makros.hpp>

struct TestRegistrar {
  TestRegistrar(const char* suite, const char* name, TestFunc func, const char* file, int line) {
    TestRegistry::instance().getRegistry().push_back({ suite, name, func, SUCCESSED, file, line });
  }
  
};

