#ifndef DLGLOADPIC_H
#define DLGLOADPIC_H

#include <QDialog>
#include <QKeyEvent>

#include "src/imgview.h"

namespace Ui {
class dlgLoadPic;
}

class dlgLoadPic : public QDialog {
  Q_OBJECT

 public:
  explicit dlgLoadPic(QWidget *parent = nullptr);
  ~dlgLoadPic();
  Ui::dlgLoadPic *ui;

  void loadPic(QString picfile, int k);

  void recvShowPicSignal(QImage image);

  void initMain(QString imgFile);

 protected:
  bool eventFilter(QObject *watch, QEvent *evn) override;

 private slots:
  void on_btnBack_clicked();

  void on_btnZoom_clicked();

  void on_btnReduce_clicked();

  void on_hsZoom_valueChanged(int value);

  void on_hsZoom_sliderReleased();

 private:
  QImageWidget *m_pImage;
  QPoint m_startPos;
  int k = 0;
  void showRatio(double w0, double w1);
  bool isMouseRelease = false;
  bool isMousePress = false;
  bool iMouseMove = false;
  int press_x;
  int press_y;
  int relea_x;
  int relea_y;
};

#endif  // DLGLOADPIC_H
