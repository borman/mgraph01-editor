#include <QFileDialog>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "filters.h"
#include "filterwrapper.h"

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
  QList<IFilter *> filters = createFilters(this);
  foreach(IFilter *filter, filters)
  {
    FilterWrapper *wrapper = new FilterWrapper(filter, this);
    ui->filtersLayout->addWidget(wrapper->createWidget(ui->filtersBox));
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
  imageView->setPixmap(QPixmap::fromImage(currentImage));
  ui->graphicsView->scene()->setSceneRect(imageView->boundingRect()); // Force shrink
}
