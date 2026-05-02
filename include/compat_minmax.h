#pragma once

#ifdef __cplusplus
  #include <algorithm>
  #include <type_traits>

  // Si vinieran como macros desde Arduino, las quitamos SOLO en C++
  #ifdef min
    #undef min
  #endif
  #ifdef max
    #undef max
  #endif

  template <typename A, typename B>
  constexpr auto min(A a, B b) -> typename std::common_type<A,B>::type {
    using C = typename std::common_type<A,B>::type;
    return std::min(static_cast<C>(a), static_cast<C>(b));
  }

  template <typename A, typename B>
  constexpr auto max(A a, B b) -> typename std::common_type<A,B>::type {
    using C = typename std::common_type<A,B>::type;
    return std::max(static_cast<C>(a), static_cast<C>(b));
  }
#endif  // __cplusplus

template <typename A, typename B>
constexpr auto max(A a, B b) -> typename std::common_type<A,B>::type {
  using C = typename std::common_type<A,B>::type;
  return std::max(static_cast<C>(a), static_cast<C>(b));
}
