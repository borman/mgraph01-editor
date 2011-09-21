#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <QColor>
#include <QPixmap>

// Pixel property accessor (R, G, B, Luma, etc)
typedef double (*ColorProp)(QRgb color);

QPixmap histogram(const QImage &img, ColorProp prop, int w, int h,
                  const QBrush &bg, const QPen &fg);

double getLuma(QRgb rgb);
double getRed(QRgb rgb);
double getGreen(QRgb rgb);
double getBlue(QRgb rgb);

#endif // HISTOGRAM_H
