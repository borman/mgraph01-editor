#include "colorcorrect.h"
#include "rgbv.h"

void whitebalance(QImage &img)
{
  RGBV mean(0.1, 0.1, 0.1); // Avoid zero division
  
  // Measure
  for (int y=0; y<img.height(); y++)
    for (int x=0; x<img.width(); x++)
      mean.add(img.pixel(x, y));
  
  double avg = (mean.r + mean.g + mean.b)/3;
  RGBV k(avg/mean.r, avg/mean.g, avg/mean.b);

  // Adjust
  for (int y=0; y<img.height(); y++)
    for (int x=0; x<img.width(); x++)
    {
      RGBV p(img.pixel(x, y));
      p.mulv(k);
      p.clamp();
      img.setPixel(x, y, p.toQRgb());
    }
}

void luma_stretch(QImage &img)
{
  static const RGBV luma(0.2125, 0.7154, 0.0721); // BT.709

  double ymin=1.0, ymax=0.0;
  for (int y=0; y<img.height(); y++)
    for (int x=0; x<img.width(); x++)
    {
      double yval = luma.dot(img.pixel(x, y));
      ymin = qMin(ymin, yval);
      ymax = qMax(ymax, yval);
    }

  for (int y=0; y<img.height(); y++)
    for (int x=0; x<img.width(); x++)
    {
      RGBV c(img.pixel(x, y));
      double yval = luma.dot(c); // Current luminance
      double ytgt = (yval - ymin)/(ymax - ymin); // Target luminance
      c.mul(ytgt/yval);
      c.clamp();
      img.setPixel(x, y, c.toQRgb());
    }
}

void rgb_stretch(QImage &img)
{
  double rmin=1.0, rmax=0.0;
  double gmin=1.0, gmax=0.0;
  double bmin=1.0, bmax=0.0;
  for (int y=0; y<img.height(); y++)
    for (int x=0; x<img.width(); x++)
    {
      RGBV c(img.pixel(x, y));
      rmin = qMin(rmin, c.r);
      rmax = qMax(rmax, c.r);
      gmin = qMin(gmin, c.g);
      gmax = qMax(gmax, c.g);
      bmin = qMin(bmin, c.b);
      bmax = qMax(bmax, c.b);
    }
  RGBV lo(rmin, gmin, bmin);
  RGBV stretch(1/(rmax-rmin), 1/(gmax-gmin), 1/(bmax-bmin));
  for (int y=0; y<img.height(); y++)
    for (int x=0; x<img.width(); x++)
    {
      RGBV c(img.pixel(x, y));
      c.addk(lo, -1);
      c.mulv(stretch);
      c.clamp();
      img.setPixel(x, y, c.toQRgb());
    }
}
