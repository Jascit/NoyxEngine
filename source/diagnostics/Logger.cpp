/* SPDX-License-Identifier: Apache-2.0 */

/**
 * @file     Logger.cpp
 * @brief     
 *
 * @date     25.03.2026
 *
 * @copyright Copyright (c) 2026 The Project Contributors
 */

#include <diagnostics/Logger.hpp>
#include <fstream>
#include <string>

using namespace noyxcore::diagnostics;

Logger::Logger(const char* log_file_name) : log_buffer_(64*1024) {
  std::filesystem::path process_path = std::move(std::filesystem::current_path());
  std::filesystem::path logs_path = process_path.parent_path().parent_path();
  logs_path /= "logs";
  if (!std::filesystem::exists(logs_path)) {
    std::filesystem::create_directories(logs_path);
  }
  std::filesystem::path log_file_path = logs_path / log_file_name;
  log_file_path_ = std::move(log_file_path);
}

bool Logger::log(const char* msg, LogLevel level) {
  return log_buffer_.append(msg, level);
}

void Logger::flush() const {
  std::ofstream log_file(log_file_path_);
  if (log_file.is_open()) {
    log_file << log_buffer_.cstr();
  }
}
