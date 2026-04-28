/* SPDX-License-Identifier: Apache-2.0 */

/**
 * @file     Logger.hpp
 * @brief     
 *
 * @date     25.03.2026
 *
 * @copyright Copyright (c) 2026 The Project Contributors
 */

#pragma once
#include <filesystem>
#include <cstring>

namespace noyxcore::diagnostics {
  enum LogLevel {
    INFO = 0,
    WARNING,
    ERROR,
    FATAL,
    TRACE,
    DEBUG
  };
  namespace details {
    constexpr const char* LOOKUP_LEVEL_MESSAGE[6] = {"[INFO] ", "[WARNING] ", "[ERROR] ", "[FATAL] ", "[TRACE] ", "[DEBUG] "};
    constexpr uint32_t LOOKUP_LEVEL_LENGTH[6] = {7, 10, 8, 8, 8, 8};

    class LogBuffer {
    public:
      LogBuffer(uint64_t size) noexcept : m_log_buffer(nullptr), m_write_index(0), m_size(size) {
        m_log_buffer = new char[size];
      };

      ~LogBuffer() {
        if (m_log_buffer != nullptr) {
          delete[] m_log_buffer;
        }
      }

      bool append(const char* msg, LogLevel level) {
        uint32_t msg_length = std::strlen(msg);
        if (m_write_index + msg_length + LOOKUP_LEVEL_LENGTH[level] > m_size) {
          return false;
        }
        append_loglevel_(level);
        memcpy(&m_log_buffer[m_write_index], msg, msg_length);
        m_write_index += msg_length;
        m_log_buffer[m_write_index] = '\n';
        m_log_buffer[m_write_index + 1] = '\0';
        m_write_index++;
        return true;
      };

      const char* cstr() const { return m_log_buffer; };

    private:
      void append_loglevel_(LogLevel level) noexcept {
        uint32_t level_index = static_cast<uint32_t>(level);
        uint32_t length = LOOKUP_LEVEL_LENGTH[level_index];
        memcpy(&m_log_buffer[m_write_index], LOOKUP_LEVEL_MESSAGE[level_index], length);
        m_write_index += length;
      };

    private:
      char* m_log_buffer;
      uint64_t m_write_index;
      uint64_t m_size;
    };
  } // namespace details

  class Logger {
  public:
    Logger(const char* log_file_name);
    bool log(const char* msg, LogLevel level);
    void flush() const;

  private:
    std::filesystem::path m_log_file_path;
    details::LogBuffer m_log_buffer;
  };
} // namespace noyxcore::diagnostics
