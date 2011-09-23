#ifndef FILTERWRAPPER_H
#define FILTERWRAPPER_H

#include <QWidget>
#include "ifilter.h"

class QToolButton;

class FilterWrapper : public QWidget
{
    Q_OBJECT
  public:
    FilterWrapper(IFilter *filter, QWidget *parent = 0);
    IFilter *filter() const { return m_filter; }

  signals:
    void activated();
    void apply();

  public slots:
    void collapse();

  private slots:
    void buttonToggled(bool isPressed);

  private:
    IFilter *m_filter;
    QToolButton *m_button;
    QWidget *m_controlButtons;
    QToolButton *m_applyButton;
};

#endif // FILTERWRAPPER_H
