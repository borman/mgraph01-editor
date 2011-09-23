#include "transform.h"
#include "rgbv.h"

#ifndef M_PI
#define M_PI 3.1415926535897932385
#endif

static QRgb getPixelEx(const QImage &img, int x, int y)
{
  if (x<0 || y<0 || x>=img.width() || y>=img.height())
    return qRgb(0, 0, 0);
  return img.pixel(x, y);
}

static QRgb interpolate(const QImage &img, double x, double y, Interpolation method = Bilinear)
{
  switch (method)
  {
  case NearestNeighbor:
    return getPixelEx(img, x, y);
  
  case Bilinear:
    {
      /* Pixels:
       *
       * C1 | C2
       * -------
       * C3 | C4
       */
      RGBV c1(getPixelEx(img, floor(x), floor(y)));
      RGBV c2(getPixelEx(img, ceil(x), floor(y)));
      RGBV c3(getPixelEx(img, floor(x), ceil(y)));
      RGBV c4(getPixelEx(img, ceil(x), ceil(y)));
      double h = x-floor(x);
      double v = y-floor(y);
      RGBV res;
      res.addk(c1, (1-h)*(1-v));
      res.addk(c2, h*(1-v));
      res.addk(c3, (1-h)*v);
      res.addk(c4, h*v);
      return res.toQRgb();
    }
  }
  return qRgb(0, 0, 0);
}

QImage transform(const QImage &img, const Transform &transform, Interpolation ipol)
{
  QImage res(img.size(), img.format());
  for (int y=0; y<img.height(); y++)
    for (int x=0; x<img.width(); x++)
    {
      double px, py;
      transform(x, y, px, py);
      res.setPixel(x, y, interpolate(img, px, py, ipol));
    }
  return res;
}

QImage scale(const QImage &img, double factor, Interpolation ipol)
{
  double cx = img.width()/2.0;
  double cy = img.height()/2.0;
  Transform tr = Transform::shift(cx, cy) 
               * Transform::scale(1/factor, 1/factor) 
               * Transform::shift(-cx, -cy);
  return transform(img, tr, ipol);
}

QImage rotate(const QImage &img, double degree, Interpolation ipol)
{
  double cx = img.width()/2.0;
  double cy = img.height()/2.0;
  Transform tr = Transform::shift(cx, cy) 
               * Transform::rotate(degree * M_PI/180) 
               * Transform::shift(-cx, -cy);
  return transform(img, tr, ipol);
}

