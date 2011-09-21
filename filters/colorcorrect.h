#ifndef COLORCORRECT_H
#define COLORCORRECT_H

#include <QImage>

void whitebalance(QImage &img);
void luma_stretch(QImage &img);
void rgb_stretch(QImage &img);

#endif // COLORCORRECT_H

