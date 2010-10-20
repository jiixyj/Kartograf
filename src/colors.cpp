#include "./colors.h"

Color blend(const Color& B, const Color& A) {
  Color C;
  C.setRedF(B.redF() + (1.0 - B.alphaF()) * A.redF());
  C.setGreenF(B.greenF() + (1.0 - B.alphaF()) * A.greenF());
  C.setBlueF(B.blueF() + (1.0 - B.alphaF()) * A.blueF());
  C.setAlphaF(B.alphaF() + (1.0 - B.alphaF()) * A.alphaF());
  return C;
}
