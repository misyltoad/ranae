#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <cmath>
#include <functional>
#include <numeric>
#include <ostream>
#include <cassert>
#include <memory>

namespace ranae {

  namespace impl {

    template <typename Func>
    class Deferrer {
    public:
      Deferrer(Func func) : m_func(func) {}
      ~Deferrer() { m_func(); }
    private:
      Func m_func;
    };

    template <typename Func>
    Deferrer<Func> DeferFunction(Func f) {
      return Deferrer<Func>(f);
    }

  }

  namespace util {

    template<typename T, typename InputIt, typename UnaryOperation>
    T transform_result(InputIt first, InputIt last, UnaryOperation op) {
      T result;
      std::transform(first, last, result.begin(), op);
      return result;
    }

    template<typename T, typename InputIt1, typename InputIt2, typename BinaryOperation>
    T transform_result(InputIt1 first1, InputIt1 last1, InputIt2 first2, BinaryOperation op) {
      T result;
      std::transform(first1, last1, first2, result.begin(), op);
      return result;
    }

  }

  template <typename T>
  void rnAssert(const T& expr) {
    assert(expr);
  }


  template<typename T>
  constexpr T clamp(T n, T lo, T hi) {
    if (n < lo) return lo;
    if (n > hi) return hi;
    return n;
  }


  template<typename T, typename U = T>
  constexpr T align(T what, U to) {
    return (what + to - 1) & ~(to - 1);
  }

  template <typename V, typename... T>
  constexpr auto array_of(T&&... t)
      -> std::array < V, sizeof...(T) >
  {
      return {{ std::forward<T>(t)... }};
  }

}

#define defer_1(x, y) x##y
#define defer_2(x, y) defer_1(x, y)
#define defer_3(x)    defer_2(x, __COUNTER__)
#define defer(code)   auto defer_3(_defer_) = ::ranae::impl::DeferFunction([&](){code;});
