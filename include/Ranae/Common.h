#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <cmath>
#include <functional>
#include <numeric>
#include <ostream>
#include <cassert>

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
  void Assert(const T& expr) {
    assert(expr);
  }

}

#define defer_1(x, y) x##y
#define defer_2(x, y) defer_1(x, y)
#define defer_3(x)    defer_2(x, __COUNTER__)
#define defer(code)   auto defer_3(_defer_) = ::ranae::impl::DeferFunction([&](){code;})
