#ifndef IFILTER_H
#define IFILTER_H

class QWidget;
#include <QString>
#include <QImage>

class IFilter
{
  public:
    virtual bool hasSettings() = 0;
    virtual QWidget *settingsWidget(QWidget *parent = 0) = 0;
    virtual QString filterName() = 0;
    virtual void apply(QImage &image) = 0;
};

class ISimpleFilter: public IFilter
{
  public:
    virtual bool hasSettings() { return false; }
    virtual QWidget *settingsWidget(QWidget *) { return 0; }
};

#endif // IFILTER_H
