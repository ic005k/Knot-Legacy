#ifndef TODOALARM_H
#define TODOALARM_H

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
  // QString btnSelStyle = "QToolButton {background-color: blue;color: white;}";
  QString btnSelStyle =
      "QToolButton {background-color: rgb(30, 144, 255);color: "
      "white;border-radius:10px; "
      "border:1px solid gray; } QToolButton:pressed { background-color: "
      "rgb(220,220,230);}";

  void addBtn(int start, int total, int col, QString flag, bool week);

 protected:
  bool eventFilter(QObject *obj, QEvent *evn) override;
 private slots:
  void on_btnOK_clicked();

  void on_btnMinute_clicked();

  void on_btnHour_clicked();

  void on_btnDay_clicked();

  void on_btnMonth_clicked();

  void on_btnYear_clicked();

  void on_btnCancelDT_clicked();

  void on_btnSetDT_clicked();

  void on_btnToday_clicked();

  void on_btnTomorrow_clicked();

  void on_btnNextWeek_clicked();

  void on_chkDaily_clicked();

 private:
  QFont font0;
  void onBtnClick(QToolButton *btn, QString flag);
  QString y, m, d, h, mm;
  void setBtnTitle();
  void onDial(QDial *btn, QString flag);
  void addDial(int min, int max, QString flag);
};

#endif  // TODOALARM_H
