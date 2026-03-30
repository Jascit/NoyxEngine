/* SPDX-License-Identifier: Apache-2.0 */

/**
 * @file     Logger.hpp
 * @brief     
 *
 * @author   Jascit (https://github.com/Jascit)
 * @date     25.03.2026
 *
 * @copyright Copyright (c) 2026 The Project Contributors
 */

#pragma once
#include <filesystem>

namespace noyxcore::diagnostics {
  enum LogLevel {
    Info = 0,
    Warning,
    Error,
    Fatal,
    Trace,
    Debug
  };
  namespace details {
    constexpr const char* lookup_level_message[6] = {"[INFO] ", "[WARNING] ", "[ERROR] ", "[FATAL] ", "[TRACE] ", "[DEBUG] "};
    constexpr uint32_t lookup_level_length[6] = {7, 10, 8, 8, 8, 8};

    class LogBuffer {
    public:
      LogBuffer(uint64_t size) noexcept : size_(size), write_index_(0), log_buffer_(nullptr) {
        log_buffer_ = new char[size];
      };

      ~LogBuffer() {
        if (log_buffer_ != nullptr) {
          delete[] log_buffer_;
        }
      }

      bool append(const char* msg, LogLevel level) {
        uint32_t msg_length = std::strlen(msg);
        if (write_index_ + msg_length + lookup_level_length[level] > size_) {
          return false;
        }
        append_loglevel_(level);
        memcpy(&log_buffer_[write_index_], msg, msg_length);
        write_index_ += msg_length;
        log_buffer_[write_index_] = '\n';
        log_buffer_[write_index_ + 1] = '\0';
        write_index_++;
        return true;
      };

      const char* cstr() const { return log_buffer_; };

    private:
      void append_loglevel_(LogLevel level) noexcept {
        uint32_t level_index = static_cast<uint32_t>(level);
        uint32_t length = lookup_level_length[level_index];
        memcpy(&log_buffer_[write_index_], lookup_level_message[level_index], length);
        write_index_ += length;
      };

    private:
      char* log_buffer_;
      uint64_t write_index_;
      uint64_t size_;
    };
  }

  class Logger {
  public:
    Logger(const char* log_file_name);
    bool log(const char* msg, LogLevel level);
    void flush() const;

  private:
    std::filesystem::path log_file_path_;
    details::LogBuffer log_buffer_;
  };
}
