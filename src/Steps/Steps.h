#ifndef STEPS_H
#define STEPS_H

#include <QAccelerometer>
#include <QDialog>
#include <QGeoCoordinate>
#include <QGeoPositionInfoSource>
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

  int toDayInitSteps = 0;
  double dleInter = 5;
  double dleSlope = 5;
  void saveSteps();
  void init_Steps();
  QString lblStyleLight = "background-color: rgb(25, 239, 21);color:black";

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
  void on_btnBack_clicked();

  void on_btnReset_clicked();

  void on_editTangentLineIntercept_textChanged(const QString &arg1);

  void on_editTangentLineSlope_textChanged(const QString &arg1);

  void setTableData(int index, QString date, int steps, QString km);

  void startRecordMotion();
  void stopRecordMotion();

  bool requestLocationPermissions();
  void loadGpsList(int nYear, int nMonth);
  int getGpsListCount();
  void selGpsListYearMonth();
  void getGpsListDataFromYearMonth();
  void delGpsListItem(int index);
  void curMonthTotal();
  void appendTrack(double lat, double lon);
 public slots:
  void clearAllGpsList();
 private slots:
  void positionUpdated(const QGeoPositionInfo &info);

 private:
  QBrush brush1 = QBrush(QColor(255, 228, 225));
  QBrush brush2 = QBrush(QColor(245, 222, 179));
  QBrush brushMax = QBrush(QColor(245, 222, 79));
  int maxCount = 90;

  double distance() const { return m_distance; }
  QGeoPositionInfoSource *m_positionSource;
  QGeoCoordinate lastPosition;
  double m_distance;
  double m_TotalDistance;
  double m_speed;
  QDateTime m_startTime;
  QTime m_time;
  QTimer *timer;
  QString strTotalDistance;
  QString strDurationTime;
  double latitude = 59.91;
  double longitude = 10.75;
  QString strGpsStatus;
  QString strGpsInfoShow;
  QString lblStyle;
  QString lblStartStyle =
      "QLabel {background-color: #FF0000;color: #ECEFF4;font-family: 'Segoe "
      "UI', sans-serif;font-weight: bold;border: 2px solid "
      "#4C566A;border-radius: 10px;padding: 10px 20px;text-align: "
      "center;box-shadow: 3px 3px 5px rgba(0, 0, 0, 0.2);}";

  QString t0, str1, str2, str3, str4, str5, str6, str7;

  void insertGpsList(int curIndex, QString t0, QString t1, QString t2,
                     QString t3, QString t4, QString t5);
  QString strStartTime, strEndTime;

  QString getGpsListText2(int index);

  void clearTrack();
  void writeGpsPos(double lat, double lon, int i, int count);
  int nWriteGpsCount;
  bool isGpsTest = false;
 signals:
  void distanceChanged(double distance);
  void timeChanged();
  void speedChanged();
};

#endif  // STEPS_H
