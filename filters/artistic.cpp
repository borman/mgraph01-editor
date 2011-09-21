#include "artistic.h"
#include "rgbv.h"

// Normal distribution approximation in [-1..1]
static double rand_n()
{
  static const int q = 12;
  double sum = 0;
  for (int i=0; i<q; i++)
    sum += double(rand())/double(RAND_MAX);
  sum -= q/2.0;
  sum /= q/2.0;
  return sum;
}

static double rand_u()
{
  return (double(rand())/double(RAND_MAX) - 0.5) * 2;
}

static int variate(int min, int base, int max, int radius)
{
#if 0
  int d = rand_u() * radius;
#else
  int d = rand_n() * radius;
#endif
  return qBound(min, base+d, max);
}

void glass(QImage &img, int radius, int samples)
{
  QImage src = img;
  for (int y=0; y<img.height(); y++)
    for (int x=0; x<img.width(); x++)
    {
      RGBV acc;
      double k = 1.0/samples;
      for (int i=0; i<samples; i++)
      {
        int px = variate(0, x, img.width()-1, radius);
        int py = variate(0, y, img.height()-1, radius);
        acc.addk(src.pixel(px, py), k);
      }
      img.setPixel(x, y, acc.toQRgb());
    }
}

