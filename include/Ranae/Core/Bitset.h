#pragma once

#include <Ranae/Common.h>

namespace Ranae {

  template <size_t BitCount>
  struct Bitset {
    static constexpr size_t DwordCount = align(BitCount, 32) / 32;

    constexpr Bitset()
      : dwords{ } { }


    constexpr bool get(size_t idx) const {
      uint32_t dword = 0u;
      uint32_t bit   = idx;

      if constexpr (DwordCount > 1u) {
        dword = idx / 32u;
        bit   = idx % 32u;
      }

      return dwords[dword] & (1u << bit);
    }


    constexpr void set(uint32_t idx, bool value) {
      uint32_t dword = 0u;
      uint32_t bit   = idx;

      if constexpr (DwordCount > 1u) {
        dword = idx / 32u;
        bit   = idx % 32u;
      }

      if (value)
        dwords[dword] |= 1u << bit;
      else
        dwords[dword] &= ~(1u << bit);
    }


    constexpr bool exchange(uint32_t idx, bool value) {
      bool oldValue = get(idx);
      set(idx, value);
      return oldValue;
    }


    constexpr void flip(uint32_t idx) {
      uint32_t dword = 0u;
      uint32_t bit   = idx;

      if constexpr (DwordCount > 1) {
        dword = idx / 32u;
        bit   = idx % 32u;
      }

      dwords[dword] ^= 1u << bit;
    }


    constexpr void setAll() {
      if constexpr (BitCount % 32u == 0u) {
        for (size_t i = 0; i < DwordCount; i++)
          dwords[i] = std::numeric_limits<uint32_t>::max();
      }
      else {
        for (size_t i = 0; i < DwordCount - 1u; i++)
          dwords[i] = std::numeric_limits<uint32_t>::max();

        dwords[DwordCount - 1] = (1u << (BitCount % 32u)) - 1u;
      }
    }


    constexpr void clearAll() {
      for (size_t i = 0; i < DwordCount; i++)
        dwords[i] = 0u;
    }


    constexpr bool any() const {
      for (size_t i = 0; i < DwordCount; i++) {
        if (dwords[i] != 0u)
          return true;
      }

      return false;
    }


    constexpr bool operator [] (uint32_t idx) const {
      return get(idx);
    }


    std::array<uint32_t, DwordCount> dwords;

  };

}