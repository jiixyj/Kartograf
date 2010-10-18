#include "./colors.h"

QColor blend(const QColor& B, const QColor& A) {
  QColor C;
  C.setRedF(B.redF() + (1 - B.alphaF()) * A.redF());
  C.setGreenF(B.greenF() + (1 - B.alphaF()) * A.greenF());
  C.setBlueF(B.blueF() + (1 - B.alphaF()) * A.blueF());
  C.setAlphaF(B.alphaF() + (1 - B.alphaF()) * A.alphaF());
  return C;
}
