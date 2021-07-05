#include <Ranae/Math/Vector.h>
#include <iostream>

using namespace ranae;

int main() {
  Vector<int32_t, 4> a = { 0, 1, 2, 3 };
  Vector<int32_t, 4> b = { 1, 2, 3, 4 };

  Assert(dot(a, b) == 20);
  Assert(a + b == Vector<int32_t, 4>{1, 3, 5, 7});

  Vector<float, 2> c = { 1.0f, 2.0f };
  Assert(1.0f / c == Vector<float, 2>{1.0f, 0.5f});

  for (auto& x : a)
    x += 1;
  Assert(a == b);

  std::cout << a << std::endl;
  std::cout << c << std::endl;

  return 0;
}