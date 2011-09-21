#include <cmath>
#include <QPainter>
#include "histogram.h"

QPixmap histogram(const QImage &img, ColorProp prop, int w, int h,
                  const QBrush &bg, const QPen &fg)
{
  int stats[w];
  for (int i=0; i<w; i++)
    stats[i] = 0;

  for (int y=0; y<img.height(); y++)
    for (int x=0; x<img.width(); x++)
    {
      int pos = round(prop(img.pixel(x, y)) * (w-1));
      stats[pos] += 1;
    }

  QPixmap res(w, h);
  QPainter p;
  p.begin(&res);
  p.setBrush(bg);
  p.setPen(fg);
  p.fillRect(res.rect(), bg);

  int smax = stats[0];
  for (int i=1; i<w; i++)
    smax = qMax(smax, stats[i]);
  if (smax == 0)
    smax = 100;

  for (int i=0; i<w; i++)
  {
    double val = stats[i]/double(smax);
    p.drawLine(i, h-1, i, h-1 - val*(h-2));
  }

  p.end();

  return res;
}

double getLuma(QRgb rgb)
{
  return 0.2125*getRed(rgb) + 0.7154*getGreen(rgb) + 0.0721*getBlue(rgb); // BT.709
}

double getRed(QRgb rgb)
{
  return qRed(rgb)/255.0;
}

double getGreen(QRgb rgb)
{
  return qGreen(rgb)/255.0;
}

double getBlue(QRgb rgb)
{
  return qBlue(rgb)/255.0;
}
