#include <cmath>
#include <cstdio>
#include <vector>

int main() {
  // double sqrt2 = sqrt(2.0);
  const int max_y = 127; // 127
  const int half_x = 32;
  std::vector<std::vector<int> > mask(288, std::vector<int>(64, -1));
  std::vector<std::vector<std::vector<int > > > to3D(288, std::vector<std::vector<int> >(64, std::vector<int>(3,-1)));
  for (int y = 0; y < 128; ++y) {
    for (int z = 15; z >= 0; --z) {
      for (int x = 15; x >= 0; --x) {
        double dz;
        int px = 15-x;
        int pz = 15-z;
        int cx, cy;
        cx = 2 * px - 2 * pz;
        cy = px + 2 * (max_y-y) + pz;
        // cz = sqrt2 * x - sqrt2 * (max_y-y) + sqrt2 * z;
        cy += 2;
        cx += half_x;
        // fprintf(stdout, "%d %d %d: %d %d\n", z, x, y, cy, cx);
        int index = 0;
        for (int i = -2; i < 2; ++i) {
          for (int j = -2; j < 2; ++j) {
            mask[cy+i][cx+j] = index++;
            to3D[cy+i][cx+j][0] = x;
            to3D[cy+i][cx+j][1] = y;
            to3D[cy+i][cx+j][2] = z;
            if (index == 1) {
              mask[cy+i][cx+j] = 6;
              to3D[cy+i][cx+j][0] = x+1;
              to3D[cy+i][cx+j][1] = y;
              to3D[cy+i][cx+j][2] = z;
            } else if (index == 4) {
              mask[cy+i][cx+j] = 5;
              to3D[cy+i][cx+j][0] = x;
              to3D[cy+i][cx+j][1] = y;
              to3D[cy+i][cx+j][2] = z+1;
            } else if (index == 13) {
              mask[cy+i][cx+j] = 4;
              to3D[cy+i][cx+j][0] = x;
              to3D[cy+i][cx+j][1] = y-1;
              to3D[cy+i][cx+j][2] = z;
            } else if (index == 16) {
              mask[cy+i][cx+j] = 7;
              to3D[cy+i][cx+j][0] = x;
              to3D[cy+i][cx+j][1] = y-1;
              to3D[cy+i][cx+j][2] = z;
            }
          }
        }
      }
    }
  }
  for (int i = 0; i < 288; ++i) {
    for (int j = 0; j < 64; ++j) {
      // printf("%d %d: %d %d %d\n", i, j, to3D[i][j][0], to3D[i][j][1], to3D[i][j][2]);
      if (mask[i][j] == -1) {
        printf("-");
      } else {
        printf("%x", mask[i][j]);
      }
    }
    printf("\n");
  }
  return 0;
}
