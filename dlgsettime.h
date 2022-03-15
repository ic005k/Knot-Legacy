#ifndef DLGSETTIME_H
#define DLGSETTIME_H

#include <QDialog>

namespace Ui {
class dlgSetTime;
}

class dlgSetTime : public QDialog {
  Q_OBJECT

 public:
  explicit dlgSetTime(QWidget *parent = nullptr);
  ~dlgSetTime();
  Ui::dlgSetTime *ui;

 protected:
  void keyReleaseEvent(QKeyEvent *event);
 private slots:
  void on_btnBack_clicked();

  void on_btnOk_clicked();

 private:
};

#endif  // DLGSETTIME_H
