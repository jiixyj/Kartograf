template<typename T>
class Image {
 public:
  Image() : rows(), cols(), channels(), data() {}
  Image(uint16_t _rows, uint16_t _cols, uint8_t _channels)
          : rows(_rows), cols(_cols), channels(_channels),
            data(rows * cols * channels, 0) {}
  uint16_t rows;
  uint16_t cols;
  uint8_t channels;
  std::vector<T> data;
  T& at(size_t i, size_t j, size_t c) {
    return data[i * cols * channels + j * channels + c];
  }
  Image<uint8_t> floyd_steinberg() const {
    Image<T> orig = *this;
    Image<uint8_t> ret(rows, cols, channels);
    for (size_t i = 0; i < rows; ++i) {
      for (size_t j = 0; j < cols; ++j) {
        for (size_t c = 0; c < channels; ++c) {
          T oldpixel = orig.at(i, j, c);
          uint8_t newpixel = static_cast<uint8_t>(oldpixel * 255.0 + 0.5);
          ret.at(i, j, c) = newpixel;
          double quant_error = oldpixel - newpixel / 255.0;
          if (j + 1 != cols)
            orig.at(i, j + 1, c)     += 7.0 / 16.0 * quant_error;
          if (j != 0 && i + 1 != rows)
            orig.at(i + 1, j - 1, c) += 3.0 / 16.0 * quant_error;
          if (i + 1 != rows)
            orig.at(i + 1, j, c)     += 5.0 / 16.0 * quant_error;
          if (j + 1 != cols && i + 1 != rows)
            orig.at(i + 1, j + 1, c) += 1.0 / 16.0 * quant_error;
        }
      }
    }
    return ret;
  }

};
