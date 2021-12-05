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
    return J(1) / J(a);
    //return J{ 1 } / J{ a };
  }

  // Fucking ostream shit decides it wants to print as actual characters
  // not numbers and there is no real workaround. What absolute fucking garbage.
  template <typename T>
  struct OstreamNumericTypeResolver {
    using NumericPrintType = T;
  };

  template <>
  struct OstreamNumericTypeResolver<uint8_t> {
    using NumericPrintType = uint32_t;
  };

  template <>
  struct OstreamNumericTypeResolver<int8_t> {
    using NumericPrintType = int32_t;
  };
    
}