#ifndef FILTERS_H
#define FILTERS_H

#include <QObject>
#include <QWidget>
#include <QImage>
#include "ifilter.h"

class QSpinBox;
class QDoubleSpinBox;
class QGridLayout;
class QSlider;
class QLabel;
class QComboBox;

// Get filter instances
QList<IFilter *> createFilters(QObject *parent);


// Simple filters

class WhiteBalance: public QObject, public IFilter
{
    Q_OBJECT
  public:
    WhiteBalance(QObject *parent) : QObject(parent) {}
    // reimplemented
    virtual QString filterName() { return tr("White Balance"); }
    virtual void apply(QImage &image);
};

class LumaStretch: public QObject, public IFilter
{
    Q_OBJECT
  public:
    LumaStretch(QObject *parent) : QObject(parent) {}
    // reimplemented
    virtual QString filterName() { return tr("Luma Stretch"); }
    virtual void apply(QImage &image);
};

class RGBStretch: public QObject, public IFilter
{
    Q_OBJECT
  public:
    RGBStretch(QObject *parent) : QObject(parent) {}
    // reimplemented
    virtual QString filterName() { return tr("RGB Stretch"); }
    virtual void apply(QImage &image);
};

// Complex filters

class GaussianBlur: public QObject, public IFilter
{
    Q_OBJECT
  public:
    GaussianBlur(QObject *parent);
    // reimplemented
    virtual QString filterName() { return tr("Gaussian Blur"); }
    virtual void apply(QImage &image);
  private slots:
    void filterChanged();
  private:
    QDoubleSpinBox *sbRadius;
    QLabel *lblSize;
    QLabel *lblVisual;
};

class UnsharpMask: public QObject, public IFilter
{
    Q_OBJECT
  public:
    UnsharpMask(QObject *parent);
    // reimplemented
    virtual QString filterName() { return tr("Unsharp Mask"); }
    virtual void apply(QImage &image);
  private slots:
    void filterChanged();
  private:
    QDoubleSpinBox *sbRadius;
    QDoubleSpinBox *sbStrength;
    QLabel *lblSize;
    QLabel *lblVisual;
};

class Median: public QObject, public IFilter
{
    Q_OBJECT
  public:
    Median(QObject *parent);
    // reimplemented
    virtual QString filterName() { return tr("Median"); }
    virtual void apply(QImage &image);
  private:
    QSpinBox *sbSize;
};

class MatteGlass: public QObject, public IFilter
{
    Q_OBJECT
  public:
    MatteGlass(QObject *parent);
    // reimplemented
    virtual QString filterName() { return tr("Matte Glass"); }
    virtual void apply(QImage &image);
  private:
    QDoubleSpinBox *sbRadius;
    QSpinBox *sbSamples;
};

class Rotate: public QObject, public IFilter
{
    Q_OBJECT
  public:
    Rotate(QObject *parent);
    // reimplemented
    virtual QString filterName() { return tr("Rotate"); }
    virtual void apply(QImage &image);
  private:
    QDoubleSpinBox *sbAngle;
};

class Scale: public QObject, public IFilter
{
    Q_OBJECT
  public:
    Scale(QObject *parent);
    // reimplemented
    virtual QString filterName() { return tr("Scale"); }
    virtual void apply(QImage &image);
  private:
    QDoubleSpinBox *sbFactor;
};

class CustomConvolution: public QObject, public IFilter
{
    Q_OBJECT
  public:
    CustomConvolution(QObject *parent);
    // reimplemented
    virtual QString filterName() { return tr("Convolution"); }
    virtual void apply(QImage &image);
  private slots:
    void updateMatrixSize();
  private:
    QGridLayout *grid;
    QComboBox *cbSize;
    QLabel *lblMatrixSize;
};

#endif // FILTERS_H
