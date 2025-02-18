#include "Steps.h"

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

  font0.setBold(true);
  font0.setPointSize(13);
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
  // requestLocationPermissions(); // 已在安卓中调用

  QSettings Reg(iniDir + "steps.ini", QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg.setIniCodec("utf-8");
#endif
  m_TotalDistance = Reg.value("/Steps/TotalDistance", 0).toDouble();

#ifdef Q_OS_ANDROID
#else
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
    jdouble distance =
        m_activity.callMethod<jdouble>("getTotalDistance", "()D");
    QString str_distance = QString::number(distance, 'f', 2);
    m_distance = str_distance.toDouble();
    if (!isGpsTest) {
      latitude = m_activity.callMethod<jdouble>("getLatitude", "()D");
      longitude = m_activity.callMethod<jdouble>("getLongitude", "()D");
    }
    QAndroidJniObject jstrGpsStatus =
        m_activity.callObjectMethod<jstring>("getGpsStatus");
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

      if (m_time.second() % 3) {
        if (!isGpsTest) {
          jdouble m_speed = m_activity.callMethod<jdouble>("getMySpeed", "()D");
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
        mw_one->ui->qwMap->rootContext()->setContextProperty(
            "strSpeed", QString::number(mySpeed) + " km/h");
      }
    }

#else
        if (m_time.second() != 0) {
          m_speed = m_distance / m_time.second();
          emit speedChanged();
        }

        if (isGpsTest) {
            if (m_time.second() % 3) {
                appendTrack(latitude, longitude);
                latitude = latitude + 0.001;
                longitude = longitude + 0.001;
                nWriteGpsCount++;
                writeGpsPos(latitude, longitude, nWriteGpsCount, nWriteGpsCount);
            }
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
    // listenerWrapper = QAndroidJniObject("com/x/LocationListenerWrapper",
    //                                     "(Landroid/content/Context;)V",
    //                                     m_activity.object<jobject>());

    if (listenerWrapper.isValid()) {
    } else {
    }

    if (m_activity.callMethod<jdouble>("startGpsUpdates", "()D") == 0) {
      qWarning() << "LocationManager is null";
      mw_one->ui->lblGpsInfo->setText("LocationManager is null...");
      mw_one->ui->btnGPS->setText(tr("Start"));
      return;
    }
  }

#else
  if (m_positionSource) {
    m_positionSource->startUpdates();
  }
#endif

  clearTrack();
  nWriteGpsCount = 0;
  m_time = QTime(0, 0);
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

  QSettings Reg(iniDir + "steps.ini", QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg.setIniCodec("utf-8");
#endif
  Reg.setValue("/Steps/TotalDistance", m_TotalDistance);

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
    QString strTitle = QString::number(nYear) + " - " + QString::number(nMonth);
    if (mw_one->ui->btnSelGpsDate->text() != strTitle) {
      clearAllGpsList();
      loadGpsList(nYear, nMonth);
      mw_one->ui->btnSelGpsDate->setText(strTitle);
    }

    insertGpsList(0, t0, t1, t2, t3, t4, t5);

    int count = getGpsListCount();
    QString strYearMonth =
        QString::number(nYear) + "-" + QString::number(nMonth);
    Reg.setValue("/" + strYearMonth + "/Count", count);
    Reg.setValue(
        "/" + strYearMonth + "/" + QString::number(count),
        t0 + "-=-" + t1 + "-=-" + t2 + "-=-" + t3 + "-=-" + t4 + "-=-" + t5);

    curMonthTotal();
  }

#ifdef Q_OS_ANDROID
  m_distance = m_activity.callMethod<jdouble>("stopGpsUpdates", "()D");
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

  QSettings Reg(iniDir + "steps.ini", QSettings::IniFormat);
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
  double t = 0;
  int count = getGpsListCount();
  for (int i = 0; i < count; i++) {
    QString str = getGpsListText2(i);
    QStringList list = str.split(" ");
    if (list.count() == 3) {
      QString str1 = list.at(1);
      double jl = str1.toDouble();
      t = t + jl;
    }
  }

  QSettings Reg(iniDir + "steps.ini", QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg.setIniCodec("utf-8");
#endif
  QString stry, strm;
  QStringList list = mw_one->ui->btnSelGpsDate->text().split("-");
  stry = list.at(0);
  strm = list.at(1);
  stry = stry.trimmed();
  strm = strm.trimmed();
  int curCount = Reg.value("/" + stry + "-" + strm + "/Count", 0).toInt();
  Reg.setValue("/" + stry + "/" + strm,
               QString::number(t) + "-=-" + QString::number(curCount));

  double yt = 0;
  int ycount = 0;
  for (int i = 0; i < 12; i++) {
    double mt;
    int mcount;
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
  }

  double m_td = Reg.value("/Steps/TotalDistance", 0).toDouble();

  mw_one->ui->lblMonthTotal->setText(
      stry + ": " + QString::number(yt) + " km  " + QString::number(ycount) +
      "\n" + strm + ": " + QString::number(t) + " km  " +
      QString::number(curCount) + "\n" + tr("All Total") + ": " +
      QString::number(m_td) + " km");
}

void Steps::appendTrack(double lat, double lon) {
  QQuickItem* root = mw_one->ui->qwMap->rootObject();
  QMetaObject::invokeMethod((QObject*)root, "appendTrack", Q_ARG(QVariant, lat),
                            Q_ARG(QVariant, lon));
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

  QSettings Reg(iniDir + s0 + "-gps-" + s1 + ".ini", QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg.setIniCodec("utf-8");
#endif
  Reg.setValue("/" + QString::number(i) + "/lat", lat);
  Reg.setValue("/" + QString::number(i) + "/lon", lon);
  Reg.setValue("/count", count);
}

void Steps::getGpsTrack() {
  QQuickItem* root = mw_one->ui->qwGpsList->rootObject();
  QVariant itemCount;
  QMetaObject::invokeMethod((QObject*)root, "getTimeString",
                            Q_RETURN_ARG(QVariant, itemCount));
  QString mStr = itemCount.toString();
  QStringList list = mStr.split("-=-");
  QString st1 = list.at(0);
  QString st2 = list.at(1);
  st1 = st1.replace(" ", "");
  st2 = st2.split("-").at(0);
  QStringList list2 = st2.split(":");
  st2 = list2.at(1) + list2.at(2) + list2.at(3);
  st1 = st1.trimmed();
  st2 = st2.trimmed();

  QString gpsFile = iniDir + st1 + "-gps-" + st2 + ".ini";
  if (QFile::exists(gpsFile)) {
    QSettings Reg(gpsFile, QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    Reg.setIniCodec("utf-8");
#endif

    clearTrack();
    int count = Reg.value("/count", 0).toInt();
    for (int i = 0; i < count; i++) {
      double lat =
          Reg.value("/" + QString::number(i + 1) + "/lat", 0).toDouble();
      double lon =
          Reg.value("/" + QString::number(i + 1) + "/lon", 0).toDouble();
      appendTrack(lat, lon);
    }
    mw_one->ui->tabMotion->setCurrentIndex(3);
  } else {
    qDebug() << "gps file=" + gpsFile + " no exists.";
  }
}
