#ifndef _IMAGE_H
#define _IMAGE_H

#include <boost/cstdint.hpp>
#include <vector>

template<typename T>
class Image {
 public:
  Image() : rows(), cols(), channels(), data() {}
  Image(uint16_t _rows, uint16_t _cols, uint8_t _channels)
          : rows(_rows), cols(_cols), channels(_channels),
            data(1u * rows * cols * channels) {}
  uint16_t rows;
  uint16_t cols;
  uint8_t channels;
  std::vector<T> data;
  T& at(size_t i, size_t j, size_t c) {
    return data[i * cols * channels + j * channels + c];
  }
  Image<uint8_t> floyd_steinberg() const;
  Image<uint8_t> floyd_steinberg_zigzag() const;
};

#endif  // _IMAGE_H
