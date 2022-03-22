#ifndef DLGSETTIME_H
#define DLGSETTIME_H

#include <QDialog>
#include <QFont>
#include <QWidgetAction>

namespace Ui {
class dlgSetTime;
}

class dlgSetTime : public QDialog {
  Q_OBJECT

 public:
  explicit dlgSetTime(QWidget *parent = nullptr);
  ~dlgSetTime();
  Ui::dlgSetTime *ui;
  void saveCustomDesc();

 protected:
  void keyReleaseEvent(QKeyEvent *event);
 private slots:
  void on_btnBack_clicked();

  void on_btnOk_clicked();

  void on_btn7_clicked();
  void on_btn8_clicked();
  void on_btn9_clicked();
  void on_btn4_clicked();
  void on_btn5_clicked();
  void on_btn6_clicked();
  void on_btn1_clicked();
  void on_btn2_clicked();
  void on_btn3_clicked();
  void on_btn0_clicked();
  void on_btnDot_clicked();
  void on_btnDel_clicked();

  void on_btnCustom_clicked();

 private:
  void set_Amount(QString Number);
};

#endif  // DLGSETTIME_H
