#pragma once

namespace ranae {

  template <typename T>
  struct DefaultLengthTypeResolver {
    using LengthType = float;
  };

  template <>
  struct DefaultLengthTypeResolver<double> {
    using LengthType = double;
  };

  template <>
  struct DefaultLengthTypeResolver<long double> {
    using LengthType = long double;
  };

  template <typename T, typename J = DefaultLengthTypeResolver<T>::LengthType>
  constexpr J rcp(const T& a) {
    // GCC gets angy at me for narrowing otherwise which is stupid.
    // Need to find out why.
    return static_cast<J>(1) / static_cast<J>(a);
    //return J{ 1 } / J{ a };
  }
    
}