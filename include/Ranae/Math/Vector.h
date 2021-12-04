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
  

    template <typename UnaryOperation>
    constexpr Vector<T, Size> transform_result(UnaryOperation op) const {
      return util::transform_result<Vector<T, Size>>(begin(), end(), op);
    }
    
    template <typename BinaryOperation>
    constexpr Vector<T, Size> transform_result(const T *other, BinaryOperation op) const {
      return util::transform_result<Vector<T, Size>>(begin(), end(), other, op);
    }

    template <typename BinaryOperation>
    constexpr Vector<T, Size> transform_result(const Vector<T, Size>& other, BinaryOperation op) const {
      return transform_result(other.begin(), op);
    }

    template <typename UnaryOperation>
    constexpr Vector<T, Size>& transform_in_place(UnaryOperation op) {
      std::transform(begin(), end(), begin(), op);
      return *this;
    }
    
    template <typename BinaryOperation>
    constexpr Vector<T, Size>& transform_in_place(const T *other, BinaryOperation op) {
      std::transform(begin(), end(), other, begin(), op);
      return *this;
    }

    template <typename BinaryOperation>
    constexpr Vector<T, Size>& transform_in_place(const Vector<T, Size>& other, BinaryOperation op) {
      return transform_in_place(other.begin(), op);
    }


    constexpr Vector<T, Size> operator-() const {
      return transform_result(std::negate());
    }
  

    constexpr Vector<T, Size> operator+(const Vector<T, Size>& other) const {
      return transform_result(other, std::plus());
    }

    constexpr Vector<T, Size> operator-(const Vector<T, Size>& other) const {
      return transform_result(other, std::minus());
    }

    constexpr Vector<T, Size> operator*(const Vector<T, Size>& other) const {
      return transform_result(other, std::multiplies());
    }

    constexpr Vector<T, Size> operator*(const T& scalar) const {
      return transform_result([scalar](T value) { return value * scalar; });
    }

    constexpr Vector<T, Size> operator/(const Vector<T, Size>& other) const {
      return transform_result(other, std::divides());
    }

    constexpr Vector<T, Size> operator/(const T& scalar) const {
      return transform_result([scalar](T value) { return value / scalar; });
    }


    constexpr Vector<T, Size>& operator+=(const Vector<T, Size>& other) {
      return transform_in_place(std::plus());
    }

    constexpr Vector<T, Size>& operator-=(const Vector<T, Size>& other) {
      return transform_in_place(std::minus());
    }

    constexpr Vector<T, Size>& operator*=(const Vector<T, Size>& other) {
      return transform_in_place(std::multiplies());
    }

    constexpr Vector<T, Size>& operator*=(const T& scalar) {
      return transform_in_place([scalar](T value) { return value * scalar; });
    }

    constexpr Vector<T, Size>& operator/=(const Vector<T, Size>& other) {
      return transform_in_place(std::divides());
    }

    constexpr Vector<T, Size>& operator/=(const T& scalar) {
      return transform_in_place([scalar](T value) { return value / scalar; });
    }


    // Align to 16 if the size is a multiple of 4 and T is 4 bytes in size.
    // to take advantage of aligned load/stores.
    static constexpr size_t Alignment = (Size % 4u == 0 && sizeof(T) == sizeof(int32_t)) ? std::max<size_t>(16u, alignof(T)) : alignof(T);

    alignas(Alignment) std::array<T, Size> data;

  };

  template <typename T, size_t Size>
  constexpr Vector<T, Size> operator*(T scalar, const Vector<T, Size>& vector) {
    return vector.transform_result([scalar](T value) { return scalar * value; });
  }

  template <typename T, size_t Size>
  constexpr Vector<T, Size> operator/(T scalar, const Vector<T, Size>& vector) {
    return vector.transform_result([scalar](T value) { return scalar / value; });
  }

  template <typename T, size_t Size>
  constexpr T accumulate(const Vector<T, Size>& a, T init = T{}) {
    return std::accumulate(a.begin(), a.end(), init);
  }

  template <typename T, size_t Size>
  constexpr T dot(const Vector<T, Size>& a, const Vector<T, Size>& b) {
    return accumulate(a * b);
  }

  template <typename T, size_t Size>
  constexpr T lengthSqr(const Vector<T, Size>& a) {
    return dot(a, a);
  }

  template <typename J, typename T, size_t Size>
  constexpr J length(const Vector<T, Size>& a) {
    return std::sqrt(J{ lengthSqr(a) });
  }

  template <typename T, size_t Size>
  constexpr Vector<T, Size> normalize(const Vector<T, Size>& a) {
    return a * (T{ 1 } / T{ length<T>(a) });
  }

  template <typename T, size_t Size>
  constexpr Vector<T, Size> rcp(const Vector<T, Size>& a) {
    return a.transform_result([](T value) { return T{ 1 } / value; });
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
