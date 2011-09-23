#include <QWidget>
#include <QFormLayout>
#include <QDoubleSpinBox>
#include <QTableWidget>
#include <QLabel>
#include <QLineEdit>
#include <QHeaderView>
#include <QLocale>

#include "filters.h"
#include "filters/colorcorrect.h"
#include "filters/artistic.h"
#include "filters/transform.h"
#include "filters/convolution.h"

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
  sbRadius->setRange(0.01, 10);
  sbRadius->setValue(1);
  layout->addRow(tr("Radius:"), sbRadius);
}

void GaussianBlur::apply(QImage &image)
{
  double sigma = sbRadius->value();
  int size = 2*int(sigma*3) - 1;
  convolve(image, gaussian(size, sigma));
}


UnsharpMask::UnsharpMask(QObject *parent)
  : QObject(parent), IFilter(new QWidget())
{
  QFormLayout *layout = new QFormLayout;
  settingsWidget()->setLayout(layout);

  sbRadius = new QDoubleSpinBox(settingsWidget());
  sbRadius->setRange(0.01, 10);
  sbRadius->setValue(1);
  layout->addRow(tr("Radius:"), sbRadius);

  sbStrength = new QDoubleSpinBox(settingsWidget());
  sbStrength->setRange(0.01, 10);
  sbStrength->setValue(0.5);
  layout->addRow(tr("Strength:"), sbStrength);
}

void UnsharpMask::apply(QImage &image)
{
  double sigma = sbRadius->value();
  int size = 2*int(sigma*3) - 1;
  convolve(image, unsharp(size, sigma, sbStrength->value()));
}

Median::Median(QObject *parent)
  : QObject(parent), IFilter(new QWidget())
{
  QFormLayout *layout = new QFormLayout;
  settingsWidget()->setLayout(layout);

  sbSize = new QSpinBox(settingsWidget());
  sbSize->setRange(3, 7);
  sbSize->setValue(3);
  layout->addRow(tr("Filter size:"), sbSize);
}

void Median::apply(QImage &image)
{
  median(image, sbSize->value());
}

MatteGlass::MatteGlass(QObject *parent)
  : QObject(parent), IFilter(new QWidget())
{
  QFormLayout *layout = new QFormLayout;
  settingsWidget()->setLayout(layout);

  sbRadius = new QDoubleSpinBox(settingsWidget());
  sbRadius->setRange(0.1, 100);
  sbRadius->setValue(10);
  layout->addRow(tr("Radius:"), sbRadius);

  sbSamples = new QSpinBox(settingsWidget());
  sbSamples->setRange(1, 20);
  sbSamples->setValue(5);
  layout->addRow(tr("Samples:"), sbSamples);
}

void MatteGlass::apply(QImage &image)
{
  glass(image, sbRadius->value(), sbSamples->value());
}

Rotate::Rotate(QObject *parent)
  : QObject(parent), IFilter(new QWidget())
{
  QFormLayout *layout = new QFormLayout;
  settingsWidget()->setLayout(layout);

  sbAngle = new QDoubleSpinBox(settingsWidget());
  sbAngle->setRange(-180, 180);
  sbAngle->setValue(0);
  layout->addRow(tr("Angle (degrees):"), sbAngle);
}

void Rotate::apply(QImage &image)
{
  image = rotate(image, sbAngle->value());
}

Scale::Scale(QObject *parent)
  : QObject(parent), IFilter(new QWidget())
{
  QFormLayout *layout = new QFormLayout;
  settingsWidget()->setLayout(layout);

  sbFactor = new QDoubleSpinBox(settingsWidget());
  sbFactor->setRange(0.1, 10);
  sbFactor->setValue(1);
  layout->addRow(tr("Factor:"), sbFactor);
}

void Scale::apply(QImage &image)
{
  image = scale(image, sbFactor->value());
}

CustomConvolution::CustomConvolution(QObject *parent)
  : QObject(parent), IFilter(new QWidget())
{
  const int maxSize = 7;
  const int minSize = 3;
  const int defaultSize = 3;

  QLocale l = QLocale::system();

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
      QLineEdit *le = new QLineEdit(l.toString(0.0, 'f', 1), settingsWidget());
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
  int size = slSize->value();
  QLocale l = QLocale::system();
  Matrix<double> m(size);
  for (int y=0; y<size; y++)
    for (int x=0; x<size; x++)
    {
      QLineEdit *le = qobject_cast<QLineEdit *>(grid->itemAtPosition(y, x)->widget());
      Q_ASSERT(le != NULL);
      if (le->hasAcceptableInput())
        m.set(x, y, l.toDouble(le->text()));
      else
        m.set(x, y, 0);
    }
  convolve(image, m);
}
