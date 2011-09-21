#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <cmath>
#include <QImage>

/** Transform matrix:
 * / a1 b1 c1 \   / x \   / x'\
 * | a2 b2 c1 | x | y | = | y'|
 * \  0  0  1 /   \ 1 /   \ 1 /
 */
class Transform
{
  public:
    Transform(double _a1, double _b1, double _c1,
              double _a2, double _b2, double _c2)
      : a1(_a1), b1(_b1), c1(_c1), a2(_a2), b2(_b2), c2(_c2) {}

  Transform operator*(const Transform &t) const
  {
    return Transform(
        a1*t.a1 + b1*t.a2,  a1*t.b1 + b1*t.b2, a1*t.c1 + b1*t.c2 + c1,
        a2*t.a1 + b2*t.a2,  a2*t.b1 + b2*t.b2, a2*t.c1 + b2*t.c2 + c2);
  }
  
  static Transform shift(double dx, double dy)
  {
    return Transform(
        1, 0, dx,
        0, 1, dy);
  }

  static Transform scale(double kx, double ky)
  {
    return Transform(
        kx, 0, 0,
        0, ky, 0);
  }
  
  static Transform rotate(double rad)
  {
    return Transform(
        cos(rad), -sin(rad), 0,
        sin(rad),  cos(rad), 0);
  }

  void operator()(double x, double y, double &xp, double &yp) const
  {
    xp = a1*x + b1*y + c1;
    yp = a2*x + b2*y + c2;
  }

  private:
    double a1, b1, c1;
    double a2, b2, c2;
};

enum Interpolation
{
  NearestNeighbor,
  Bilinear
};

QImage transform(const QImage &img, const Transform &transform, Interpolation ipol = Bilinear);
QImage scale(const QImage &img, double factor, Interpolation ipol = Bilinear);
QImage rotate(const QImage &img, double degree, Interpolation ipol = Bilinear);

#endif // TRANSFORM_H
