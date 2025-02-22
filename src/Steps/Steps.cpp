#include "Steps.h"

#include <QVector>
#include <cmath>

#include "src/MainWindow.h"
#include "ui_MainWindow.h"
#include "ui_StepsOptions.h"

extern MainWindow* mw_one;
extern Method* m_Method;
extern QRegularExpression regxNumber;
extern QList<float> rlistX, rlistY, rlistZ, glistX, glistY, glistZ;
extern unsigned int num_steps_walk, num_steps_run, num_steps_hop;
extern bool loading, isAndroid;
extern QString iniFile, iniDir;
extern void setTableNoItemFlags(QTableWidget* t, int row);

struct GPSCoordinate {
  double latitude;
  double longitude;
};
QVector<GPSCoordinate> gaussianFilter(const QVector<GPSCoordinate>& rawData,
                                      int windowSize, double sigma);
QVector<GPSCoordinate> detectAndCorrectOutliers(
    const QVector<GPSCoordinate>& data, double threshold);

#ifdef Q_OS_ANDROID
QAndroidJniObject listenerWrapper;
QAndroidJniObject m_activity;
#endif

Steps::Steps(QWidget* parent) : QDialog(parent) {
  this->installEventFilter(this);

  mw_one->ui->lblSingle->adjustSize();
  QString date = QString::number(QDate::currentDate().month()) + "-" +
                 QString::number(QDate::currentDate().day());
  mw_one->ui->lblCurrent->setText(date + " " + QTime::currentTime().toString());

  QFont font0 = m_Method->getNewFont(15);
  mw_one->ui->lblSteps->setFont(font0);

  font0.setPointSize(13);
  mw_one->ui->lblGpsDateTime->setFont(font0);
  font0.setBold(true);
  mw_one->ui->lblCurrent->setFont(font0);
  mw_one->ui->lblToNow->setFont(font0);
  mw_one->ui->lblNow->setFont(font0);
  mw_one->ui->lblTitle1->setFont(font0);
  mw_one->ui->lblTitle2->setFont(font0);
  mw_one->ui->lblTitle3->setFont(font0);
  mw_one->ui->lblTitle4->setFont(font0);

  QFont font1 = m_Method->getNewFont(17);
  font1.setBold(true);
  mw_one->ui->lblKM->setFont(font1);
  mw_one->ui->lblSingle->setFont(font1);

  lblStyle = mw_one->ui->lblTotalDistance->styleSheet();
  mw_one->ui->lblCurrentDistance->setStyleSheet(lblStyle);
  mw_one->ui->lblRunTime->setStyleSheet(lblStyle);
  mw_one->ui->lblAverageSpeed->setStyleSheet(lblStyle);
  mw_one->ui->lblGpsInfo->setStyleSheet(lblStyle);
  mw_one->ui->lblMonthTotal->setStyleSheet(lblStyle);
  mw_one->ui->btnGetGpsListData->hide();
}

Steps::~Steps() {}

void Steps::keyReleaseEvent(QKeyEvent* event) { Q_UNUSED(event) }

bool Steps::eventFilter(QObject* watch, QEvent* evn) {
  if (evn->type() == QEvent::KeyRelease) {
    QKeyEvent* keyEvent = static_cast<QKeyEvent*>(evn);
    if (keyEvent->key() == Qt::Key_Back) {
      on_btnBack_clicked();
      return true;
    }
  }

  return QWidget::eventFilter(watch, evn);
}

void Steps::on_btnBack_clicked() {
  saveSteps();
  mw_one->ui->frameSteps->hide();
  mw_one->ui->frameMain->show();
}

void Steps::on_btnReset_clicked() {
  mw_one->accel_pedometer->resetStepCount();

  mw_one->CurrentSteps = 0;
  mw_one->ui->lblSingle->setText("0");
  toDayInitSteps = getCurrentSteps();
  if (mw_one->isHardStepSensor == 1) mw_one->resetSteps = mw_one->tc;

  QString date = QString::number(QDate::currentDate().month()) + "-" +
                 QString::number(QDate::currentDate().day());
  mw_one->ui->lblCurrent->setText(date + " " + QTime::currentTime().toString());
  mw_one->ui->lblNow->setText(date + " " + QTime::currentTime().toString());
  mw_one->ui->lblKM->setText("0.00  " + tr("KM"));
}

void Steps::saveSteps() {
  QSettings Reg(iniDir + "steps.ini", QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg.setIniCodec("utf-8");
#endif

  if (getCount() > maxCount) {
    delItem(0);
  }
  int count = getCount();
  if (count > 0) {
    Reg.setValue("/Steps/Count", count);
  }
  for (int i = 0; i < count; i++) {
    Reg.setValue("/Steps/Table-" + QString::number(i) + "-0", getDate(i));
    Reg.setValue("/Steps/Table-" + QString::number(i) + "-1", getSteps(i));
    Reg.setValue("/Steps/Table-" + QString::number(i) + "-2", getKM(i));
  }

  QSettings Reg1(iniDir + "initsteps.ini", QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg1.setIniCodec("utf-8");
#endif
  Reg1.setValue("TodaySteps", getCurrentSteps());
}

void Steps::init_Steps() {
  clearAll();

  QString ini_file;
  ini_file = iniDir + "steps.ini";
  QSettings Reg(ini_file, QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg.setIniCodec("utf-8");
#endif

  mw_one->m_StepsOptions->ui->editStepLength->setText(
      Reg.value("/Steps/Length", "35").toString());
  mw_one->m_StepsOptions->ui->editStepsThreshold->setText(
      Reg.value("/Steps/Threshold", "10000").toString());

  int count = Reg.value("/Steps/Count").toInt();
  int start = 0;
  if (count > maxCount) start = 1;

  for (int i = start; i < count; i++) {
    QString str0 =
        Reg.value("/Steps/Table-" + QString::number(i) + "-0").toString();
    qlonglong steps =
        Reg.value("/Steps/Table-" + QString::number(i) + "-1").toLongLong();
    QString str2 =
        Reg.value("/Steps/Table-" + QString::number(i) + "-2").toString();
    if (str2 == "") {
      double km = mw_one->m_StepsOptions->ui->editStepLength->text()
                      .trimmed()
                      .toDouble() *
                  steps / 100 / 1000;
      str2 = QString("%1").arg(km, 0, 'f', 2);
    }

    addRecord(str0, steps, str2);
  }

  for (int i = start; i < count; i++) {
    QString date = getDate(i);
    if (QDate::currentDate().toString("ddd MM dd ") == date) {
      toDayInitSteps = getSteps(i);
      break;
    }
  }
}

void Steps::on_editTangentLineIntercept_textChanged(const QString& arg1) {
  mw_one->accel_pedometer->setTangentLineIntercept(arg1.toFloat());
}

void Steps::on_editTangentLineSlope_textChanged(const QString& arg1) {
  mw_one->accel_pedometer->setTangentLineSlope(arg1.toFloat());
}

void Steps::addRecord(QString date, qlonglong steps, QString km) {
  QString str0;
  QString strD0 = date;
  int m = strD0.split(" ").at(1).toInt();
  if (m == 0) {
    QDate::fromString(strD0, "ddd MMM d yyyy").toString("ddd MM dd yyyy");
  } else
    str0 = strD0;
  date = str0;
  QString str1 = QString::number(QDate::currentDate().year());
  date.replace(str1, "");

  bool isYes = false;

  int count = getCount();
  for (int i = 0; i < count; i++) {
    QString str = getDate(i);
    if (mw_one->getYMD(str) == mw_one->getYMD(date)) {
      appendSteps(date, steps, km);

      isYes = true;
      break;
    }
  }

  if (!isYes) {
    appendSteps(date, steps, km);
  }
}

qlonglong Steps::getCurrentSteps() {
  int count = getCount();
  if (count == 0) return 0;

  QString str = getDate(count - 1);
  if (str == QDate::currentDate().toString("ddd MM dd "))
    return getSteps(count - 1);
  return 0;
}

void Steps::setTableSteps(qlonglong steps) {
  // int count = getCount();

  QSettings Reg(iniDir + "steps.ini", QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg.setIniCodec("utf-8");
#endif
  int count = Reg.value("/Steps/Count", 0).toInt();

  if (count > 0) {
    QString date;

    date = Reg.value("/Steps/Table-" + QString::number(count - 1) + "-0")
               .toString();

    if (date == QDate::currentDate().toString("ddd MM dd ")) {
      double km = mw_one->m_StepsOptions->ui->editStepLength->text()
                      .trimmed()
                      .toDouble() *
                  steps / 100 / 1000;
      QString strKM = QString("%1").arg(km, 0, 'f', 2);

      Reg.setValue("/Steps/Table-" + QString::number(count - 1) + "-0", date);
      Reg.setValue("/Steps/Table-" + QString::number(count - 1) + "-1", steps);
      Reg.setValue("/Steps/Table-" + QString::number(count - 1) + "-2", strKM);

    } else {
      count = count + 1;
      Reg.setValue("/Steps/Table-" + QString::number(count - 1) + "-0",
                   QDate::currentDate().toString("ddd MM dd "));
      Reg.setValue("/Steps/Table-" + QString::number(count - 1) + "-1", 0);
      Reg.setValue("/Steps/Table-" + QString::number(count - 1) + "-2", "0");

      Reg.setValue("/Steps/Count", count);
    }
  } else {
    count = count + 1;
    Reg.setValue("/Steps/Table-" + QString::number(count - 1) + "-0",
                 QDate::currentDate().toString("ddd MM dd "));
    Reg.setValue("/Steps/Table-" + QString::number(count - 1) + "-1", 0);
    Reg.setValue("/Steps/Table-" + QString::number(count - 1) + "-2", "0");

    Reg.setValue("/Steps/Count", count);
  }
}

void Steps::releaseWakeLock() {
#ifdef Q_OS_ANDROID
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  QAndroidJniObject jo = QAndroidJniObject::fromString("releaseWakeLock");
  jo.callStaticMethod<void>("com.x/MyActivity", "releaseWakeLock", "()V");
#else
  QJniObject jo = QJniObject::fromString("releaseWakeLock");
  jo.callStaticMethod<void>("com.x/MyActivity", "releaseWakeLock", "()V");
#endif
#endif
}

void Steps::acquireWakeLock() {
#ifdef Q_OS_ANDROID
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  QAndroidJniObject m_activity = QtAndroid::androidActivity();
  m_activity.callMethod<void>("acquireWakeLock");
#else
  QJniObject m_activity = QNativeInterface::QAndroidApplication::context();
  m_activity.callMethod<void>("acquireWakeLock");
#endif
#endif
}

void Steps::setMaxMark() {
  if (getCount() > 1) {
    QList<int> list;

    for (int i = 0; i < getCount(); i++) {
      list.append(getSteps(i));
    }

    int maxValue = *std::max_element(list.begin(), list.end());
    for (int i = 0; i < list.count(); i++) {
      if (maxValue == list.at(i)) {
        // max value
        break;
      }
    }
  }
}

void Steps::appendSteps(QString date, int steps, QString km) {
  QString strSteps = QString::number(steps);
  double dCalorie = steps * 0.04;
  QString strCalorie =
      QString("%1").arg(dCalorie, 0, 'f', 2) + "  " + tr("Calorie");

  double d_km =
      mw_one->m_StepsOptions->ui->editStepLength->text().trimmed().toDouble() *
      steps / 100 / 1000;
  km = QString("%1").arg(d_km, 0, 'f', 2) + "  " + tr("KM");

  QQuickItem* root = mw_one->ui->qwSteps->rootObject();
  QMetaObject::invokeMethod((QObject*)root, "addItem", Q_ARG(QVariant, date),
                            Q_ARG(QVariant, strSteps), Q_ARG(QVariant, km),
                            Q_ARG(QVariant, strCalorie), Q_ARG(QVariant, 0));
}

int Steps::getCount() {
  QQuickItem* root = mw_one->ui->qwSteps->rootObject();
  QVariant itemCount;
  QMetaObject::invokeMethod((QObject*)root, "getItemCount",
                            Q_RETURN_ARG(QVariant, itemCount));
  return itemCount.toInt();
}

QString Steps::getDate(int row) {
  QQuickItem* root = mw_one->ui->qwSteps->rootObject();
  QVariant item;
  QMetaObject::invokeMethod((QObject*)root, "getText0",
                            Q_RETURN_ARG(QVariant, item), Q_ARG(QVariant, row));
  return item.toString();
}

int Steps::getSteps(int row) {
  QQuickItem* root = mw_one->ui->qwSteps->rootObject();
  QVariant item;
  QMetaObject::invokeMethod((QObject*)root, "getText1",
                            Q_RETURN_ARG(QVariant, item), Q_ARG(QVariant, row));
  return item.toInt();
}

QString Steps::getKM(int row) {
  QQuickItem* root = mw_one->ui->qwSteps->rootObject();
  QVariant item;
  QMetaObject::invokeMethod((QObject*)root, "getText2",
                            Q_RETURN_ARG(QVariant, item), Q_ARG(QVariant, row));
  return item.toString();
}

void Steps::delItem(int index) {
  QQuickItem* root = mw_one->ui->qwSteps->rootObject();
  QMetaObject::invokeMethod((QObject*)root, "delItem", Q_ARG(QVariant, index));
}

void Steps::setTableData(int index, QString date, int steps, QString km) {
  QQuickItem* root = mw_one->ui->qwSteps->rootObject();
  QMetaObject::invokeMethod((QObject*)root, "modifyItem",
                            Q_ARG(QVariant, index), Q_ARG(QVariant, date),
                            Q_ARG(QVariant, QString::number(steps)),
                            Q_ARG(QVariant, km));
}

void Steps::clearAll() {
  int count = getCount();
  for (int i = 0; i < count; i++) delItem(0);
}

void Steps::setScrollBarPos(double pos) {
  QQuickItem* root = mw_one->ui->qwSteps->rootObject();
  QMetaObject::invokeMethod((QObject*)root, "setScrollBarPos",
                            Q_ARG(QVariant, pos));
}

void Steps::startRecordMotion() {
  QSettings Reg(iniDir + "gpslist.ini", QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg.setIniCodec("utf-8");
#endif
  m_TotalDistance = Reg.value("/GPS/TotalDistance", 0).toDouble();

#ifdef Q_OS_ANDROID
#else
  isGpsTest = true;

  m_positionSource = QGeoPositionInfoSource::createDefaultSource(this);
  if (m_positionSource) {
    connect(m_positionSource, &QGeoPositionInfoSource::positionUpdated, this,
            &Steps::positionUpdated);
    m_positionSource->setUpdateInterval(2000);
  } else {
    mw_one->ui->lblGpsInfo->setText(tr("No GPS signal..."));
    mw_one->ui->btnGPS->setText(tr("Start"));
    return;
  }
#endif

  timer = new QTimer(this);
  connect(timer, &QTimer::timeout, this, [this]() {
    m_time = m_time.addSecs(1);

#ifdef Q_OS_ANDROID

    // 获取当前运动距离
    jdouble distance;
    if (nGpsMethod == 1)
      distance = m_activity.callMethod<jdouble>("getTotalDistance", "()D");
    else
      listenerWrapper.callMethod<jdouble>("getTotalDistance", "()D");

    QString str_distance = QString::number(distance, 'f', 2);
    m_distance = str_distance.toDouble();

    if (!isGpsTest) {
      if (nGpsMethod == 1) {
        latitude = m_activity.callMethod<jdouble>("getLatitude", "()D");
        longitude = m_activity.callMethod<jdouble>("getLongitude", "()D");
      } else {
        latitude = listenerWrapper.callMethod<jdouble>("getLatitude", "()D");
        longitude = listenerWrapper.callMethod<jdouble>("getLongitude", "()D");
      }

      latitude = QString::number(latitude, 'f', 6).toDouble();
      longitude = QString::number(longitude, 'f', 6).toDouble();
    }

    QAndroidJniObject jstrGpsStatus;
    if (nGpsMethod == 1)
      jstrGpsStatus = m_activity.callObjectMethod<jstring>("getGpsStatus");
    else
      jstrGpsStatus = listenerWrapper.callObjectMethod<jstring>("getGpsStatus");

    if (jstrGpsStatus.isValid()) {
      strGpsStatus = jstrGpsStatus.toString();
      QStringList list = strGpsStatus.split("\n");
      if (list.count() > 2) {
        str1 = list.at(0);
        str2 = list.at(1);
        str3 = list.at(2);
        mw_one->ui->lblCurrentDistance->setText(str1);
        mw_one->ui->lblRunTime->setText(str2);
        mw_one->ui->lblAverageSpeed->setText(str3);
        str4 = list.at(3);
        str5 = list.at(4);
        str6 = list.at(5);
        str7 = list.at(6);
        strGpsStatus = str4 + "\n" + str5 + "\n" + str6 + "\n" + str7;
      }

      if (m_time.second() % 3 == 0) {
        if (!isGpsTest) {
          jdouble m_speed;
          if (nGpsMethod == 1)
            m_speed = m_activity.callMethod<jdouble>("getMySpeed", "()D");
          else
            m_speed = listenerWrapper.callMethod<jdouble>("getMySpeed", "()D");

          mySpeed = m_speed;
          if (m_distance > 0 & mySpeed > 0) {
            if (latitude + longitude != oldLat + oldLon) {
              appendTrack(latitude, longitude);
              nWriteGpsCount++;
              writeGpsPos(latitude, longitude, nWriteGpsCount, nWriteGpsCount);
              oldLat = latitude;
              oldLon = longitude;
            }
          }
        } else {
          appendTrack(latitude, longitude);
          latitude = latitude + 0.001;
          longitude = longitude + 0.001;
          nWriteGpsCount++;
          writeGpsPos(latitude, longitude, nWriteGpsCount, nWriteGpsCount);
        }

        mw_one->ui->qwMap->rootContext()->setContextProperty("strDistance",
                                                             str1);
        mw_one->ui->qwMap->rootContext()->setContextProperty("strSpeed", str3);
      }
    }

#else
        if (m_time.second() != 0) {
          m_speed = m_distance / m_time.second();
          emit speedChanged();
        }

        if (isGpsTest) {

            if (m_time.second() % 3==0) {
                appendTrack(latitude, longitude);
                latitude = latitude + 0.001;
                longitude = longitude + 0.001;
                nWriteGpsCount++;
                writeGpsPos(latitude, longitude, nWriteGpsCount, nWriteGpsCount);

                qDebug()<<"m_time%3="<<m_time.second();
            }

            qDebug()<<"m_time="<< m_time.second();
        }

#endif

    strTotalDistance = QString::number(m_TotalDistance) + " km";
    mw_one->ui->lblTotalDistance->setText(strTotalDistance);
    strDurationTime = tr("Duration") + " : " + m_time.toString("hh:mm:ss");
    strGpsInfoShow = strDurationTime +
                     "\nLon.-Lat.: " + QString::number(longitude) + " - " +
                     QString::number(latitude) + "\n" + strGpsStatus;
    mw_one->ui->lblGpsInfo->setText(strGpsInfoShow);
    emit timeChanged();
  });

#ifdef Q_OS_ANDROID

  m_activity = QtAndroid::androidActivity();

  if (m_activity.isValid()) {
    if (nGpsMethod == 1) {
      if (m_activity.callMethod<jdouble>("startGpsUpdates", "()D") == 0) {
        qWarning() << "LocationManager is null";
        mw_one->ui->lblGpsInfo->setText("LocationManager is null...");
        mw_one->ui->btnGPS->setText(tr("Start"));
        return;
      }
    }

    if (nGpsMethod == 2) {
      listenerWrapper = QAndroidJniObject("com/x/LocationListenerWrapper",
                                          "(Landroid/content/Context;)V",
                                          m_activity.object<jobject>());
      if (listenerWrapper.isValid()) {
        if (listenerWrapper.callMethod<jdouble>("startGpsUpdates", "()D") ==
            0) {
          qWarning() << "LocationManager is null";
          mw_one->ui->lblGpsInfo->setText("LocationManager is null...");
          mw_one->ui->btnGPS->setText(tr("Start"));
          return;
        }
      }
    }
  }

#else
  if (m_positionSource) {
    m_positionSource->startUpdates();
  }
#endif

  clearTrack();
  nWriteGpsCount = 0;
  m_time.setHMS(0, 0, 0, 0);
  timer->start(1000);
  m_distance = 0;
  m_speed = 0;
  emit distanceChanged(m_distance);
  emit timeChanged();

  mw_one->ui->btnGPS->setText(tr("Stop"));
  mw_one->ui->tabMotion->setCurrentIndex(1);

  mw_one->ui->lblRunTime->setStyleSheet(lblStartStyle);
  mw_one->ui->lblAverageSpeed->setStyleSheet(lblStartStyle);
  mw_one->ui->lblCurrentDistance->setStyleSheet(lblStartStyle);

  strStartTime = QTime::currentTime().toString();
  t0 = QDate::currentDate().toString();
  mw_one->ui->lblGpsDateTime->setText(t0 + " " + strStartTime);
  mw_one->ui->qwMap->rootContext()->setContextProperty("isGpsRun", true);
}

void Steps::positionUpdated(const QGeoPositionInfo& info) {
  if (lastPosition.isValid()) {
    double b = 1000;
    // Convert to km
    m_distance += (double)lastPosition.distanceTo(info.coordinate()) / b;
    emit distanceChanged(m_distance);
  }
  lastPosition = info.coordinate();

  if (!isGpsTest) {
    latitude = lastPosition.latitude();
    longitude = lastPosition.longitude();
  }
}

void Steps::stopRecordMotion() {
  timer->stop();
  m_TotalDistance = m_TotalDistance + m_distance;
  strTotalDistance = QString::number(m_TotalDistance) + " km";
  mw_one->ui->lblTotalDistance->setText(strTotalDistance);
  mw_one->ui->lblGpsInfo->setText(strGpsInfoShow);

  mw_one->ui->lblRunTime->setStyleSheet(lblStyle);
  mw_one->ui->lblAverageSpeed->setStyleSheet(lblStyle);
  mw_one->ui->lblCurrentDistance->setStyleSheet(lblStyle);

  QSettings Reg(iniDir + "gpslist.ini", QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg.setIniCodec("utf-8");
#endif
  Reg.setValue("/GPS/TotalDistance", m_TotalDistance);

  strEndTime = QTime::currentTime().toString();
  QString t1, t2, t3, t4, t5;

  t1 = tr("Time") + ": " + strStartTime + " - " + strEndTime;
  t2 = tr("Distance") + ": " + str1;
  t3 = tr("Exercise Duration") + ": " + str2;
  t4 = tr("Average Speed") + ": " + str3;
  t5 = str6;

  if (m_distance > 0 || isGpsTest) {
    int nYear = QDate::currentDate().year();
    int nMonth = QDate::currentDate().month();
    QString stry, strm;
    stry = QString::number(nYear);
    strm = QString::number(nMonth);
    QString strTitle = stry + " - " + strm;
    if (mw_one->ui->btnSelGpsDate->text() != strTitle) {
      clearAllGpsList();
      loadGpsList(nYear, nMonth);
      mw_one->ui->btnSelGpsDate->setText(strTitle);
    }

    insertGpsList(0, t0, t1, t2, t3, t4, t5);

    QSettings Reg1(iniDir + stry + "-gpslist.ini", QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    Reg1.setIniCodec("utf-8");
#endif
    int count = getGpsListCount();
    QString strYearMonth = stry + "-" + strm;
    Reg1.setValue("/" + strYearMonth + "/Count", count);
    Reg1.setValue(
        "/" + strYearMonth + "/" + QString::number(count),
        t0 + "-=-" + t1 + "-=-" + t2 + "-=-" + t3 + "-=-" + t4 + "-=-" + t5);

    double t = 0;
    for (int i = 0; i < count; i++) {
      QString str = getGpsListText2(i);
      QStringList list = str.split(" ");
      if (list.count() == 3) {
        QString str1 = list.at(1);
        double jl = str1.toDouble();
        t = t + jl;
      }
    }

    Reg1.setValue("/" + stry + "/" + strm,
                  QString::number(t) + "-=-" + QString::number(count));

    curMonthTotal();
  }

#ifdef Q_OS_ANDROID
  if (nGpsMethod == 1)
    m_distance = m_activity.callMethod<jdouble>("stopGpsUpdates", "()D");
  else
    m_distance = listenerWrapper.callMethod<jdouble>("stopGpsUpdates", "()D");
#else
  if (m_positionSource) {
    m_positionSource->stopUpdates();
  }
  delete m_positionSource;
#endif

  mw_one->ui->qwMap->rootContext()->setContextProperty("isGpsRun", false);

  ShowMessage* msg = new ShowMessage(this);
  msg->showMsg("Knot", mw_one->ui->lblGpsInfo->text(), 1);
}

bool Steps::requestLocationPermissions() {
#ifdef Q_OS_ANDROID
  const QStringList permissions = {"android.permission.ACCESS_FINE_LOCATION",
                                   "android.permission.ACCESS_COARSE_LOCATION"};

  for (const QString& permission : permissions) {
    auto result = QtAndroid::checkPermission(permission);
    if (result == QtAndroid::PermissionResult::Denied) {
      auto resultHash =
          QtAndroid::requestPermissionsSync(QStringList() << permission);
      if (resultHash[permission] != QtAndroid::PermissionResult::Granted) {
        qDebug() << "Permission" << permission << "denied";
        return false;
      }
    }
  }
  return true;
#endif
  return false;
}

void Steps::insertGpsList(int curIndex, QString t0, QString t1, QString t2,
                          QString t3, QString t4, QString t5) {
  QQuickItem* root = mw_one->ui->qwGpsList->rootObject();
  QMetaObject::invokeMethod((QObject*)root, "insertItem",
                            Q_ARG(QVariant, curIndex), Q_ARG(QVariant, t0),
                            Q_ARG(QVariant, t1), Q_ARG(QVariant, t2),
                            Q_ARG(QVariant, t3), Q_ARG(QVariant, t4),
                            Q_ARG(QVariant, t5), Q_ARG(QVariant, 0));
}

int Steps::getGpsListCount() {
  QQuickItem* root = mw_one->ui->qwGpsList->rootObject();
  QVariant itemCount;
  QMetaObject::invokeMethod((QObject*)root, "getItemCount",
                            Q_RETURN_ARG(QVariant, itemCount));
  return itemCount.toInt();
}

void Steps::delGpsListItem(int index) {
  QQuickItem* root = mw_one->ui->qwGpsList->rootObject();
  QMetaObject::invokeMethod((QObject*)root, "delItem", Q_ARG(QVariant, index));
}

void Steps::clearAllGpsList() {
  int count = getGpsListCount();
  for (int i = 0; i < count; i++) {
    delGpsListItem(0);
  }
}

void Steps::loadGpsList(int nYear, int nMonth) {
  mw_one->ui->btnSelGpsDate->setText(QString::number(nYear) + " - " +
                                     QString::number(nMonth));

  QSettings Reg(iniDir + QString::number(nYear) + "-gpslist.ini",
                QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg.setIniCodec("utf-8");
#endif

  QString strYearMonth = QString::number(nYear) + "-" + QString::number(nMonth);
  int count = Reg.value("/" + strYearMonth + "/Count", 0).toInt();
  for (int i = 0; i < count; i++) {
    QString str =
        Reg.value("/" + strYearMonth + "/" + QString::number(i + 1), "")
            .toString();
    QStringList list = str.split("-=-");
    QString t0, t1, t2, t3, t4, t5;
    if (list.count() > 0) {
      t0 = list.at(0);
      t1 = list.at(1);
      t2 = list.at(2);
      t3 = list.at(3);
      t4 = list.at(4);
      t5 = list.at(5);
    }

    insertGpsList(0, t0, t1, t2, t3, t4, t5);
  }
}

void Steps::selGpsListYearMonth() {
  if (isAndroid) {
    int y, m;
    QString str = mw_one->ui->btnSelGpsDate->text();
    QStringList list = str.split("-");
    y = list.at(0).toInt();
    m = list.at(1).toInt();
    m_Method->setDateTimePickerFlag("ym", y, m, 0, 0, 0, "gpslist");
    m_Method->openDateTimePicker();
    return;
  }
}

void Steps::getGpsListDataFromYearMonth() {
  clearAllGpsList();

  QStringList list = m_Method->getDateTimePickerValue();
  int y = list.at(0).toInt();
  int m = list.at(1).toInt();

  loadGpsList(y, m);
  curMonthTotal();
}

QString Steps::getGpsListText2(int index) {
  QQuickItem* root = mw_one->ui->qwGpsList->rootObject();
  QVariant item;
  QMetaObject::invokeMethod((QObject*)root, "getText2",
                            Q_RETURN_ARG(QVariant, item),
                            Q_ARG(QVariant, index));
  return item.toString();
}

void Steps::curMonthTotal() {
  QString title = mw_one->ui->btnSelGpsDate->text();
  QStringList list = title.split("-");
  QString stry = list.at(0);
  stry = stry.trimmed();
  QString strm = list.at(1);
  strm = strm.trimmed();

  QSettings Reg(iniDir + stry + "-gpslist.ini", QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg.setIniCodec("utf-8");
#endif

  double currentMonthTotal;
  int curCount;
  double yt = 0;
  int ycount = 0;
  for (int i = 0; i < 12; i++) {
    double mt = 0;
    int mcount = 0;
    QString str_mt =
        Reg.value("/" + stry + "/" + QString::number(i + 1), 0).toString();
    QStringList list = str_mt.split("-=-");
    if (list.count() == 2) {
      mt = list.at(0).toDouble();
      mcount = list.at(1).toInt();

    } else {
      mt = str_mt.toDouble();
      mcount = 0;
    }
    yt = yt + mt;
    ycount = ycount + mcount;

    if (QString::number(i + 1) == strm) {
      currentMonthTotal = mt;
      curCount = mcount;
    }
  }

  QSettings Reg1(iniDir + "gpslist.ini", QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg1.setIniCodec("utf-8");
#endif
  double m_td = Reg1.value("/GPS/TotalDistance", 0).toDouble();

  mw_one->ui->lblMonthTotal->setText(
      stry + ": " + QString::number(yt) + " km  " + QString::number(ycount) +
      "\n" + strm + ": " + QString::number(currentMonthTotal) + " km  " +
      QString::number(curCount) + "\n" + tr("All Total") + ": " +
      QString::number(m_td) + " km");
}

void Steps::appendTrack(double lat, double lon) {
  QQuickItem* root = mw_one->ui->qwMap->rootObject();
  QMetaObject::invokeMethod((QObject*)root, "appendTrack", Q_ARG(QVariant, lat),
                            Q_ARG(QVariant, lon));
}

void Steps::updateTrackData(double lat, double lon) {
  QQuickItem* root = mw_one->ui->qwMap->rootObject();
  QMetaObject::invokeMethod((QObject*)root, "updateTrackData",
                            Q_ARG(QVariant, lat), Q_ARG(QVariant, lon));
}

void Steps::updateMapTrackUi(double lat, double lon) {
  QQuickItem* root = mw_one->ui->qwMap->rootObject();
  QMetaObject::invokeMethod((QObject*)root, "updateMapTrackUi",
                            Q_ARG(QVariant, lat), Q_ARG(QVariant, lon));
}

void Steps::clearTrack() {
  QQuickItem* root = mw_one->ui->qwMap->rootObject();
  QMetaObject::invokeMethod((QObject*)root, "clearTrack");
}

void Steps::writeGpsPos(double lat, double lon, int i, int count) {
  QString ss0 = t0;
  QString s0 = ss0.replace(" ", "");
  QString sst = strStartTime;
  QString s1 = sst.replace(":", "");

  lat = QString::number(lat, 'f', 6).toDouble();
  lon = QString::number(lon, 'f', 6).toDouble();

  QSettings Reg(iniDir + s0 + "-gps-" + s1 + ".ini", QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg.setIniCodec("utf-8");
#endif
  Reg.setValue("/" + QString::number(i) + "/lat", lat);
  Reg.setValue("/" + QString::number(i) + "/lon", lon);
  Reg.setValue("/count", count);
}

void Steps::getGpsTrack() {
  if (timer->isActive()) return;

  mw_one->showProgress();
  QQuickItem* root = mw_one->ui->qwGpsList->rootObject();
  QVariant item;
  QMetaObject::invokeMethod((QObject*)root, "getGpsList",
                            Q_RETURN_ARG(QVariant, item));
  strGpsList = item.toString();
  mw_one->myUpdateGpsMapThread->start();
}

void Steps::updateGpsTrack() {
  QStringList list = strGpsList.split("-=-");
  QString st1 = list.at(0);
  QString st2 = list.at(1);
  strGpsMapDateTime = st1 + " " + st2;

  QString st3 = list.at(2);
  QString st4 = list.at(3);
  st1 = st1.replace(" ", "");
  st2 = st2.split("-").at(0);
  QStringList list2 = st2.split(":");
  st2 = list2.at(1) + list2.at(2) + list2.at(3);
  st1 = st1.trimmed();
  st2 = st2.trimmed();
  st3 = st3.split(":").at(1);
  st3 = st3.trimmed();
  st4 = st4.split(":").at(1);
  st4 = st4.trimmed();
  strGpsMapDistnce = st3;
  strGpsMapSpeed = st4;

  QString gpsFile = iniDir + st1 + "-gps-" + st2 + ".ini";
  if (QFile::exists(gpsFile)) {
    QSettings Reg(gpsFile, QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    Reg.setIniCodec("utf-8");
#endif

    bool isOptimizedGps = Reg.value("/Optimized", false).toBool();
    QVector<GPSCoordinate> rawGPSData;

    clearTrack();
    double lat;
    double lon;
    int count = Reg.value("/count", 0).toInt();
    for (int i = 0; i < count; i++) {
      lat = Reg.value("/" + QString::number(i + 1) + "/lat", 0).toDouble();
      lon = Reg.value("/" + QString::number(i + 1) + "/lon", 0).toDouble();

      if (!isOptimizedGps)
        rawGPSData.append({lat, lon});
      else
        updateTrackData(lat, lon);
    }

    if (!isOptimizedGps) {
      int gaussianWindowSize = 3;
      double gaussianSigma = 1.0;
      double outlierThreshold = 0.001;  // 距离阈值，可根据实际情况调整

      // 应用高斯滤波
      QVector<GPSCoordinate> filteredData =
          gaussianFilter(rawGPSData, gaussianWindowSize, gaussianSigma);

      // 检测并修正异常点
      QVector<GPSCoordinate> optimizedData =
          detectAndCorrectOutliers(filteredData, outlierThreshold);

      QFile file(gpsFile);
      file.remove();

      QSettings Reg(gpsFile, QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
      Reg.setIniCodec("utf-8");
#endif
      int count = optimizedData.count();
      Reg.setValue("/count", count);
      Reg.setValue("/Optimized", true);

      for (int i = 0; i < count; i++) {
        lat = optimizedData.at(i).latitude;
        lon = optimizedData.at(i).longitude;
        updateTrackData(lat, lon);

        lat = QString::number(lat, 'f', 6).toDouble();
        lon = QString::number(lon, 'f', 6).toDouble();
        Reg.setValue("/" + QString::number(i + 1) + "/lat", lat);
        Reg.setValue("/" + QString::number(i + 1) + "/lon", lon);
      }
    }

    isGpsMapTrackFile = true;
    lastLat = lat;
    lastLon = lon;

  } else {
    isGpsMapTrackFile = false;
    qDebug() << "gps file=" + gpsFile + " no exists.";
  }
}

void Steps::updateGpsMapUi() {
  if (isGpsMapTrackFile) {
    updateMapTrackUi(lastLat, lastLon);
    mw_one->ui->lblGpsDateTime->setText(strGpsMapDateTime);
    mw_one->ui->qwMap->rootContext()->setContextProperty("strDistance",
                                                         strGpsMapDistnce);
    mw_one->ui->qwMap->rootContext()->setContextProperty("strSpeed",
                                                         strGpsMapSpeed);
    mw_one->ui->tabMotion->setCurrentIndex(3);
  }
}

// 高斯滤波函数
QVector<GPSCoordinate> gaussianFilter(const QVector<GPSCoordinate>& rawData,
                                      int windowSize, double sigma) {
  QVector<GPSCoordinate> filteredData;
  int dataSize = rawData.size();

  for (int i = 0; i < dataSize; ++i) {
    double sumLatitude = 0.0;
    double sumLongitude = 0.0;
    double sumWeight = 0.0;

    for (int j = qMax(0, i - windowSize / 2);
         j < qMin(dataSize, i + windowSize / 2 + 1); ++j) {
      double distance = std::abs(i - j);
      double weight = std::exp(-(distance * distance) / (2 * sigma * sigma));
      sumLatitude += rawData[j].latitude * weight;
      sumLongitude += rawData[j].longitude * weight;
      sumWeight += weight;
    }

    GPSCoordinate filteredCoord;
    filteredCoord.latitude = sumLatitude / sumWeight;
    filteredCoord.longitude = sumLongitude / sumWeight;

    filteredData.append(filteredCoord);
  }

  return filteredData;
}

// 计算两点之间的距离（简化的平面距离计算，实际应用中可能需要更精确的地球表面距离计算）
double calculateDistance(const GPSCoordinate& p1, const GPSCoordinate& p2) {
  double dx = p2.longitude - p1.longitude;
  double dy = p2.latitude - p1.latitude;
  return std::sqrt(dx * dx + dy * dy);
}

// 异常点检测与修正函数
QVector<GPSCoordinate> detectAndCorrectOutliers(
    const QVector<GPSCoordinate>& data, double threshold) {
  QVector<GPSCoordinate> correctedData = data;
  int dataSize = data.size();

  for (int i = 1; i < dataSize - 1; ++i) {
    double dist1 = calculateDistance(data[i - 1], data[i]);
    double dist2 = calculateDistance(data[i], data[i + 1]);

    if (dist1 > threshold || dist2 > threshold) {
      // 进行线性插值修正
      double newLat = (data[i - 1].latitude + data[i + 1].latitude) / 2;
      double newLon = (data[i - 1].longitude + data[i + 1].longitude) / 2;
      correctedData[i].latitude = newLat;
      correctedData[i].longitude = newLon;
    }
  }

  return correctedData;
}
