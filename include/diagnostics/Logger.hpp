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
#include <atomic>
#include <cstring>
#include <filesystem>

namespace noyxcore::diagnostics
{
enum LogLevel
{
    INFO = 0,
    WARNING,
    ERROR,
    FATAL,
    TRACE,
    DEBUG,
    JSON
};

namespace details
{
constexpr const char *LOOKUP_LEVEL_MESSAGE[7] = {"[INFO] ",  "[WARNING] ", "[ERROR] ", "[FATAL] ",
                                                 "[TRACE] ", "[DEBUG] ",   ""};
constexpr const char *LOOKUP_LEVEL_NAME[7]    = {"INFO",  "WARNING", "ERROR", "FATAL",
                                                 "TRACE", "DEBUG",   "JSON"};
constexpr uint32_t LOOKUP_LEVEL_LENGTH[7]     = {7, 10, 8, 8, 8, 8, 0};

inline void format_current_time(char *time_buffer, size_t buffer_size)
{
    auto now          = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);

    std::tm parts;
#if defined(_WIN32)
    localtime_s(&parts, &now_c);
#else
    localtime_r(&now_c, &parts);
#endif
    std::strftime(time_buffer, buffer_size, "%H:%M:%S", &parts);
}

class LogBuffer
{
public:
    LogBuffer(uint64_t size) noexcept
        : m_log_buffer(nullptr),
          m_write_index(0),
          m_size(size)
    {
        m_log_buffer = new char[size];
    };

    ~LogBuffer()
    {
        if (m_log_buffer != nullptr)
        {
            delete[] m_log_buffer;
        }
    }

    bool append(const char *msg, LogLevel level)
    {
        uint64_t msg_len    = std::strlen(msg);
        uint64_t prefix_len = LOOKUP_LEVEL_LENGTH[static_cast<uint32_t>(level)];
        uint64_t total_len  = msg_len + prefix_len + 1;

        uint64_t my_offset = m_write_index.fetch_add(total_len, std::memory_order_relaxed);
        if (total_len + my_offset > m_size)
        {
            return false;
        }

        const char *prefix_msg = LOOKUP_LEVEL_MESSAGE[static_cast<uint32_t>(level)];
        memcpy(&m_log_buffer[my_offset], prefix_msg, prefix_len);
        memcpy(&m_log_buffer[my_offset + prefix_len], msg, msg_len);
        m_log_buffer[my_offset + prefix_len + msg_len] = '\n';
        return true;
    };

    uint64_t get_written_bytes() const
    {
        uint64_t current = m_write_index.load(std::memory_order_acquire);
        return (current > m_size) ? m_size : current;
    }

    const char *data() const { return m_log_buffer; };

private:
    char *m_log_buffer;
    std::atomic<uint64_t> m_write_index;
    uint64_t m_size;
};
} // namespace details

class Logger
{
public:
    Logger(const char *log_file_name);
    bool log(const char *msg, LogLevel level);
    bool log_json(LogLevel level, const char *msg);
    void flush() const;

private:
    std::filesystem::path m_log_file_path;
    details::LogBuffer m_log_buffer;
};
} // namespace noyxcore::diagnostics
