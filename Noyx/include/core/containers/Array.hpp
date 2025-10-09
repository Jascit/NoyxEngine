#pragma once

#include <type_traits>
#include <../../platform/Typedef.hpp>
#include "XMemory.hpp"
#include <vector>

template<typename T, typename Allocator>
class TArray {
  _GET_PROXY_ALLOCATOR()
private:
  TCompressedPair<Allocator, T> pair_;
};