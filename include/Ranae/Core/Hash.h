#pragma once

#include <Ranae/Common.h>

namespace ranae {

  constexpr uint32_t hash_string(const char* s, size_t count) {
    return ((count ? hash_string(s, count - 1) : 2166136261u) ^ s[count]) * 16777619u;
  }

  constexpr uint32_t operator"" _hash(const char* s, size_t count) {
    return hash_string(s, count);
  }

}