#ifndef PAGEINDICATOR_H
#define PAGEINDICATOR_H

#include <QDialog>
#include <QGraphicsOpacityEffect>

namespace Ui {
class dlgFloatFun;
}

class dlgFloatFun : public QDialog {
  Q_OBJECT

 public:
  explicit dlgFloatFun(QWidget *parent = nullptr);
  ~dlgFloatFun();
  Ui::dlgFloatFun *ui;

  void init();

  void setY(int y);
  void setPicLeft();
  void setPicRight();

 protected:
  bool eventFilter(QObject *watch, QEvent *evn) override;
  void closeEvent(QCloseEvent *event) override;
  void paintEvent(QPaintEvent *event) override;
 private slots:

 private:
};

#endif  // PAGEINDICATOR_H
