#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}
class QFileDialog;
class QGraphicsPixmapItem;

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
  void setRotateMode(bool enabled);
  void setScaleMode(bool enabled);

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
};

#endif // MAINWINDOW_H
