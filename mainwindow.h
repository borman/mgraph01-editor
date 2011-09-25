#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}
class QFileDialog;
class QGraphicsPixmapItem;
class FilterWrapper;

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

public slots:
  void showOpenDialog();
  void showSaveDialog();
  bool loadFile(const QString &filename);
  void saveFile();
  void updateView();
  void filterActivated();
  void filterApply();

signals:
  void fileOperationsEnabled(bool);
  void imageUpdated();

private:
  Ui::MainWindow *ui;

  QAction *actOpen;
  QAction *actSave;
  QAction *actSaveAs;

  QFileDialog *dlgOpen;
  QFileDialog *dlgSave;

  QGraphicsPixmapItem *imageView;

  QImage currentImage;
  QString currentFileName;

  QList<FilterWrapper *> filters;
};

#endif // MAINWINDOW_H
