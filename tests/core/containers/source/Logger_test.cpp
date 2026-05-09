/* SPDX-License-Identifier: Apache-2.0 */

/**
 * @file     Logger_test.cpp
 * @brief     
 *
 * @author   Jascit (https://github.com/Jascit)
 * @date     25.03.2026
 *
 * @copyright Copyright (c) 2026 The Project Contributors
 */

#include <thread>
#include <vector>
#include <diagnostics/Logger.hpp>
#include <tests_details.h>

using namespace noyxcore::diagnostics;

NOYX_TEST(logger_test, smoke_test) {
  Logger logger("log.txt");
  NOYX_ASSERT_TRUE(logger.log("Error1", INFO));
  NOYX_ASSERT_TRUE(logger.log("Error2öä", ERROR));
  NOYX_ASSERT_TRUE(logger.log("ssss", WARNING));
  NOYX_ASSERT_TRUE(logger.log("Message", DEBUG));
  logger.flush();
}

NOYX_TEST(logger_test, buffer_overflow_test) {
  Logger logger("overflow_log.txt");
  std::string long_msg(1000, 'X');

  int successful_logs = 0;
  for (int i = 0; i < 100; ++i) {
    if (logger.log(long_msg.c_str(), INFO)) {
      successful_logs++;
    }
  }

  NOYX_ASSERT_TRUE(successful_logs > 0);
  NOYX_ASSERT_TRUE(successful_logs < 100);
  NOYX_ASSERT_FALSE(logger.log("This should fail", ERROR));
  logger.flush();
}

NOYX_TEST(logger_test, giant_message_test) {
  Logger logger("giant_log.txt");
  std::string giant_msg(70000, 'Z');
  NOYX_ASSERT_FALSE(logger.log(giant_msg.c_str(), FATAL));
}

NOYX_TEST(logger_test, multithreaded_stress_test) {
  Logger logger("mt_log.txt");
  std::vector<std::thread> threads;

  for (int i = 0; i < 10; ++i) {
    threads.emplace_back([&logger]() {
      for (int j = 0; j < 500; ++j) {
        logger.log("Concurrent spam message from thread", TRACE);
      }
    });
  }

  for (auto& t : threads) {
    t.join();
  }
  logger.flush();
  NOYX_ASSERT_TRUE(true);
}

NOYX_TEST(logger_test, empty_flush_test) {
  Logger logger("empty_log.txt");
  logger.flush();
  NOYX_ASSERT_TRUE(true);
}