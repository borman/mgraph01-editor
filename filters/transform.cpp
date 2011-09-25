#include <QPainter>
#include "transform.h"
#include "rgbv.h"

#ifndef M_PI
#define M_PI 3.1415926535897932385
#endif

static QRgb getPixelEx(const QImage &img, const QRect &clipRect,
                       int x, int y)
{
  if (clipRect.contains(x, y))
    return img.pixel(x, y);
  else
  {
    int px = qBound(clipRect.left(), x, clipRect.right());
    int py = qBound(clipRect.top(),  y, clipRect.bottom());
    QRgb c = img.pixel(px, py);
    return qRgba(qRed(c), qGreen(c), qBlue(c), 0);
  }
}

static QRgb interpolate(const QImage &img, const QRect &clipRect,
                        double x, double y,
                        Interpolation method = Bilinear)
{
  switch (method)
  {
  case NearestNeighbor:
    return getPixelEx(img, clipRect, x, y);

  default:
  case Bilinear:
    {
      /* Pixels:
       *
       * C11 | C12
       * ---------
       * C21 | C22
       */
      QRgb c11 = getPixelEx(img, clipRect, floor(x), floor(y));
      QRgb c12 = getPixelEx(img, clipRect, ceil(x), floor(y));
      QRgb c21 = getPixelEx(img, clipRect, floor(x), ceil(y));
      QRgb c22 = getPixelEx(img, clipRect, ceil(x), ceil(y));

      double h = x-floor(x);
      double v = y-floor(y);

      // FIXME: RGBV doesn't handle alpha properly
      RGBV res;
      res.addk(c11, (1-h)*(1-v));
      res.addk(c12, h*(1-v));
      res.addk(c21, (1-h)*v);
      res.addk(c22, h*v);

      QRgb qres = res.toQRgb();
      int alpha = (1-h) * ((1-v)*qAlpha(c11) + v*qAlpha(c21))
                    + h * ((1-v)*qAlpha(c12) + v*qAlpha(c22));
      qres = qRgba(qRed(qres), qGreen(qres), qBlue(qres), alpha);

      return qres;
    }
  }
}

QImage transform(const QImage &img, const QRect &rect,
                 const Transform &transform, Interpolation ipol)
{
  QImage overlay(img.size(), img.format());
  overlay.fill(qRgba(0, 0, 0, 0));
  for (int y=0; y<img.height(); y++)
    for (int x=0; x<img.width(); x++)
    {
      double px, py;
      transform(x, y, px, py);
      overlay.setPixel(x, y, interpolate(img, rect, px, py, ipol));
    }
  // Assemble result
  QImage res(img.size(), img.format());
  QPainter p;
  p.begin(&res);
  p.drawImage(0, 0, img);
  p.fillRect(rect, Qt::black);
  p.drawImage(0, 0, overlay);
  p.end();
  return res;
}

QImage scale(const QImage &img, const QRect &rect,
             double factor, Interpolation ipol)
{
  double cx = rect.left() + rect.width()/2.0;
  double cy = rect.top() + rect.height()/2.0;
  Transform tr = Transform::shift(cx, cy) 
               * Transform::scale(1/factor, 1/factor) 
               * Transform::shift(-cx, -cy);
  return transform(img, rect, tr, ipol);
}

QImage rotate(const QImage &img, const QRect &rect,
              double degree, Interpolation ipol)
{
  double cx = rect.left() + rect.width()/2.0;
  double cy = rect.top() + rect.height()/2.0;
  Transform tr = Transform::shift(cx, cy)
               * Transform::rotate(degree * M_PI/180) 
               * Transform::shift(-cx, -cy);
  return transform(img, rect, tr, ipol);
}

