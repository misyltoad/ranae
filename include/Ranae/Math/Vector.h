#pragma once

#include <Ranae/Common.h>

namespace ranae {

  template <typename T, size_t Size>
  struct Vector {
    constexpr Vector()
      : data{ } { }

    constexpr explicit Vector(T splat) {
      std::fill(data.begin(), data.end(), splat);
    }

    constexpr Vector(const T components[Size]) {
      std::copy(&components[0], &components[Size], data.begin());
    }

    template <typename... Args>
    constexpr Vector(const Args&... args)
      : data {{ args... }} {
      static_assert(sizeof...(Args) == Size);
    }

    constexpr Vector(const Vector<T, Size>& other) = default;


    constexpr       T& operator[](size_t index)       { return data[index]; }
    constexpr const T& operator[](size_t index) const { return data[index]; }


    constexpr const T* begin() const { return data.begin(); }
    constexpr       T* begin()       { return data.begin(); }
    constexpr const T* end()   const { return data.end();   }
    constexpr       T* end()         { return data.end();   }


    constexpr bool operator==(const Vector<T, Size>& other) const {
      return std::equal(begin(), end(), other.begin());
    }

    constexpr bool operator!=(const Vector<T, Size>& other) const {
      return !operator==(other);
    }
  

    constexpr Vector<T, Size> operator-() const {
      return util::transform_result<Vector<T, Size>>(begin(), end(), std::negate());
    }
  

    constexpr Vector<T, Size> operator+(const Vector<T, Size>& other) const {
      return util::transform_result<Vector<T, Size>>(begin(), end(), other.begin(), std::plus());
    }

    constexpr Vector<T, Size> operator-(const Vector<T, Size>& other) const {
      return util::transform_result<Vector<T, Size>>(begin(), end(), other.begin(), std::minus());
    }

    constexpr Vector<T, Size> operator*(const Vector<T, Size>& other) const {
      return util::transform_result<Vector<T, Size>>(begin(), end(), other.begin(), std::multiplies());
    }

    constexpr Vector<T, Size> operator/(const Vector<T, Size>& other) const {
      return util::transform_result<Vector<T, Size>>(begin(), end(), other.begin(), std::divides());
    }


    constexpr Vector<T, Size>& operator+=(const Vector<T, Size>& other) {
      std::transform(begin(), end(), other.begin(), begin(), std::plus());
      return *this;
    }

    constexpr Vector<T, Size>& operator-=(const Vector<T, Size>& other) {
      std::transform(begin(), end(), other.begin(), begin(), std::minus());
      return *this;
    }

    constexpr Vector<T, Size>& operator*=(const Vector<T, Size>& other) {
      std::transform(begin(), end(), other.begin(), begin(), std::multiplies());
      return *this;
    }

    constexpr Vector<T, Size>& operator/=(const Vector<T, Size>& other) {
      std::transform(begin(), end(), other.begin(), begin(), std::divides());
      return *this;
    }


    constexpr Vector<T, Size> operator*(const T& scalar) const {
      return util::transform_result<Vector<T, Size>>(begin(), end(), [scalar](T value) { return value * scalar; });
    }

    constexpr Vector<T, Size> operator/(const T& scalar) const {
      return util::transform_result<Vector<T, Size>>(begin(), end(), [scalar](T value) { return value / scalar; });
    }

    constexpr Vector<T, Size>& operator*=(const T& scalar) {
      std::transform(begin(), end(), begin(), [scalar](T value) { return value * scalar; });
      return *this;
    }

    constexpr Vector<T, Size>& operator/=(const T& scalar) {
      std::transform(begin(), end(), begin(), [scalar](T value) { return value / scalar; });
      return *this;
    }


    // Align to 16 for anything greater than Vec2
    // to take advantage of aligned load/store.
    static constexpr size_t Alignment = Size > 2u ? std::max<size_t>(16u, alignof(T)) : alignof(T);


    alignas(Alignment) std::array<T, Size> data;

  };

  template <typename T, size_t Size>
  constexpr Vector<T, Size> operator*(T scalar, const Vector<T, Size>& vector) {
    return util::transform_result<Vector<T, Size>>(vector.begin(), vector.end(), [scalar](T value) { return scalar * value; });
  }

  template <typename T, size_t Size>
  constexpr Vector<T, Size> operator/(T scalar, const Vector<T, Size>& vector) {
    return util::transform_result<Vector<T, Size>>(vector.begin(), vector.end(), [scalar](T value) { return scalar / value; });
  }

  template <typename T, size_t Size>
  constexpr T dot(const Vector<T, Size>& a, const Vector<T, Size>& b) {
    auto multiplied = a * b;
    return std::accumulate(multiplied.begin(), multiplied.end(), T{});
  }

  template <typename T, size_t Size>
  constexpr T lengthSqr(const Vector<T, Size>& a) {
    return dot(a, a);
  }

  template <typename T, size_t Size>
  constexpr float length(const Vector<T, Size>& a) {
    return std::sqrt(float{ lengthSqr(a) });
  }

  template <typename T, size_t Size>
  constexpr Vector<T, Size> normalize(const Vector<T, Size>& a) {
    return a * T{ 1.0f / length(a) };
  }

  template <typename T, size_t Size>
  std::ostream& operator<<(std::ostream& os, const Vector<T, Size>& v) {
    os << "Vector<" << typeid(T).name() << ", " << Size << ">(";
    for (size_t i = 0; i < Size; i++) {
      os << v[i];
      if (i != Size - 1)
        os << ", ";
    }
    return os << ")";
  }

}
