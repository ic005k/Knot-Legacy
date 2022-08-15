#ifndef MSGDIALOG_H
#define MSGDIALOG_H

#include <QDialog>
#include <QHBoxLayout>
#include <QToolButton>

namespace Ui {
class msgDialog;
}

class msgDialog : public QDialog {
  Q_OBJECT

 public:
  explicit msgDialog(QWidget *parent = nullptr);
  ~msgDialog();
  Ui::msgDialog *ui;

  void initDlg();

  void addBtn(int start, int total, int col, QString flag);

 protected:
  bool eventFilter(QObject *obj, QEvent *evn) override;
 private slots:
  void on_btnOK_clicked();

  void on_hsHour_valueChanged(int value);

  void on_btnMinute_clicked();

  void on_btnHour_clicked();

  void on_btnDay_clicked();

  void on_btnMonth_clicked();

  void on_btnYear_clicked();

 private:
  void onBtnClick(QToolButton *btn, QString flag);
  QString y, m, d, h, mm;
};

#endif  // MSGDIALOG_H
