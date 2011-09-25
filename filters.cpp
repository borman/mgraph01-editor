#include <QWidget>
#include <QFormLayout>
#include <QDoubleSpinBox>
#include <QTableWidget>
#include <QLabel>
#include <QLineEdit>
#include <QHeaderView>
#include <QLocale>
#include <QPainter>
#include <QRect>
#include <QComboBox>

#include "filters.h"
#include "filters/colorcorrect.h"
#include "filters/artistic.h"
#include "filters/transform.h"
#include "filters/convolution.h"

QList<IFilter *> createFilters(QObject *parent)
{
  return QList<IFilter *>()
      << new WhiteBalance(parent)
      << new LumaStretch(parent)
      << new RGBStretch(parent)
      << 0
      << new Rotate(parent)
      << new Scale(parent)
      << 0
      << new GaussianBlur(parent)
      << new UnsharpMask(parent)
      << new Median(parent)
      << new MatteGlass(parent)
      << new CustomConvolution(parent);
}

static int sizeForSigma(double sigma)
{
  return qMax(1, int(2*sigma)-1);
}

static int actualFilterSize(int size)
{
  return 2*size+1;
}

static QPixmap visualFilter(const Matrix<double> &m)
{
  static const int cell = 3;

  QPixmap viz(m.size()*cell, m.size()*cell);
  QPainter p;
  p.begin(&viz);

  double maxVal = 0.0001, minVal = -0.0001;
  for (int y=0; y<m.size(); y++)
    for (int x=0; x<m.size(); x++)
    {
      maxVal = qMax(m.at(x, y), maxVal);
      minVal = qMin(m.at(x, y), minVal);
    }

  for (int y=0; y<m.size(); y++)
    for (int x=0; x<m.size(); x++)
    {
      QRect cellRect(x*cell, y*cell, cell, cell);
      double val = m.at(x, y)>0? m.at(x, y)/maxVal : m.at(x, y)/minVal;
      QColor col = m.at(x, y)>0? QColor(val*50, val*100, val*255)
                               : QColor(val*255, val*100, val*50);
      p.fillRect(cellRect, col);
    }
  p.end();

  return viz;
}

// =======

void WhiteBalance::apply(QImage &image, const QRect &rect)
{
  whitebalance(image, rect);
}

void LumaStretch::apply(QImage &image, const QRect &rect)
{
  luma_stretch(image, rect);
}

void RGBStretch::apply(QImage &image, const QRect &rect)
{
  rgb_stretch(image, rect);
}

// ========

GaussianBlur::GaussianBlur(QObject *parent)
  : QObject(parent), IFilter(new QWidget())
{
  QFormLayout *layout = new QFormLayout;
  settingsWidget()->setLayout(layout);

  sbRadius = new QDoubleSpinBox(settingsWidget());
  sbRadius->setRange(0.1, 10);
  sbRadius->setSingleStep(0.1);
  sbRadius->setValue(1);

  lblSize = new QLabel(settingsWidget());
  lblVisual = new QLabel(settingsWidget());

  layout->addRow(tr("Radius:"), sbRadius);
  layout->addRow(tr("Filter size:"), lblSize);
  layout->addRow(tr("Preview:"), lblVisual);

  connect(sbRadius, SIGNAL(valueChanged(double)), SLOT(filterChanged()));
  filterChanged();
}

void GaussianBlur::apply(QImage &image, const QRect &rect)
{
  double sigma = sbRadius->value();
  convolve(image, rect, gaussian(sizeForSigma(sigma), sigma));
}

void GaussianBlur::filterChanged()
{
  double sigma = sbRadius->value();
  int hsize = sizeForSigma(sigma);
  int size = actualFilterSize(hsize);
  lblSize->setText(tr("%1x%1").arg(size));
  lblVisual->setPixmap(visualFilter(gaussian(hsize, sigma)));
}

UnsharpMask::UnsharpMask(QObject *parent)
  : QObject(parent), IFilter(new QWidget())
{
  QFormLayout *layout = new QFormLayout;
  settingsWidget()->setLayout(layout);

  sbRadius = new QDoubleSpinBox(settingsWidget());
  sbRadius->setRange(0.1, 10);
  sbRadius->setSingleStep(0.1);
  sbRadius->setValue(1);

  sbStrength = new QDoubleSpinBox(settingsWidget());
  sbStrength->setRange(0.01, 10);
  sbStrength->setSingleStep(0.1);
  sbStrength->setValue(0.5);

  lblSize = new QLabel(settingsWidget());
  lblVisual = new QLabel(settingsWidget());

  layout->addRow(tr("Radius:"), sbRadius);
  layout->addRow(tr("Strength:"), sbStrength);
  layout->addRow(tr("Filter size:"), lblSize);
  layout->addRow(tr("Preview:"), lblVisual);

  connect(sbRadius, SIGNAL(valueChanged(double)), SLOT(filterChanged()));
  connect(sbStrength, SIGNAL(valueChanged(double)), SLOT(filterChanged()));
  filterChanged();
}

void UnsharpMask::apply(QImage &image, const QRect &rect)
{
  double sigma = sbRadius->value();
  convolve(image, rect, unsharp(sizeForSigma(sigma), sigma, sbStrength->value()));
}

void UnsharpMask::filterChanged()
{
  double sigma = sbRadius->value();
  int hsize = sizeForSigma(sigma);
  int size = actualFilterSize(hsize);
  double alpha = sbStrength->value();
  lblSize->setText(tr("%1x%1").arg(size));
  lblVisual->setPixmap(visualFilter(unsharp(hsize, sigma, alpha)));
}

Median::Median(QObject *parent)
  : QObject(parent), IFilter(new QWidget())
{
  QFormLayout *layout = new QFormLayout;
  settingsWidget()->setLayout(layout);

  cbSize = new QComboBox(settingsWidget());
  cbSize->addItem(tr("3x3"), 3);
  cbSize->addItem(tr("5x5"), 5);
  cbSize->addItem(tr("7x7"), 7);
  cbSize->setCurrentIndex(0);

  layout->addRow(tr("Filter size:"), cbSize);
}

void Median::apply(QImage &image, const QRect &rect)
{
  int size = cbSize->itemData(cbSize->currentIndex()).toInt();
  median(image, rect, size);
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

void MatteGlass::apply(QImage &image, const QRect &rect)
{
  glass(image, rect, sbRadius->value(), sbSamples->value());
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

void Rotate::apply(QImage &image, const QRect &rect)
{
  image = rotate(image, rect, sbAngle->value());
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

void Scale::apply(QImage &image, const QRect &rect)
{
  image = scale(image, rect, sbFactor->value());
}

CustomConvolution::CustomConvolution(QObject *parent)
  : QObject(parent), IFilter(new QWidget())
{
  static const int maxSize = 7;

  QLocale l = QLocale::system();

  QFormLayout *layout = new QFormLayout;
  settingsWidget()->setLayout(layout);

  cbSize = new QComboBox(settingsWidget());
  cbSize->addItem(tr("3x3"), 3);
  cbSize->addItem(tr("5x5"), 5);
  cbSize->addItem(tr("7x7"), 7);
  cbSize->setCurrentIndex(0);

  grid = new QGridLayout;
  grid->setSpacing(0);
  for (int y=0; y<maxSize; y++)
    for (int x=0; x<maxSize; x++)
    {
      QLineEdit *le = new QLineEdit(l.toString(0.0, 'f', 1), settingsWidget());
      le->setValidator(new QDoubleValidator(le));
      grid->addWidget(le, y, x);
    }

  layout->addRow(tr("Matrix size:"), cbSize);
  layout->addRow(grid);

  connect(cbSize, SIGNAL(currentIndexChanged(int)), SLOT(updateMatrixSize()));

  updateMatrixSize();
}

void CustomConvolution::updateMatrixSize()
{
  int size = cbSize->itemData(cbSize->currentIndex()).toInt();
  for (int y=0; y<grid->rowCount(); y++)
    for (int x=0; x<grid->columnCount(); x++)
    {
      QWidget *w = grid->itemAtPosition(y, x)->widget();
      if (!w)
        continue;

      if (y<size && x<size)
        w->show();
      else
        w->hide();
    }
}

void CustomConvolution::apply(QImage &image, const QRect &rect)
{
  int size = cbSize->itemData(cbSize->currentIndex()).toInt();

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
  convolve(image, rect, m);
}
