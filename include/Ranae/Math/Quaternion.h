#pragma once

#include <Ranae/Common.h>
#include <Ranae/Math/Vector.h>

namespace ranae {

  template <typename T>
  struct Quaternion : public Vector<T, 4> {

    Quaternion()
      : Vector<T, 4>{ } { }

    Quaternion(T x, T y, T z, T w)
      : Vector<T, 4>{ x, y, z, w } { }

    Quaternion(const Vector<T, 3> v, T s)
      : Vector<T, 4>{ v[0], v[1], v[2], s } { }

    Quaternion(const Quaternion& other) = default;


    constexpr Quaternion<T> operator*(const Quaternion<T>& b) const {
      const Quaternion<T>& a = *this;
      return Quaternion<T> {
        a[3] * b[0] + a[0] * b[3] + a[1] * b[2] - a[2] * b[1],
        a[3] * b[1] - a[0] * b[2] + a[1] * b[3] + a[2] * b[0],
        a[3] * b[2] + a[0] * b[1] - a[1] * b[0] + a[2] * b[3],
        a[3] * b[3] - a[0] * b[0] - a[1] * b[1] - a[2] * b[2]
      };
    }

    constexpr Quaternion<T> operator/(const Quaternion<T>& other) const = delete;


    constexpr Quaternion<T>& operator*=(const Quaternion<T>& other) {
      *this = *this * other;
      return *this;
    }

    constexpr Quaternion<T>& operator/=(const Quaternion<T>& other) = delete;

  };


  template <typename T> 
  constexpr Vector<T, 3> vector(const Quaternion<T>& q) { 
    return Vector<T, 3>{ q[0], q[1], q[2] };
  }

  template <typename T>
  constexpr T scalar(const Quaternion<T>& q) {
    return q[3];
  }


  template <typename T>
  constexpr Quaternion<T> cross(const Quaternion<T>& a, const Quaternion<T>& b) {
    return Quaternion<T> {
      a[3] * b[0] + a[0] * b[3] + a[1] * b[2] - a[2] * b[1],
      a[3] * b[1] + a[1] * b[3] + a[2] * b[0] - a[0] * b[2],
      a[3] * b[2] + a[2] * b[3] + a[0] * b[1] - a[1] * b[0],
      a[3] * b[3] - a[0] * b[0] - a[1] * b[1] - a[2] * b[2]
    };
  }


  template <typename T>
  constexpr Quaternion<T> conjugate(const Quaternion<T>& q) {
    return Quaternion<T> { -vector(q), scalar(q) };
  }

  template <typename T>
  constexpr Quaternion<T> inverse(const Quaternion<T>& q) {
    return Quaternion<T> { conjugate(q) / length(q) };
  }


  template <typename T>
  constexpr Vector<T, 3> operator*(const Quaternion<T>& q, const Vector<T, 3> v) {
    const Vector<T, 3> t = 2.0f * cross(vector(q), v);
    return Vector<T, 3>{ v + scalar(q) * t + cross(vector(q), t) };
  }

}