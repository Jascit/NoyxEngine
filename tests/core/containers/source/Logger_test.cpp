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
