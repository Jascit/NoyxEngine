/* SPDX-License-Identifier: Apache-2.0 */

/**
 * @file    BitMap.hpp
 * @brief
 *
 * @date    24/06/2026
 *
 * @copyright Copyright (c) 2026 The Project Contributors
 */

#pragma once

namespace noyxcore::containers
{

class BitMap
{
public:
    BitMap();
    BitMap(const BitMap &other);
    BitMap(BitMap &&other);
    BitMap &operator=(const BitMap &other);
    BitMap &operator=(BitMap &&other);
    ~BitMap();

private:
};

} // namespace noyxcore::containers