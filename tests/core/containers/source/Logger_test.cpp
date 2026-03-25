/* SPDX-License-Identifier: Apache-2.0 */
/*
 * \file   Logger_test.cpp
 * \brief  
 *
 * Copyright (c) 2026 Project Contributors
 * \author Jascit <https://github.com/Jascit>
 * \date   25.03.2026
 * \note   
 */

#include <diagnostics/Logger.hpp>
#include <tests_details.h>

using namespace noyxcore::diagnostics;

NOYX_TEST(logger_test, smoke_test) {
  Logger logger("log.txt");
  NOYX_ASSERT_TRUE(logger.log("Error1", Info));
  NOYX_ASSERT_TRUE(logger.log("Error2öä", Error));
  NOYX_ASSERT_TRUE(logger.log("ssss", Warning));
  NOYX_ASSERT_TRUE(logger.log("Message", Debug));
  logger.flush();
}
