#ifndef FILTERS_H
#define FILTERS_H

#include <QObject>
#include <QWidget>
#include <QImage>
#include "ifilter.h"

// Get filter instances
QList<IFilter *> createFilters(QObject *parent);


// Simple filters

class WhiteBalance: public QObject, public ISimpleFilter
{
    Q_OBJECT
  public:
    WhiteBalance(QObject *parent) : QObject(parent) {}
    // reimplemented
    virtual QString filterName() { return tr("White Balance"); }
    virtual void apply(QImage &image);
};

class LumaStretch: public QObject, public ISimpleFilter
{
    Q_OBJECT
  public:
    LumaStretch(QObject *parent) : QObject(parent) {}
    // reimplemented
    virtual QString filterName() { return tr("Luma Stretch"); }
    virtual void apply(QImage &image);
};

class RGBStretch: public QObject, public ISimpleFilter
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
    GaussianBlur(QObject *parent) : QObject(parent) {}
    // reimplemented
    virtual bool hasSettings() { return true; }
    virtual QString filterName() { return tr("Gaussian Blur"); }
    virtual QWidget *settingsWidget(QWidget *parent);
    virtual void apply(QImage &image);
};

class UnsharpMask: public QObject, public IFilter
{
    Q_OBJECT
  public:
    UnsharpMask(QObject *parent) : QObject(parent) {}
    // reimplemented
    virtual bool hasSettings() { return true; }
    virtual QString filterName() { return tr("Unsharp Mask"); }
    virtual QWidget *settingsWidget(QWidget *parent);
    virtual void apply(QImage &image);
};

class Median: public QObject, public IFilter
{
    Q_OBJECT
  public:
    Median(QObject *parent) : QObject(parent) {}
    // reimplemented
    virtual bool hasSettings() { return true; }
    virtual QString filterName() { return tr("Median"); }
    virtual QWidget *settingsWidget(QWidget *parent);
    virtual void apply(QImage &image);
};

class MatteGlass: public QObject, public IFilter
{
    Q_OBJECT
  public:
    MatteGlass(QObject *parent) : QObject(parent) {}
    // reimplemented
    virtual bool hasSettings() { return true; }
    virtual QString filterName() { return tr("Matte Glass"); }
    virtual QWidget *settingsWidget(QWidget *parent);
    virtual void apply(QImage &image);
};

class Rotate: public QObject, public IFilter
{
    Q_OBJECT
  public:
    Rotate(QObject *parent) : QObject(parent) {}
    // reimplemented
    virtual bool hasSettings() { return true; }
    virtual QString filterName() { return tr("Rotate"); }
    virtual QWidget *settingsWidget(QWidget *parent);
    virtual void apply(QImage &image);
};

class Scale: public QObject, public IFilter
{
    Q_OBJECT
  public:
    Scale(QObject *parent) : QObject(parent) {}
    // reimplemented
    virtual bool hasSettings() { return true; }
    virtual QString filterName() { return tr("Scale"); }
    virtual QWidget *settingsWidget(QWidget *parent);
    virtual void apply(QImage &image);
};

#endif // FILTERS_H
