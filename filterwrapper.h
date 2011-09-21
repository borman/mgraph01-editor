#ifndef FILTERWRAPPER_H
#define FILTERWRAPPER_H

#include <QObject>
#include "ifilter.h"

class FilterWrapper : public QObject
{
    Q_OBJECT
  public:
    FilterWrapper(IFilter *filter, QObject *parent = 0);
    QWidget *createWidget(QWidget *parent);

  signals:

  public slots:
  private:
    IFilter *m_filter;
};

#endif // FILTERWRAPPER_H
