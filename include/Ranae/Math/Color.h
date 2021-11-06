#pragma once

#include <cstdint>
#include <array>
#include <limits>

namespace ranae {

  template <typename T>
  struct RGBAColor {
    static constexpr T NormalizedMinValue = T{};
    static constexpr T NormalizedMaxValue = T{ std::is_floating_point<T>::value ? 1.0 : std::numeric_limits<T>::max() };
    static constexpr T PackScale          = NormalizedMaxValue / T{ 255 };

    explicit RGBAColor(T r, T g, T b, T a)
      : r{r}, g{g}, b{b}, a{a} {}

    explicit RGBAColor(T r, T g, T b)
      : RGBAColor{ r, g, b, NormalizedMaxValue } {}

    explicit RGBAColor(uint32_t rgba)
      : RGBAColor{ ((rgba >> 24u) & 0xFFu) * PackScale,
                   ((rgba >> 16u) & 0xFFu) * PackScale,
                   ((rgba >>  8u) & 0xFFu) * PackScale,
                   ((rgba >>  0u) & 0xFFu) * PackScale } {}

    uint32_t pack() {
      return uint32_t{ std::clamp(r / PackScale, 0u, 255u) } << 24u |
             uint32_t{ std::clamp(g / PackScale, 0u, 255u) } << 16u |
             uint32_t{ std::clamp(b / PackScale, 0u, 255u) } <<  8u |
             uint32_t{ std::clamp(a / PackScale, 0u, 255u) } <<  0u;
    }

    union {
      std::array<T, 4> data;
      struct {
        T r, g, b, a;
      };
    };

  };

}
