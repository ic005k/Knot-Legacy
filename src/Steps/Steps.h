#ifndef STEPS_H
#define STEPS_H

#include <QAccelerometer>
#include <QDialog>
#include <QRegularExpressionValidator>

#include "src/Steps/StepsOptions.h"

namespace Ui {
class Steps;
}

class Steps : public QDialog {
  Q_OBJECT

 public:
  explicit Steps(QWidget *parent = nullptr);
  ~Steps();
  Ui::Steps *ui;

  StepsOptions *m_StepsOptions;
  int toDayInitSteps = 0;
  double dleInter = 5;
  double dleSlope = 5;
  void saveSteps();
  void init_Steps();
  QString lblStyleLight = "background-color: rgb(25, 239, 21);color:black";
  QString lblStyleNormal;
  void addRecord(QString, qlonglong, QString);

  qlonglong getCurrentSteps();

  void setTableSteps(qlonglong steps);

  void releaseWakeLock();
  void acquireWakeLock();

  void setMaxMark();

  void appendSteps(QString date, int steps, QString km);
  int getCount();
  QString getDate(int row);
  int getSteps(int row);

  void delItem(int index);
  QString getKM(int row);
  void clearAll();

  void setScrollBarPos(double pos);

 protected:
  void keyReleaseEvent(QKeyEvent *event) override;

  bool eventFilter(QObject *watch, QEvent *evn) override;

 public:
  void on_rbAlg1_clicked();

  void on_rbAlg2_clicked();

  void on_btnBack_clicked();

  void on_btnPause_clicked();

  void on_btnReset_clicked();

  void on_editTangentLineIntercept_textChanged(const QString &arg1);

  void on_editTangentLineSlope_textChanged(const QString &arg1);

  void on_btnDefaultIntercept_clicked();

  void on_btnDefaultSlope_clicked();

  void setTableData(int index, QString date, int steps, QString km);

 private:
  QBrush brush1 = QBrush(QColor(255, 228, 225));
  QBrush brush2 = QBrush(QColor(245, 222, 179));
  QBrush brushMax = QBrush(QColor(245, 222, 79));
  int maxCount = 90;
};

#endif  // STEPS_H
