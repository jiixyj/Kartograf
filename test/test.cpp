#include <cmath>
#include <cstdio>

int main() {
  // double sqrt2 = sqrt(2.0);
  const int max_y = 127; // 127
  const int half_x = 32;
  for (int y = 0; y < 128; ++y) {
    for (int z = 0; z < 16; ++z) {
      for (int x = 0; x < 16; ++x) {
        double dz;
        int cx, cy;
        cx = 2 * x - 2 * z;
        cy = x + 2 * (max_y-y) + z;
        // cz = sqrt2 * x - sqrt2 * (max_y-y) + sqrt2 * z;
        cy += 2;
        cx += half_x;
        fprintf(stderr, "%d %d %d: %d %d\n", z, x, y, cy, cx);
      }
    }
  }
  return 0;
}
