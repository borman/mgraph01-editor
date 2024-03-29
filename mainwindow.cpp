#include <QFileDialog>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QSignalMapper>
#include <QTime>

#include "mainwindow.h"
#include "regioneditor.h"
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
  ui->toolBar->addAction(tr("Save as..."), this, SLOT(showSaveDialog()));
  //ui->toolBar->addAction(tr("Save"), this, SLOT(saveFile())); // Dangerous it is.

  actOpen = ui->toolBar->actions()[0];
  actSaveAs = ui->toolBar->actions()[1];
  //actSave = ui->toolBar->actions()[2];

  // Prepare dialogs
  dlgOpen = new QFileDialog(this, tr("Select image..."), QString());
  dlgOpen->setNameFilters(QStringList() << tr("Images (*.bmp *.png *.jpg)"));
  dlgOpen->setFileMode(QFileDialog::ExistingFile);

  dlgSave = new QFileDialog(this, tr("Select file..."), QString());
  dlgSave->setNameFilters(QStringList() << tr("Images (*.bmp *.png *.jpg)"));
  dlgSave->setFileMode(QFileDialog::AnyFile);
  dlgSave->setAcceptMode(QFileDialog::AcceptSave);

  // No file opened -> disable unavailable actions
  //connect(this, SIGNAL(fileOperationsEnabled(bool)), actSave, SLOT(setEnabled(bool)));
  connect(this, SIGNAL(fileOperationsEnabled(bool)), actSaveAs, SLOT(setEnabled(bool)));
  connect(this, SIGNAL(fileOperationsEnabled(bool)), ui->dockTools, SLOT(setEnabled(bool)));
  connect(this, SIGNAL(fileOperationsEnabled(bool)), ui->dockInfo, SLOT(setEnabled(bool)));
  emit fileOperationsEnabled(false);

  // Setup graphics view
  ui->graphicsView->setScene(new QGraphicsScene(ui->graphicsView));

  imageView = new QGraphicsPixmapItem();
  ui->graphicsView->scene()->addItem(imageView);

  region = new RegionEditor(imageView->boundingRect());
  ui->graphicsView->scene()->addItem(region);
  connect(ui->btnResetMask, SIGNAL(clicked()), region, SLOT(resetSelection()));
  connect(ui->chkShowMask, SIGNAL(toggled(bool)), region, SLOT(setShowMask(bool)));

  connect(this, SIGNAL(imageUpdated()), SLOT(updateView()));

  // Filters
  QList<IFilter *> ifilters = createFilters(this);
  foreach(IFilter *ifilter, ifilters)
  {
    if (ifilter)
    {
      FilterWrapper *wrapper = new FilterWrapper(ifilter, ui->filtersBox);
      ui->filtersLayout->addWidget(wrapper);
      filters << wrapper;
      connect(wrapper, SIGNAL(activated()), SLOT(filterActivated()));
      connect(wrapper, SIGNAL(apply()), SLOT(filterApply()));
    }
    else
      ui->filtersLayout->addSpacing(20);
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
  if (dlgSave->exec() == QDialog::Accepted)
    saveFile(dlgSave->selectedFiles()[0]);
}

bool MainWindow::loadFile(const QString &filename)
{
  if (currentImage.load(filename))
  {
    currentFileName = filename;
    if (currentImage.format() != QImage::Format_ARGB32)
      currentImage = currentImage.convertToFormat(QImage::Format_ARGB32);
    region->resetSelection();
    emit imageUpdated();
    ui->statusBar->showMessage(tr("Image %1 loaded successfully.").arg(filename));
    return true;
  }
  else
  {
    ui->statusBar->showMessage(tr("Loading %1 failed.").arg(filename));
    return false;
  }
}

void MainWindow::saveFile()
{
  saveFile(currentFileName);
}

void MainWindow::saveFile(const QString &filename)
{
  if (currentImage.save(filename))
  {
    ui->statusBar->showMessage(tr("Image successfully saved to %1.").arg(filename));
    currentFileName = filename;
  }
  else
    ui->statusBar->showMessage(tr("Saving to %1 failed.").arg(filename));
}

void MainWindow::updateView()
{
  static const int histWidth = 128;
  static const int histHeight = 64;

  imageView->setPixmap(QPixmap::fromImage(currentImage));
  region->setArea(imageView->boundingRect());

  ui->graphicsView->scene()->setSceneRect(imageView->boundingRect()); // Force shrink

  QRect full(QPoint(0, 0), currentImage.size());
  ui->hstLuminance->
      setPixmap(drawHistogram(currentImage, full, getLuma,
                              histWidth, histHeight,
                              Qt::black, Qt::white));
  ui->hstRed->
      setPixmap(drawHistogram(currentImage, full, getRed,
                              histWidth, histHeight,
                              Qt::black, Qt::red));
  ui->hstGreen->
      setPixmap(drawHistogram(currentImage, full, getGreen,
                              histWidth, histHeight,
                              Qt::black, Qt::green));
  ui->hstBlue->
      setPixmap(drawHistogram(currentImage, full, getBlue,
                              histWidth, histHeight,
                              Qt::black, Qt::blue));
}

void MainWindow::filterActivated()
{
  // Exclusive filter selection. Do we need it?
#if 0
  FilterWrapper *thisFilter = qobject_cast<FilterWrapper *>(sender());
  if (!thisFilter)
    return;

  foreach(FilterWrapper *filter, filters)
  {
    if (filter != thisFilter)
      filter->collapse();
  }
#endif
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
  ifilter->apply(currentImage, region->selection().toRect());
  int elapsed = measure.elapsed();

  emit imageUpdated();
  ui->statusBar->showMessage(tr("%1 applied (%2 ms).").arg(ifilter->filterName()).arg(elapsed));
}
