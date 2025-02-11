#include "Steps.h"

#include "src/MainWindow.h"
#include "ui_MainWindow.h"
#include "ui_StepsOptions.h"

extern MainWindow* mw_one;
extern Method* m_Method;
extern QRegularExpression regxNumber;
extern QList<float> rlistX, rlistY, rlistZ, glistX, glistY, glistZ;
extern unsigned int num_steps_walk, num_steps_run, num_steps_hop;
extern bool loading;
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
  mw_one->ui->lblX->setFont(font0);
  mw_one->ui->lblY->setFont(font0);
  mw_one->ui->lblZ->setFont(font0);
  mw_one->ui->lblSteps->setFont(font0);

  font0.setBold(true);
  font0.setPointSize(13);
  mw_one->ui->lblCurrent->setFont(font0);
  mw_one->ui->lblToNow->setFont(font0);
  mw_one->ui->lblNow->setFont(font0);
  mw_one->ui->lblGpsInfo->setFont(font0);

  QFont font1 = m_Method->getNewFont(19);
  font1.setBold(true);
  mw_one->ui->lblKM->setFont(font1);
  mw_one->ui->lblSingle->setFont(font1);

  lblStyleNormal = mw_one->ui->lblX->styleSheet();

  QValidator* validator = new QRegularExpressionValidator(
      regxNumber, mw_one->ui->editTangentLineIntercept);
  mw_one->ui->editTangentLineIntercept->setValidator(validator);
  mw_one->ui->editTangentLineSlope->setValidator(validator);
  mw_one->m_StepsOptions->ui->editStepLength->setValidator(validator);
  mw_one->m_StepsOptions->ui->editStepsThreshold->setValidator(validator);
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

void Steps::on_btnPause_clicked() {
  if (mw_one->ui->btnPauseSteps->text() == tr("Pause")) {
    mw_one->ui->btnPauseSteps->setText(tr("Start"));
    mw_one->stopJavaTimer();
    mw_one->accel_pedometer->stop();
    mw_one->accel_pedometer->setActive(false);
    mw_one->gyroscope->stop();
    mw_one->gyroscope->setActive(false);
    mw_one->ui->btnPause->setIcon(QIcon(":/res/pause.png"));

    releaseWakeLock();

  } else if (mw_one->ui->btnPauseSteps->text() == tr("Start")) {
    mw_one->ui->btnPauseSteps->setText(tr("Pause"));

    acquireWakeLock();

    if (mw_one->ui->rbAlg1->isChecked()) on_rbAlg1_clicked();

    mw_one->ui->btnPause->setIcon(QIcon(":/res/run.png"));
  }
}

void Steps::on_btnReset_clicked() {
  mw_one->accel_pedometer->resetStepCount();

  mw_one->CurrentSteps = 0;
  mw_one->ui->lblSingle->setText("0");
  mw_one->ui->btnSteps->setText(tr("Steps"));
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
  Reg.setValue("/Steps/Intercept",
               mw_one->ui->editTangentLineIntercept->text().trimmed());
  Reg.setValue("/Steps/Slope",
               mw_one->ui->editTangentLineSlope->text().trimmed());
  Reg.setValue("/Steps/Alg1", mw_one->ui->rbAlg1->isChecked());

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

  bool isRun = false;
  if (mw_one->ui->btnPauseSteps->text() == tr("Pause")) {
    mw_one->ui->btnPauseSteps->click();
    isRun = true;
  }

  QString ini_file;
  ini_file = iniDir + "steps.ini";
  QSettings Reg(ini_file, QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg.setIniCodec("utf-8");
#endif

  mw_one->ui->editTangentLineIntercept->setText(
      Reg.value("/Steps/Intercept", dleInter).toString());
  mw_one->ui->editTangentLineSlope->setText(
      Reg.value("/Steps/Slope", dleSlope).toString());
  mw_one->m_StepsOptions->ui->editStepLength->setText(
      Reg.value("/Steps/Length", "35").toString());
  mw_one->m_StepsOptions->ui->editStepsThreshold->setText(
      Reg.value("/Steps/Threshold", "10000").toString());
  mw_one->ui->rbAlg1->setChecked(Reg.value("Steps/Alg1", true).toBool());

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

  if (isRun) {
    if (mw_one->ui->btnPauseSteps->text() == tr("Start"))
      mw_one->ui->btnPause->click();
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

void Steps::on_btnDefaultIntercept_clicked() {
  mw_one->ui->editTangentLineIntercept->setText(QString::number(dleInter));
}

void Steps::on_btnDefaultSlope_clicked() {
  mw_one->ui->editTangentLineSlope->setText(QString::number(dleSlope));
}

void Steps::on_rbAlg1_clicked() {
  if (mw_one->ui->btnPauseSteps->text() == tr("Start")) return;
  // if (mw_one->m_Preferences->ui->chkDebug->isChecked())
  // ui->frameWay1->show();
  mw_one->ui->lblSteps->setText("");

  mw_one->accel_pedometer->start();
  mw_one->accel_pedometer->setActive(true);
  mw_one->gyroscope->stop();
  mw_one->gyroscope->setActive(false);

#ifdef Q_OS_ANDROID

#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  QAndroidJniObject jo = QAndroidJniObject::fromString("Sleep1Win");
  jo.callStaticMethod<int>("com.x/MyService", "setSleep1", "()I");
#else
  QJniObject jo = QJniObject::fromString("Sleep1Win");
  jo.callStaticMethod<int>("com.x/MyService", "setSleep1", "()I");
#endif

#endif
}

void Steps::on_rbAlg2_clicked() {}

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
    QString strGpsStatus;
#ifdef Q_OS_ANDROID
    // 获取总运动距离
    jdouble distance =
        m_activity.callMethod<jdouble>("getTotalDistance", "()D");
    m_distance = (double)distance / 1000.00;
    latitude = m_activity.callMethod<jdouble>("getLatitude", "()D");
    longitude = m_activity.callMethod<jdouble>("getLongitude", "()D");
    QAndroidJniObject jstrGpsStatus =
        m_activity.callObjectMethod<jstring>("getGpsStatus");
    if (jstrGpsStatus.isValid()) strGpsStatus = jstrGpsStatus.toString();
#else

#endif

    if (m_time.second() != 0) {
      m_speed = m_distance / m_time.second();
      emit speedChanged();
    }
    strDistance = tr("Distance") + " : " + QString::number(m_distance) + " km";
    strMotionTime = tr("Duration") + " : " + m_time.toString("hh:mm:ss");
    mw_one->ui->lblGpsInfo->setText(strDistance + "    " + strMotionTime +
                                    "\n" + QString::number(latitude) + " - " +
                                    QString::number(longitude) + "\n" +
                                    strGpsStatus);
    emit timeChanged();
  });

#ifdef Q_OS_ANDROID

  m_activity = QtAndroid::androidActivity();
  if (m_activity.isValid()) {
    QAndroidJniObject locationService = m_activity.callObjectMethod(
        "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;",
        QAndroidJniObject::fromString("location").object<jstring>());
    if (locationService.isValid()) {
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
  }

#else
  if (m_positionSource) {
    m_positionSource->startUpdates();
  }
#endif

  m_time = QTime(0, 0);
  timer->start(1000);
  m_distance = 0;
  m_speed = 0;
  emit distanceChanged(m_distance);
  emit timeChanged();

  mw_one->ui->btnGPS->setText(tr("Stop"));
}

void Steps::positionUpdated(const QGeoPositionInfo& info) {
  if (lastPosition.isValid()) {
    double b = 1000;
    // Convert to km
    m_distance += (double)lastPosition.distanceTo(info.coordinate()) / b;
    emit distanceChanged(m_distance);
  }
  lastPosition = info.coordinate();

  latitude = lastPosition.latitude();
  longitude = lastPosition.longitude();
}

void Steps::stopRecordMotion() {
  timer->stop();

#ifdef Q_OS_ANDROID
  m_distance = m_activity.callMethod<jdouble>("stopGpsUpdates", "()D");
#else
  if (m_positionSource) {
    m_positionSource->stopUpdates();
  }
  delete m_positionSource;
#endif

  ShowMessage* msg = new ShowMessage(this);
  const auto speed = m_speed * 3.6;
  QString strSpeed =
      tr("Speed") + " : " + QString::number(speed, 'g', 2) + " km/h";
  msg->showMsg("Knot", strDistance + "\n\n" + strMotionTime + "\n\n" + strSpeed,
               1);
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
