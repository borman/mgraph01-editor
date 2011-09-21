#include <QWidget>
#include <QVBoxLayout>
#include <QToolButton>
#include "filterwrapper.h"

FilterWrapper::FilterWrapper(IFilter *filter, QObject *parent) :
    QObject(parent), m_filter(filter)
{
}

QWidget *FilterWrapper::createWidget(QWidget *parent)
{
  QWidget *container = new QWidget(parent);

  QVBoxLayout *layout = new QVBoxLayout;
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);
  container->setLayout(layout);

  QToolButton *filterButton = new QToolButton(container);
  filterButton->setText(m_filter->filterName());
  filterButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  filterButton->setCheckable(true);
  layout->addWidget(filterButton);

  if (m_filter->hasSettings())
  {
    QWidget *settings = m_filter->settingsWidget();
    layout->addWidget(settings);
    settings->hide();
  }
  return container;
}
