#include <Ranae/Math/Vector.h>
#include <iostream>

using namespace ranae;

int main() {
  Vector<int32_t, 4> a = { 0, 1, 2, 3 };
  Vector<int32_t, 4> b = { 1, 2, 3, 4 };

  rnAssert(dot(a, b) == 20);
  rnAssert(a + b == Vector<int32_t, 4>{1, 3, 5, 7});

  Vector<float, 2> c = { 1.0f, 2.0f };
  rnAssert(1.0f / c == Vector<float, 2>{1.0f, 0.5f});

  for (auto& x : a)
    x += 1;
  rnAssert(a == b);

  std::cout << a << std::endl;
  std::cout << c << std::endl;

  auto d = Vector<float, 2>{ 1.0f };

  float scalar = { 2.0 };
  std::cout << d * scalar << std::endl;

  d *= 3.0;
  rnAssert(d == Vector<float, 2>(3.0f, 3.0f));
  d /= 3.0;
  rnAssert(d == Vector<float, 2>(1.0f, 1.0f));

  return 0;
}