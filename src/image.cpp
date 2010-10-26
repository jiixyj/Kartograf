#include "./image.h"

#include "./colors.h"

template<>
Image<uint8_t> Image<Color>::floyd_steinberg() const {
  Image<Color> orig = *this;
  Image<uint8_t> ret(rows, cols, 4);
  for (size_t i = 0; i < rows; ++i) {
    for (size_t j = 0; j < cols; ++j) {
      Color oldpixel = orig.at(i, j, 0);
      for (size_t c = 0; c < 4; ++c) {
        uint8_t newpixel = static_cast<uint8_t>(oldpixel.c[c] * 255.0 + 0.5);
        ret.at(i, j, c) = newpixel;
        double quant_error = oldpixel.c[c] - newpixel / 255.0;
        if (j + 1 != cols)
          orig.at(i, j + 1, 0).c[c]     += 7.0 / 16.0 * quant_error;
        if (j != 0 && i + 1 != rows)
          orig.at(i + 1, j - 1, 0).c[c] += 3.0 / 16.0 * quant_error;
        if (i + 1 != rows)
          orig.at(i + 1, j, 0).c[c]     += 5.0 / 16.0 * quant_error;
        if (j + 1 != cols && i + 1 != rows)
          orig.at(i + 1, j + 1, 0).c[c] += 1.0 / 16.0 * quant_error;
      }
    }
  }
  return ret;
}

template<>
Image<uint8_t> Image<Color>::floyd_steinberg_zigzag() const {
  Image<Color> orig = *this;
  Image<uint8_t> ret(rows, cols, 4);
  for (size_t i = 0; i < rows; ++i) {
    size_t j = 0;
    bool one = false;
    if (i % 2) j = cols - 1u;
    for (;;) {
      Color oldpixel = orig.at(i, j, 0);
      for (size_t c = 0; c < 4; ++c) {
        uint8_t newpixel = static_cast<uint8_t>(oldpixel.c[c] * 255.0 + 0.5);
        ret.at(i, j, c) = newpixel;
        double quant_error = oldpixel.c[c] - newpixel / 255.0;
        if (j + 1 != cols)
          orig.at(i, j + 1, 0).c[c]     += 7.0 / 16.0 * quant_error;
        if (j != 0 && i + 1 != rows)
          orig.at(i + 1, j - 1, 0).c[c] += 3.0 / 16.0 * quant_error;
        if (i + 1 != rows)
          orig.at(i + 1, j, 0).c[c]     += 5.0 / 16.0 * quant_error;
        if (j + 1 != cols && i + 1 != rows)
          orig.at(i + 1, j + 1, 0).c[c] += 1.0 / 16.0 * quant_error;
      }
      if ((j == 0 || j + 1 == cols) && one) break;
      if (i % 2) --j; else ++j;
      one = true;
    }
  }
  return ret;
}
