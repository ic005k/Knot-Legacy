#ifndef MSGDIALOG_H
#define MSGDIALOG_H

#include <QDial>
#include <QDialog>
#include <QHBoxLayout>
#include <QObjectList>
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
  QString btnSelStyle = "QToolButton {background-color: blue;color: white;}";
  QString btnNorStyle;
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

  void on_btnCancelDT_clicked();

  void on_btnSetDT_clicked();

 private:
  void onBtnClick(QToolButton *btn, QString flag);
  QString y, m, d, h, mm;
  void setBtnTitle();
  void onDial(QDial *btn, QString flag);
  void addDial(int min, int max, QString flag);
};

#endif  // MSGDIALOG_H
