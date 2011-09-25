#include <QWidget>
#include <QVBoxLayout>
#include <QToolButton>
#include "filterwrapper.h"

FilterWrapper::FilterWrapper(IFilter *filter, QWidget *parent) :
    QWidget(parent), m_filter(filter), m_button(0),
    m_controlButtons(0), m_applyButton(0)
{
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    setLayout(layout);

    m_controlButtons = new QWidget(this);
    QHBoxLayout *cbLayout = new QHBoxLayout();
    cbLayout->setContentsMargins(0, 0, 0, 0);
    cbLayout->setSpacing(0);
    m_controlButtons->setLayout(cbLayout);
    layout->addWidget(m_controlButtons);

    // Main button
    m_button = new QToolButton(this);
    m_button->setText(m_filter->filterName());
    m_button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    m_button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    cbLayout->addWidget(m_button);

    if (m_filter->hasSettings())
    {
      // Main button: toggle settings
      m_button->setArrowType(Qt::RightArrow);
      m_button->setCheckable(true);
      connect(m_button, SIGNAL(toggled(bool)), SLOT(buttonToggled(bool)));

      // Apply button needed
      m_applyButton = new QToolButton(this);
      m_applyButton->setText(tr("Apply"));
      QFont fnt = m_applyButton->font();
      fnt.setBold(true);
      m_applyButton->setFont(fnt);
      m_applyButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
      cbLayout->addWidget(m_applyButton);
      connect(m_applyButton, SIGNAL(clicked()), SIGNAL(apply()));
      m_applyButton->hide();

      // Insert settings widget
      m_filter->settingsWidget()->setParent(this);
      layout->addWidget(m_filter->settingsWidget());
      layout->addStrut(m_filter->settingsWidget()->minimumSizeHint().width()); // Fix size hopping
      m_filter->settingsWidget()->hide();
    }
    else
      // Main button: apply filter
      connect(m_button, SIGNAL(clicked()), SIGNAL(apply()));
}

void FilterWrapper::buttonToggled(bool isPressed)
{
  if (isPressed)
  {
    emit activated();
    if (m_filter->hasSettings())
    {
      m_filter->settingsWidget()->show();
      m_button->setArrowType(Qt::DownArrow);
      m_applyButton->show();
    }
  }
  else
  {
    if (m_filter->hasSettings())
    {
      m_filter->settingsWidget()->hide();
      m_button->setArrowType(Qt::RightArrow);
      m_applyButton->hide();
    }
  }
}

void FilterWrapper::collapse()
{
  m_button->setChecked(false);
}
