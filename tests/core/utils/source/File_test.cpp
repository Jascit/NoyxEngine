/* SPDX-License-Identifier: Apache-2.0 */

/**
 * @file     File_test.cpp
 * @brief     
 *
 * @author   Jascit (https://github.com/Jascit)
 * @date     27.03.2026
 *
 * @copyright Copyright (c) 2026 The Project Contributors
 */

#include <utils/File.hpp>
#include <tests_details.h>
#include <fstream>

//NOYX_TEST(file_test, smoke_test) {
//  std::string data = "some data 1 \n some data 2, some data 3 \n some data 4";
//  std::filesystem::path test_path("files/test.txt");
//  if (!std::filesystem::exists(test_path.parent_path())) {
//    std::filesystem::create_directories(test_path.parent_path());
//  }
//  // prepare a file to read
//  std::ofstream file(test_path, std::ios::binary);
//  if (file.is_open()) {
//    file << data;
//  }
//  file.close();
//
//  noyxcore::utils::File file_test;
//  file_test.open_file(test_path);
//  for (;;) {
//    try {
//      std::cout << file_test.get_line() << std::endl;
//    } catch (...) { break; }
//  }
//}
