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
#include <thread>
#include <cstdio>

using namespace noyxcore::diagnostics;

Logger::Logger(const char* log_file_name) : m_log_buffer(64*1024) {
  std::filesystem::path process_path = std::filesystem::current_path();
  std::filesystem::path logs_path = process_path.parent_path().parent_path();
  logs_path /= "logs";

  if (!std::filesystem::exists(logs_path)) {
    std::filesystem::create_directories(logs_path);
  }

  std::filesystem::path log_file_path = logs_path / log_file_name;
  m_log_file_path = log_file_path;
}

bool Logger::log(const char* msg, LogLevel level) {
  return m_log_buffer.append(msg, level);
}

bool Logger::log_json(LogLevel original_level, const char* json_payload) {
  char time_buf[16];
  details::format_current_time(time_buf, sizeof(time_buf));

  size_t thread_id = std::hash<std::thread::id>{}(std::this_thread::get_id()) % 10000;

  const char* level_name = details::LOOKUP_LEVEL_NAME[static_cast<uint32_t>(original_level)];

  char final_json_buffer[512];

  int written = std::snprintf(final_json_buffer, sizeof(final_json_buffer),
      "{\"time\":\"%s\", \"thread\":%zu, \"level\":\"%s\", \"data\":%s}",
      time_buf, thread_id, level_name, json_payload);

  if (written < 0) {
    return false;
  }
  return m_log_buffer.append(final_json_buffer, JSON);
}

void Logger::flush() const {
  std::ofstream log_file(m_log_file_path, std::ios::binary);

  if (log_file.is_open()) {
    log_file.write(m_log_buffer.data(), m_log_buffer.get_written_bytes());
  }
}
