#ifndef DLGFLOATFUN_H
#define DLGFLOATFUN_H

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
  protected:
  bool eventFilter(QObject *watch, QEvent *evn) override;
  void closeEvent(QCloseEvent *event) override;
  void paintEvent(QPaintEvent *event) override;
 private slots:
  void on_btnAdd_clicked();

  void on_btnDel_clicked();

 private:
};

#endif  // DLGFLOATFUN_H
