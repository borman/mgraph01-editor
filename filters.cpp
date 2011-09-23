#include <QWidget>
#include <QFormLayout>
#include <QDoubleSpinBox>
#include <QTableWidget>
#include <QLabel>
#include <QLineEdit>
#include <QHeaderView>

#include "filters.h"
#include "filters/colorcorrect.h"

QList<IFilter *> createFilters(QObject *parent)
{
  return QList<IFilter *>()
      << new Rotate(parent)
      << new Scale(parent)
      << new WhiteBalance(parent)
      << new LumaStretch(parent)
      << new RGBStretch(parent)
      << new GaussianBlur(parent)
      << new UnsharpMask(parent)
      << new Median(parent)
      << new MatteGlass(parent)
      << new CustomConvolution(parent);
}

// =======

void WhiteBalance::apply(QImage &image)
{
  whitebalance(image);
}

void LumaStretch::apply(QImage &image)
{
  luma_stretch(image);
}

void RGBStretch::apply(QImage &image)
{
  rgb_stretch(image);
}

// ========

GaussianBlur::GaussianBlur(QObject *parent)
  : QObject(parent), IFilter(new QWidget())
{
  QFormLayout *layout = new QFormLayout;
  settingsWidget()->setLayout(layout);

  sbRadius = new QDoubleSpinBox(settingsWidget());
  layout->addRow(tr("Radius:"), sbRadius);
}

void GaussianBlur::apply(QImage &image)
{
}


UnsharpMask::UnsharpMask(QObject *parent)
  : QObject(parent), IFilter(new QWidget())
{
  QFormLayout *layout = new QFormLayout;
  settingsWidget()->setLayout(layout);

  sbRadius = new QDoubleSpinBox(settingsWidget());
  layout->addRow(tr("Radius:"), sbRadius);

  sbStrength = new QDoubleSpinBox(settingsWidget());
  layout->addRow(tr("Strength:"), sbStrength);
}

void UnsharpMask::apply(QImage &image)
{
}

Median::Median(QObject *parent)
  : QObject(parent), IFilter(new QWidget())
{
  QFormLayout *layout = new QFormLayout;
  settingsWidget()->setLayout(layout);

  sbSize = new QSpinBox(settingsWidget());
  layout->addRow(tr("Filter size:"), sbSize);
}

void Median::apply(QImage &image)
{
}

MatteGlass::MatteGlass(QObject *parent)
  : QObject(parent), IFilter(new QWidget())
{
  QFormLayout *layout = new QFormLayout;
  settingsWidget()->setLayout(layout);

  sbRadius = new QDoubleSpinBox(settingsWidget());
  layout->addRow(tr("Radius:"), sbRadius);

  sbSamples = new QSpinBox(settingsWidget());
  layout->addRow(tr("Samples:"), sbSamples);
}

void MatteGlass::apply(QImage &image)
{
}

Rotate::Rotate(QObject *parent)
  : QObject(parent), IFilter(new QWidget())
{
  QFormLayout *layout = new QFormLayout;
  settingsWidget()->setLayout(layout);

  sbAngle = new QSpinBox(settingsWidget());
  layout->addRow(tr("Angle (degrees):"), sbAngle);
}

void Rotate::apply(QImage &image)
{
}

Scale::Scale(QObject *parent)
  : QObject(parent), IFilter(new QWidget())
{
  QFormLayout *layout = new QFormLayout;
  settingsWidget()->setLayout(layout);

  sbFactor = new QDoubleSpinBox(settingsWidget());
  layout->addRow(tr("Factor:"), sbFactor);
}

void Scale::apply(QImage &image)
{
}

CustomConvolution::CustomConvolution(QObject *parent)
  : QObject(parent), IFilter(new QWidget())
{
  const int maxSize = 7;
  const int minSize = 3;
  const int defaultSize = 3;

  QFormLayout *layout = new QFormLayout;
  layout->setRowWrapPolicy(QFormLayout::WrapAllRows);
  settingsWidget()->setLayout(layout);

  slSize = new QSlider(Qt::Horizontal, settingsWidget());
  slSize->setRange(minSize, maxSize);
  slSize->setValue(defaultSize);
  slSize->setTickPosition(QSlider::TicksBelow);
  slSize->setTickInterval(1);
  lblMatrixSize = new QLabel(settingsWidget());
  layout->addRow(lblMatrixSize, slSize);
  connect(slSize, SIGNAL(valueChanged(int)), SLOT(updateMatrixSize(int)));

  grid = new QGridLayout;
  grid->setSpacing(0);
  for (int y=0; y<maxSize; y++)
    for (int x=0; x<maxSize; x++)
    {
      QLineEdit *le = new QLineEdit("0.0", settingsWidget());
      le->setValidator(new QDoubleValidator(le));
      grid->addWidget(le, y, x);
    }
  layout->addRow(grid);

  updateMatrixSize(defaultSize);
}

void CustomConvolution::updateMatrixSize(int newSize)
{
  lblMatrixSize->setText(tr("Matrix size: %1x%2").arg(newSize).arg(newSize));
  for (int y=0; y<grid->rowCount(); y++)
    for (int x=0; x<grid->columnCount(); x++)
    {
      QWidget *w = grid->itemAtPosition(y, x)->widget();
      if (!w)
        continue;
      if (y<newSize && x<newSize)
        w->show();
      else
        w->hide();
    }
}

void CustomConvolution::apply(QImage &image)
{
}
