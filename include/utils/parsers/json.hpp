/* SPDX-License-Identifier: Apache-2.0 */

/**
 * @file     json.hpp
 * @brief     
 *
 * @date     26.03.2026
 *
 * @copyright Copyright (c) 2026 The Project Contributors
 */

#pragma once
#include <string>
#include <filesystem>
#include <unordered_map>
#include <concepts>

namespace noyxcore::utils {
  template<typename Alloc>
  class JSON;
  namespace details {
    enum JSONType : uint32_t{
      boolean,
      number,
      string,
      floating_point,
      null,
      array,
      object
    };

    struct ArrayType {
      JSONType type;
      void* data;
      uint64_t size;
    };

    template <typename Alloc>
    union JSONValue {
      uint64_t boolean;
      uint64_t number;
      const char* string; // mb int32 offset; int32 length;
      double floating_point;
      uint64_t null;
      ArrayType* array;
      JSON<Alloc>* object;
    };

    template <typename Alloc>
    bool parse_json_file(const std::filesystem::path& path, JSON<Alloc>* json_obj);

    template <typename Alloc>
    bool parse_json_string(const char* str, JSON<Alloc>* json_obj);

    template <typename Alloc>
    bool parse_json_string(std::string& str, JSON<Alloc>* json_obj);

  }

  template <typename Alloc>
  class JSON {
  public:
    constexpr JSON(const char* json);
    constexpr JSON(std::string json);
    constexpr JSON(std::filesystem::path const& path);

    constexpr std::string str() const;
  private:                                  //Index     TypeID
    std::unordered_map<std::string, std::pair<uint32_t, details::JSONType>> json_;
    std::vector<details::JSONValue<Alloc>> json_data_;
  };
}
