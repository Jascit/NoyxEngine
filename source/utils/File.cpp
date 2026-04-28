/* SPDX-License-Identifier: Apache-2.0 */

/**
 * @file     File.cpp
 * @brief     
 *
 * @date     26.03.2026
 *
 * @copyright Copyright (c) 2026 The Project Contributors
 */

#include <utils/File.hpp>
#include <fstream>
using namespace noyxcore::utils;

File::File() : file_size_(0), current_position_(0) {}

bool File::open_file(const std::filesystem::path& path) noexcept {
  std::ifstream file(path, std::ios::binary);

  if (!file.is_open()) {
    return false;
  }

  file.seekg(0, std::ios::end);
  file_size_ = file.tellg();
  file.seekg(0, std::ios::beg);

  buffer_.reserve(file_size_);

  if (!file.read(buffer_.data(), file_size_)) {
    buffer_.resize(0);
    return false;
  };

  path_ = path;
  return true;
}

void File::close_file() noexcept {
  if (!path_.empty()) {
    path_.clear();
    buffer_.clear();
    file_size_ = 0;
    current_position_ = 0;
  }
}

std::string_view File::get_line() {
  using char_traits = std::char_traits<char>;
  auto meta_delimiter = char_traits::to_char_type('\n');

  std::string_view line(buffer_.c_str() + current_position_, file_size_ - current_position_);
  const int64_t FF = file_size_ - current_position_;
  uint64_t length = 0;

  for (;;) {
    if (length >= FF) break;
    const char META = line[length];
    if (META == meta_delimiter) break;
    length++;
  }

  if (length == 0) {
    throw std::runtime_error("Can not find the line in buffer");
  }

  std::string_view founded_line = line.substr(0, length);
  current_position_ += length + 1;
  return founded_line;
}
