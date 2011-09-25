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
  if (dlgSave->exec() == QDialog::Accepted)
    currentImage.save(dlgSave->selectedFiles()[0]);
}

bool MainWindow::loadFile(const QString &filename)
{
  if (currentImage.load(filename))
  {
    region->resetSelection();
    emit imageUpdated();
    return true;
  }
  return false;
}

void MainWindow::saveFile()
{
  currentImage.save(currentFileName);
}

void MainWindow::updateView()
{
  static const int histWidth = 128;
  static const int histHeight = 64;

  imageView->setPixmap(QPixmap::fromImage(currentImage));
  region->setArea(imageView->boundingRect());

  ui->graphicsView->scene()->setSceneRect(imageView->boundingRect()); // Force shrink

  ui->hstLuminance->
      setPixmap(drawHistogram(currentImage, getLuma,
                              histWidth, histHeight,
                              Qt::black, Qt::white));
  ui->hstRed->
      setPixmap(drawHistogram(currentImage, getRed,
                              histWidth, histHeight,
                              Qt::black, Qt::red));
  ui->hstGreen->
      setPixmap(drawHistogram(currentImage, getGreen,
                              histWidth, histHeight,
                              Qt::black, Qt::green));
  ui->hstBlue->
      setPixmap(drawHistogram(currentImage, getBlue,
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
  ifilter->apply(currentImage);
  int elapsed = measure.elapsed();

  emit imageUpdated();
  ui->statusBar->showMessage(tr("%1 applied (%2 ms).").arg(ifilter->filterName()).arg(elapsed));
}
