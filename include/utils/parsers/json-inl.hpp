/* SPDX-License-Identifier: Apache-2.0 */

/**
 * @file     json-inl.hpp
 * @brief
 *
 * @date     26.03.2026
 *
 * @copyright Copyright (c) 2026 The Project Contributors
 */

#pragma once

#include "json.hpp"
#include <fstream>
#include <utils/File.hpp>

namespace noyxcore::utils
{
namespace details
{
template <typename Alloc>
bool parse_json_file(const std::filesystem::path &path, JSON<Alloc> *json_obj)
{
    std::ofstream json_file(path, std::ios::in);

    return true;
};

template <typename Alloc>
bool parse_json_string(const char *str, JSON<Alloc> *json_obj);

template <typename Alloc>
bool parse_json_string(std::string &str, JSON<Alloc> *json_obj);
} // namespace details
} // namespace noyxcore::utils
