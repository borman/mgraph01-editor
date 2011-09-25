#ifndef IFILTER_H
#define IFILTER_H

class QWidget;
#include <QString>
#include <QImage>

class IFilter
{
  public:
    IFilter(QWidget *_sw = 0) : m_settingsWidget(_sw) {}

    bool hasSettings() { return m_settingsWidget != 0; }
    QWidget *settingsWidget() const { return m_settingsWidget; }

    virtual QString filterName() = 0;
    virtual void apply(QImage &image, const QRect &rect) = 0;

  private:
    QWidget *m_settingsWidget;
};

#endif // IFILTER_H
