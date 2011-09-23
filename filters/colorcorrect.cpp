#include "colorcorrect.h"
#include "rgbv.h"
#include "histogram.h"

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
  int qmin, qmax;
  makeHistogram(img, getLuma, 256, &qmin, &qmax);

  double ymin = qmin/255.0;
  double k = qmax==qmin? 1 : 255.0/(qmax-qmin);

  for (int y=0; y<img.height(); y++)
    for (int x=0; x<img.width(); x++)
    {
      RGBV c(img.pixel(x, y));
      double yval = getLuma(img.pixel(x, y)); // Current luminance
      double ytgt = (yval - ymin)*k; // Target luminance
      c.mul(ytgt/yval);
      c.clamp();
      img.setPixel(x, y, c.toQRgb());
    }
}

void rgb_stretch(QImage &img)
{
  int rmin, rmax;
  int gmin, gmax;
  int bmin, bmax;
  makeHistogram(img, getRed,   256, &rmin, &rmax);
  makeHistogram(img, getGreen, 256, &gmin, &gmax);
  makeHistogram(img, getBlue,  256, &bmin, &bmax);

  RGBV lo(rmin/255.0, gmin/255.0, bmin/255.0);
  RGBV stretch(rmax==rmin? 1.0 : 255.0/(rmax-rmin),
               gmax==gmin? 1.0 : 255.0/(gmax-gmin),
               bmax==bmin? 1.0 : 255.0/(bmax-bmin));
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
