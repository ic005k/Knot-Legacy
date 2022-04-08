#ifndef DLGSTEPS_H
#define DLGSTEPS_H

#include <QAccelerometer>
#include <QDialog>
#include <QRegularExpressionValidator>

namespace Ui {
class dlgSteps;
}

class dlgSteps : public QDialog {
  Q_OBJECT

 public:
  explicit dlgSteps(QWidget *parent = nullptr);
  ~dlgSteps();
  Ui::dlgSteps *ui;

  int toDayInitSteps = 0;
  double dleInter = 9.55;
  double dleSlope = 9.55;
  void saveSteps();
  void init_Steps();
  QString lblStyleLight = "background-color: rgb(25, 239, 21);color:black";
  QString lblStyleNormal;
  void addRecord(QString, qlonglong);

  qlonglong getCurrentSteps();

  void setTableSteps(qlonglong steps);

 protected:
  void keyReleaseEvent(QKeyEvent *event) override;

  bool eventFilter(QObject *watch, QEvent *evn) override;
 private slots:
  void on_btnBack_clicked();

  void on_btnPause_clicked();

  void on_btnReset_clicked();

  void on_editTangentLineIntercept_textChanged(const QString &arg1);

  void on_editTangentLineSlope_textChanged(const QString &arg1);

  void on_btnDefaultIntercept_clicked();

  void on_btnDefaultSlope_clicked();

  void on_rbAlg1_clicked();

  void on_rbAlg2_clicked();

 private:
};

#endif  // DLGSTEPS_H
