/* SPDX-License-Identifier: Apache-2.0 */

/**
 * @file     File.hpp
 * @brief
 *
 * @date     26.03.2026
 *
 * @copyright Copyright (c) 2026 The Project Contributors
 */

#pragma once
#include <filesystem>

namespace noyxcore::utils
{
class File
{
public:
    explicit File();

    bool open_file(const std::filesystem::path &path) noexcept;
    void close_file() noexcept;

    bool is_opened() const noexcept { return !path_.empty(); }

    const std::string &data() const noexcept { return buffer_; }

    uint64_t size() const noexcept { return file_size_; }

    bool empty() const noexcept { return buffer_.empty(); }

    std::string_view get_line();

private:
    std::filesystem::path path_;
    uint64_t file_size_;
    std::string buffer_;
    uint64_t current_position_;
};
}