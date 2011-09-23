#include <QFileDialog>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QSignalMapper>
#include <QTime>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "filters.h"
#include "filterwrapper.h"
#include "filters/histogram.h"

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  // Setup toolbar
  ui->toolBar->addAction(tr("Open..."), this, SLOT(showOpenDialog()));
  ui->toolBar->addAction(tr("Save"), this, SLOT(saveFile()));
  ui->toolBar->addAction(tr("Save as..."), this, SLOT(showSaveDialog()));

  actOpen = ui->toolBar->actions()[0];
  actSave = ui->toolBar->actions()[1];
  actSaveAs = ui->toolBar->actions()[2];

  // Prepare dialogs
  dlgOpen = new QFileDialog(this, tr("Select image..."), QString());
  dlgOpen->setNameFilters(QStringList() << tr("Images (*.bmp *.png *.jpg)"));
  dlgOpen->setFileMode(QFileDialog::ExistingFile);

  dlgSave = new QFileDialog(this, tr("Select file..."), QString());
  dlgSave->setNameFilters(QStringList() << tr("Images (*.bmp *.png *.jpg)"));
  dlgSave->setFileMode(QFileDialog::AnyFile);

  // No file opened -> disable unavailable actions
  connect(this, SIGNAL(fileOperationsEnabled(bool)), actSave, SLOT(setEnabled(bool)));
  connect(this, SIGNAL(fileOperationsEnabled(bool)), actSaveAs, SLOT(setEnabled(bool)));
  connect(this, SIGNAL(fileOperationsEnabled(bool)), ui->dockTools, SLOT(setEnabled(bool)));
  connect(this, SIGNAL(fileOperationsEnabled(bool)), ui->dockInfo, SLOT(setEnabled(bool)));
  emit fileOperationsEnabled(false);

  // Setup graphics view
  ui->graphicsView->setScene(new QGraphicsScene(this));
  imageView = new QGraphicsPixmapItem();
  ui->graphicsView->scene()->addItem(imageView);
  ui->graphicsView->scene()->setBackgroundBrush(Qt::black);
  connect(this, SIGNAL(imageUpdated()), SLOT(updateView()));

  // Filters
  QList<IFilter *> ifilters = createFilters(this);
  foreach(IFilter *ifilter, ifilters)
  {
    FilterWrapper *wrapper = new FilterWrapper(ifilter, ui->filtersBox);
    ui->filtersLayout->addWidget(wrapper);
    filters << wrapper;
    connect(wrapper, SIGNAL(activated()), SLOT(filterActivated()));
    connect(wrapper, SIGNAL(apply()), SLOT(filterApply()));
  }
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::showOpenDialog()
{
  if (dlgOpen->exec() == QDialog::Accepted)
  {
    if (loadFile(dlgOpen->selectedFiles()[0]))
      emit fileOperationsEnabled(true);
  }
}

void MainWindow::showSaveDialog()
{
  dlgSave->exec();
}

bool MainWindow::loadFile(const QString &filename)
{
  if (currentImage.load(filename))
  {
    emit imageUpdated();
    return true;
  }
  return false;
}

void MainWindow::saveFile()
{
}

void MainWindow::updateView()
{
  static const int histWidth = 128;
  static const int histHeight = 64;

  imageView->setPixmap(QPixmap::fromImage(currentImage));
  ui->graphicsView->scene()->setSceneRect(imageView->boundingRect()); // Force shrink

  ui->hstLuminance->
      setPixmap(histogram(currentImage, getLuma,
                          histWidth, histHeight,
                          Qt::black, QColor(Qt::white)));
  ui->hstRed->
      setPixmap(histogram(currentImage, getRed,
                          histWidth, histHeight,
                          Qt::black, QColor(Qt::red)));
  ui->hstGreen->
      setPixmap(histogram(currentImage, getGreen,
                          histWidth, histHeight,
                          Qt::black, QColor(Qt::green)));
  ui->hstBlue->
      setPixmap(histogram(currentImage, getBlue,
                          histWidth, histHeight,
                          Qt::black, QColor(Qt::blue)));
}

void MainWindow::filterActivated()
{
  FilterWrapper *thisFilter = qobject_cast<FilterWrapper *>(sender());
  if (!thisFilter)
    return;

  foreach(FilterWrapper *filter, filters)
  {
    if (filter != thisFilter)
      filter->collapse();
  }
}

void MainWindow::filterApply()
{
  FilterWrapper *thisFilter = qobject_cast<FilterWrapper *>(sender());
  if (!thisFilter)
    return;

  IFilter *ifilter = thisFilter->filter();
  ui->statusBar->showMessage(tr("Please wait: applying %1...").arg(ifilter->filterName()));
  QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

  QTime measure;
  measure.start();
  ifilter->apply(currentImage);
  int elapsed = measure.elapsed();

  emit imageUpdated();
  ui->statusBar->showMessage(tr("%1 applied (%2 ms).").arg(ifilter->filterName()).arg(elapsed));
}
