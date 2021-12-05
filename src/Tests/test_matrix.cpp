#include <Ranae/Math/Matrix.h>
#include <iostream>

using namespace ranae;

template <typename T>
void test_generic_type() {
  const Matrix<T, 4, 4> identity{};
  const Matrix<T, 4, 4> scale{ T{ 2 } };
  const Matrix<T, 4, 4> a{ T{ 3 } };
  const Matrix<T, 4, 4> b{ T{ 0 } };
  const Matrix<T, 4, 4> c = Matrix<T, 4, 4>{
    Vector<T, 4>{ T{ 1 }, T{ 2 }, T{ 3 }, T{ 4 }, },
    Vector<T, 4>{ T{ 4 }, T{ 3 }, T{ 2 }, T{ 1 }, },
    Vector<T, 4>{ T{ 1 }, T{ 2 }, T{ 3 }, T{ 4 }, },
    Vector<T, 4>{ T{ 4 }, T{ 3 }, T{ 2 }, T{ 1 }, },
  };
  const Matrix<T, 3, 3> d = Matrix<T, 3, 3>{
    Vector<T, 3>{ T{ 3 }, T{ 2 }, T{ 1 }, },
    Vector<T, 3>{ T{ 2 }, T{ 3 }, T{ 4 }, },
    Vector<T, 3>{ T{ 3 }, T{ 2 }, T{ 1 }, },
  };
  const Matrix<T, 4, 4> e = Matrix<T, 4, 4>{
    Vector<T, 4>{ T{ 1 }, T{ 4 }, T{ 1 }, T{ 4 }, },
    Vector<T, 4>{ T{ 2 }, T{ 3 }, T{ 2 }, T{ 3 }, },
    Vector<T, 4>{ T{ 3 }, T{ 2 }, T{ 3 }, T{ 2 }, },
    Vector<T, 4>{ T{ 4 }, T{ 1 }, T{ 4 }, T{ 1 }, },
  };

  // Test indexing/init
  rnAssert(identity[0][0] == T{ 1 });
  rnAssert(identity[0][1] == T{ 0 });
  rnAssert(identity[1][1] == T{ 1 });
  rnAssert(scale[0][0] == T{ 2 });
  rnAssert(scale[0][1] == T{ 0 });
  rnAssert(scale[1][1] == T{ 2 });

  // Test basic ops.
  rnAssert(scale == identity * T{ 2 });
  rnAssert(scale + identity == a);
  rnAssert(scale - identity == identity);
  rnAssert(identity * T{ 3 } == a);
  rnAssert(identity * T{ 0 } == b);
  rnAssert(identity - identity == b);
  rnAssert(scale / T{ 2 } == identity);
  
  // Test some matrix ops
  rnAssert(minor(identity, 0, 0) == Matrix<T, 3, 3>{});
  rnAssert(minor(c, 0, 0) == d);
  rnAssert(hadamard_product(identity, scale) == scale);
  rnAssert(transpose(identity) == identity);
  rnAssert(transpose(c) == e);

  // Test determinant
  rnAssert(determinant(Matrix<T, 1, 1>{}) == T{ 1 });
  rnAssert(determinant(Matrix<T, 2, 2>{}) == T{ 1 });
  rnAssert(determinant(identity) == T{ 1 });
  rnAssert(determinant(Matrix<T, 2, 2>{ T{ 2 } }) == T{ 4 });
  rnAssert(determinant(scale) == T{ 16 });
  rnAssert(determinant(c) == T{ 0 });

  // Test printing
  std::cout << identity << std::endl;
  std::cout << e << std::endl;
}

template <typename T> 
void test_integer_type() {
  test_generic_type<T>();

  // Test integer ops
  const Matrix<T, 4, 4> identity{};
  const Matrix<T, 4, 4> a{ T{ 3 } };
  rnAssert(a % 2 == identity);
}

template <typename T>
void test_float_type() {
  test_generic_type<T>();
}

void run_tests() {
  test_float_type<float>();
  test_float_type<double>(); 
  test_float_type<long double>();

  test_integer_type<int8_t>();
  test_integer_type<int16_t>();
  test_integer_type<int32_t>();
  test_integer_type<int64_t>();
}

int main() {
  run_tests();

  std::cout << "Tests passed!" << std::endl;

  return 0;
}
