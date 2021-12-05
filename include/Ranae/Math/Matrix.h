#pragma once

#include <Ranae/Common.h>
#include <Ranae/Math/Vector.h>

namespace ranae {

  template <typename T, size_t Rows, size_t Columns>
  struct Matrix {
    using RowVector = Vector<T, Columns>;

    constexpr Matrix(T scale = T{ 1 }) {
      for (size_t i = 0; i < Rows; i++) {
        RowVector vector{};
        vector[i] = scale;
        data[i] = vector;
      }
    }

    template <typename... Args>
    constexpr Matrix(const Args&... args)
      : data {{ args... }} {
      static_assert(sizeof...(Args) == Rows);
    }

    constexpr Matrix(const T components[Columns][Rows]) {
      std::copy(&components[0], &components[Columns][Rows], data.begin());
    }

    constexpr Matrix(const Matrix<T, Rows, Columns>& other) = default;


    constexpr       RowVector& operator[](size_t index)       { return data[index]; }
    constexpr const RowVector& operator[](size_t index) const { return data[index]; }


    constexpr const RowVector* begin() const { return data.begin(); }
    constexpr       RowVector* begin()       { return data.begin(); }
    constexpr const RowVector* end()   const { return data.end();   }
    constexpr       RowVector* end()         { return data.end();   }


    constexpr bool operator==(const Matrix<T, Rows, Columns>& other) const {
      return std::equal(begin(), end(), other.begin());
    }

    constexpr bool operator!=(const Matrix<T, Rows, Columns>& other) const {
      return !operator==(other);
    }


    template <typename UnaryOperation>
    constexpr Matrix<T, Rows, Columns> transform_result(UnaryOperation op) const {
      return util::transform_result<Matrix<T, Rows, Columns>>(begin(), end(), op);
    }
    
    template <typename BinaryOperation>
    constexpr Matrix<T, Rows, Columns> transform_result(const RowVector *other, BinaryOperation op) const {
      return util::transform_result<Matrix<T, Rows, Columns>>(begin(), end(), other, op);
    }

    template <typename BinaryOperation>
    constexpr Matrix<T, Rows, Columns> transform_result(const Matrix<T, Rows, Columns>& other, BinaryOperation op) const {
      return transform_result(other.begin(), op);
    }

    template <typename UnaryOperation>
    constexpr Matrix<T, Rows, Columns>& transform_in_place(UnaryOperation op) {
      std::transform(begin(), end(), begin(), op);
      return *this;
    }
    
    template <typename BinaryOperation>
    constexpr Matrix<T, Rows, Columns>& transform_in_place(const RowVector *other, BinaryOperation op) {
      std::transform(begin(), end(), other, begin(), op);
      return *this;
    }

    template <typename BinaryOperation>
    constexpr Matrix<T, Rows, Columns>& transform_in_place(const Matrix<T, Rows, Columns>& other, BinaryOperation op) {
      return transform_in_place(other.begin(), op);
    }

    // Simple math operations

    constexpr Matrix<T, Rows, Columns> operator+(const Matrix<T, Rows, Columns>& other) const {
      return transform_result(other, std::plus());
    }

    constexpr Matrix<T, Rows, Columns> operator-(const Matrix<T, Rows, Columns>& other) const {
      return transform_result(other, std::minus());
    }

    constexpr Matrix<T, Rows, Columns> operator*(const T& scalar) const {
      return transform_result([scalar](const RowVector& value) { return value * scalar; });
    }

    constexpr Matrix<T, Rows, Columns> operator/(const T& scalar) const {
      return transform_result([scalar](const RowVector&  value) { return value / scalar; });
    }

    constexpr Matrix<T, Rows, Columns> operator%(const T& scalar) const {
      return transform_result([scalar](const RowVector&  value) { return value % scalar; });
    }


    constexpr Matrix<T, Rows, Columns>& operator+=(const Matrix<T, Rows, Columns>& other) {
      return transform_in_place(std::plus());
    }

    constexpr Matrix<T, Rows, Columns>& operator-=(const Matrix<T, Rows, Columns>& other) {
      return transform_in_place(std::minus());
    }

    constexpr Matrix<T, Rows, Columns>& operator*=(const T& scalar) {
      return transform_in_place([scalar](const RowVector&  value) { return value * scalar; });
    }

    constexpr Matrix<T, Rows, Columns>& operator/=(const T& scalar) {
      return transform_in_place([scalar](const RowVector&  value) { return value / scalar; });
    }

    constexpr Matrix<T, Rows, Columns>& operator%=(const T& scalar) {
      return transform_in_place([scalar](const RowVector&  value) { return value % scalar; });
    }

    // Real matrix operations

    constexpr RowVector operator*(const RowVector& v) const {
      auto mul = transform_result(v.begin(), std::multiplies());
      return std::accumulate(mul.begin(), mul.end(), RowVector{});
    }

    std::array<RowVector, Rows> data;
  };

  template <typename T, size_t Rows, size_t Columns>
  constexpr Matrix<T, Rows, Columns> operator*(T scalar, const Matrix<T, Rows, Columns>& matrix) {
    using J = Matrix<T, Rows, Columns>::RowVector;
    return matrix.transform_result([scalar](J value) { return scalar * value; });
  }

  template <typename T, size_t Rows, size_t Columns>
  constexpr Matrix<T, Rows, Columns> operator/(T scalar, const Matrix<T, Rows, Columns>& matrix) {
    using J = Matrix<T, Rows, Columns>::RowVector;
    return matrix.transform_result([scalar](J value) { return scalar / value; });
  }

  template <typename T, size_t Rows, size_t Columns>
  constexpr Matrix<T, Rows, Columns> operator%(T scalar, const Matrix<T, Rows, Columns>& matrix) {
    using J = Matrix<T, Rows, Columns>::RowVector;
    return matrix.transform_result([scalar](J value) { return scalar % value; });
  }


  template <typename T, size_t Rows, size_t Columns>
  constexpr Matrix<T, Rows - 1, Columns - 1> minor(const Matrix<T, Rows, Columns>& a, size_t column, size_t row) {
    Matrix<T, Rows - 1, Columns - 1> mtx;
    for (size_t y = 0, my = 0; y < Rows; y++) {
      if (y == row) continue;
      for (size_t x = 0, mx = 0; x < Columns; x++) {
        if (x == column) continue;
        mtx[my][mx] = a[y][x];
        mx++;
      }
      my++;
    }
    return mtx;
  }

  template <typename T, size_t Rows, size_t Columns>
  constexpr T determinant(const Matrix<T, Rows, Columns>& a) {
    static_assert(Rows == Columns);

    if constexpr (Rows == 1) {
      return a[0][0];
    } else {
      T result = T{};
      for (size_t x = 0; x < Columns; x++) {
        const T sign = (x % 2) ? T{ -1 } : T{ 1 };
        result += sign * a[0][x] * determinant(minor(a, x, 0));
      }
      return result;
    }
  }

  template <typename T, size_t Rows, size_t Columns>
  constexpr Matrix<T, Columns, Rows> transpose(const Matrix<T, Rows, Columns>& a) {
    Matrix<T, Columns, Rows> result;
    for (size_t y = 0; y < Rows; y++) {
      for (size_t x = 0; x < Columns; x++)
        result[x][y] = a[y][x];
    }
    return result;
  }

  template <typename T, size_t Rows, size_t Columns>
  constexpr Matrix<T, Rows, Columns> hadamard_product(const Matrix<T, Rows, Columns>& x, const Matrix<T, Rows, Columns>& y) {
    return x.transform_result(y, std::multiplies());
  }


  template <typename T, size_t Rows, size_t Columns>
  std::ostream& operator<<(std::ostream& os, const Matrix<T, Rows, Columns>& m) {
    os << "Matrix<" << typeid(T).name() << ", " << Rows << ", " << Columns << ">(\n";
    for (size_t i = 0; i < Rows; i++) {
      os << "  " << m[i];
      if (i != Rows - 1)
        os << "\n";
    }
    return os << ")";
  }

}