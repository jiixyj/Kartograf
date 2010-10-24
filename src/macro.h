#ifndef _MACRO_H
#define _MACRO_H

#include <limits>
#include <cstdio>

template<typename T, typename U>
inline T safe_cast_uu(U argument) {
  T min = std::numeric_limits<T>::min();
  T max = std::numeric_limits<T>::max();
  if (argument < min || argument > max) {
    fprintf(stderr, "fatal cast!\n");
    exit(1);
  } else {
    return static_cast<T>(argument);
  }
}

template<typename T, typename U>
inline T safe_cast_us(U argument) {
  T max = std::numeric_limits<T>::max();
  if (argument < 0 || static_cast<size_t>(argument) > max) {
    fprintf(stderr, "fatal cast!\n");
    exit(1);
  } else {
    return static_cast<T>(argument);
  }
}

#endif  // _MACRO_H
