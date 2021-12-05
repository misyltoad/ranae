#include <Ranae/Math/Vector.h>
#include <iostream>

using namespace ranae;

template <typename T>
void test_generic_type() {
  const T values[4] = { T{ 0 }, T{ 0 }, T{ 0 }, T{ 0 } };
  const Vector<T, 4> a{ T{ 0 }, T{ 1 }, T{ 2 }, T{ 3 } };
  const Vector<T, 4> b{ T{ 1 } };
  const Vector<T, 4> c{ values };
  const Vector<T, 4> d{ T{ 0 }, T{ 4 }, T{ 8 }, T{ 12 } };

  // Test indexing
  rnAssert(a[0] == T{ 0 });
  rnAssert(a[1] == T{ 1 });
  rnAssert(a[2] == T{ 2 });
  rnAssert(a[3] == T{ 3 });

  // Test result operators.
  rnAssert(a + b == Vector<T, 4>{ T{ 1 }, T{ 2 }, T{ 3 }, T{ 4 } });
  rnAssert(a - b == Vector<T, 4>{ T{ -1 }, T{ 0 }, T{ 1 }, T{ 2 } });
  rnAssert(a * b == a);
  rnAssert(a / b == a);
  rnAssert(-a == Vector<T, 4>{ T{ 0 }, T{ -1 }, T{ -2 }, T{ -3 } });
  rnAssert(-a != Vector<T, 4>{ T{ 1 }, T{ 0 }, T{ 5 }, T{ 1 } });
  rnAssert(a * c == c);
  rnAssert(a * T{ 4 } == Vector<T, 4>{ T{ 0 }, T{ 4 }, T{ 8 }, T{ 12 } });
  rnAssert(T{ 4 } * a == d);
  rnAssert(d / T{ 4 } == a);

  // Test dot/accumulate
  rnAssert(accumulate(a) == T{ 6 });
  rnAssert(dot(a, d) == T{ 56 });

  // Test length squared
  rnAssert(lengthSqr(a) == T{ 14 });
  rnAssert(lengthSqr(b) == T{ 4 });
  rnAssert(lengthSqr(c) == T{ 0 });
  rnAssert(dot(a, a) == lengthSqr(a));

  // Test rcp
  {
    using J = Vector<T, 4>::DefaultLengthType;
    rnAssert(rcp(b) == Vector<J, 4>{ J{ 1 } });
  }

  // Test identity, empty
  rnAssert(identity(b));
  rnAssert(empty(c));

  // Test printing
  std::cout << "a: " << a << std::endl;
  std::cout << "b: " << b << std::endl;
  std::cout << "c: " << c << std::endl;
  std::cout << "d: " << d << std::endl;
}

template <typename T> 
void test_integer_type() {
  test_generic_type<T>();

  const Vector<T, 4> a{ T{ 0 }, T{ 2 }, T{ 4 }, T{ 6 } };
  const Vector<T, 4> b{ T{ 4 }, T{ 4 }, T{ 4 }, T{ 4 } };
  const Vector<T, 4> c{ T{ 1 }, T{ 2 }, T{ 5 }, T{ 6 } };

  // Test modulo
  rnAssert(a % b == Vector<T, 4>{ T{ 0 }, T{ 2 }, T{ 0 }, T{ 2 } });
  rnAssert(T{ 4 } % c == Vector<T, 4>{ T{ 0 }, T{ 0 }, T{ 4 }, T{ 4 } });
}

template <typename T>
void test_float_type() {
  test_generic_type<T>();

  const Vector<T, 4> a{ T{ 2 }, T{ 2 }, T{ 2 }, T{ 2 } };
  const Vector<T, 4> b{ T{ 1 } };
  const Vector<T, 4> c{ T{ 6 }, T{ 0 }, T{ 0 }, T{ 0 } };

  // Test length
  {
    using J = Vector<T, 4>::DefaultLengthType;
    rnAssert(length(b) == J{ 2 });
    rnAssert(length(c) == J{ 6 });
  }

  // Test normalize
  {
    using J = Vector<T, 4>::DefaultLengthType;
    rnAssert(normalize(a) == Vector<J, 4>{ J{ 0.5 } });
    rnAssert(normalize(b) == Vector<J, 4>{ J{ 0.5 } });
    rnAssert(normalize(c) == Vector<J, 4>{ J{ 1 }, J{ 0 }, J{ 0 }, J{ 0 } });
  }

  // Test rcp
  {
    using J = Vector<T, 4>::DefaultLengthType;
    rnAssert(rcp(a) == Vector<J, 4>{ J{ 0.5 } });
  }
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
