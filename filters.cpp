#include <QWidget>
#include <QFormLayout>
#include <QDoubleSpinBox>

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
      << new MatteGlass(parent);
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

QWidget *GaussianBlur::settingsWidget(QWidget *parent)
{
  QWidget *container = new QWidget(parent);

  QFormLayout *layout = new QFormLayout;
  container->setLayout(layout);

  QDoubleSpinBox *sbRadius = new QDoubleSpinBox(container);
  layout->addRow(tr("Radius:"), sbRadius);

  return container;
}

void GaussianBlur::apply(QImage &image)
{
}


QWidget *UnsharpMask::settingsWidget(QWidget *parent)
{
  QWidget *container = new QWidget(parent);

  QFormLayout *layout = new QFormLayout;
  container->setLayout(layout);

  QDoubleSpinBox *sbRadius = new QDoubleSpinBox(container);
  layout->addRow(tr("Radius:"), sbRadius);

  QDoubleSpinBox *sbStrength = new QDoubleSpinBox(container);
  layout->addRow(tr("Strength:"), sbStrength);

  return container;
}

void UnsharpMask::apply(QImage &image)
{
}

QWidget *Median::settingsWidget(QWidget *parent)
{
  QWidget *container = new QWidget(parent);

  QFormLayout *layout = new QFormLayout;
  container->setLayout(layout);

  QSpinBox *sbSize = new QSpinBox(container);
  layout->addRow(tr("Filter size:"), sbSize);

  return container;
}

void Median::apply(QImage &image)
{
}

QWidget *MatteGlass::settingsWidget(QWidget *parent)
{
  QWidget *container = new QWidget(parent);

  QFormLayout *layout = new QFormLayout;
  container->setLayout(layout);

  QDoubleSpinBox *sbRadius = new QDoubleSpinBox(container);
  layout->addRow(tr("Radius:"), sbRadius);

  QSpinBox *sbSamples = new QSpinBox(container);
  layout->addRow(tr("Samples:"), sbSamples);

  return container;
}

void MatteGlass::apply(QImage &image)
{
}

QWidget *Rotate::settingsWidget(QWidget *parent)
{
  QWidget *container = new QWidget(parent);

  QFormLayout *layout = new QFormLayout;
  container->setLayout(layout);

  QSpinBox *sbAngle = new QSpinBox(container);
  layout->addRow(tr("Angle (degrees):"), sbAngle);

  return container;
}

void Rotate::apply(QImage &image)
{
}

QWidget *Scale::settingsWidget(QWidget *parent)
{
  QWidget *container = new QWidget(parent);

  QFormLayout *layout = new QFormLayout;
  container->setLayout(layout);

  QDoubleSpinBox *sbFactor = new QDoubleSpinBox(container);
  layout->addRow(tr("Factor:"), sbFactor);

  return container;
}

void Scale::apply(QImage &image)
{
}

