#include "Todo.h"

#include "mainwindow.h"
#include "ui_Todo.h"
#include "ui_mainwindow.h"

QString highLblStyle = "color:rgb(212,35,122)";
int highCount;
QString orgLblStyle;

extern MainWindow* mw_one;
extern QString iniFile, iniDir;
extern bool loading, isBreak, isImport;
extern int fontSize;

dlgTodo::dlgTodo(QWidget* parent) : QDialog(parent), ui(new Ui::dlgTodo) {
  ui->setupUi(this);

  mw_one->set_btnStyle(this);

  this->installEventFilter(this);
  mw_one->ui->editTodo->viewport()->installEventFilter(mw_one);

  this->setModal(true);

  QString strTar = "/data/data/com.x/files/msg.mp3";
  QFile::copy(":/res/msg.mp3", strTar);

  // 删除线风格
  // ui->listRecycle->setStyleSheet("text-decoration: line-through;");

  mw_one->ui->frameTodo->layout()->setContentsMargins(1, 1, 1, 1);
  mw_one->ui->frameTodo->layout()->setSpacing(2);
  mw_one->ui->editTodo->setContentsMargins(12, 0, 12, 0);
  mw_one->ui->btnAddTodo->setStyleSheet("border:none");
  mw_one->ui->btnBackTodo->setStyleSheet("border:none");
  mw_one->ui->btnHigh->setStyleSheet("border:none");
  mw_one->ui->btnLow->setStyleSheet("border:none");
  mw_one->ui->btnModify->setStyleSheet("border:none");
  mw_one->ui->btnSetTime->setStyleSheet("border:none");
  mw_one->ui->btnRecycle->setStyleSheet("border:none");

  QFont f = this->font();
  f.setPointSize(12);
  mw_one->ui->btnAddTodo->setFont(f);
  mw_one->ui->btnBackTodo->setFont(f);
  mw_one->ui->btnHigh->setFont(f);
  mw_one->ui->btnLow->setFont(f);
  mw_one->ui->btnModify->setFont(f);
  mw_one->ui->btnSetTime->setFont(f);
  mw_one->ui->btnRecycle->setFont(f);

  mw_one->ui->btnModify->hide();

  mw_one->ui->editTodo->setFixedHeight(getEditTextHeight(mw_one->ui->editTodo) +
                                       2);
}

dlgTodo::~dlgTodo() { delete ui; }

void dlgTodo::keyReleaseEvent(QKeyEvent* event) { Q_UNUSED(event); }

void dlgTodo::saveTodo() {
  if (!isSave) return;

  mw_one->isSelf = true;
  isSave = false;

  highCount = 0;
  QSettings Reg(iniDir + "todo.ini", QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg.setIniCodec("utf-8");
#endif

  int count_items = getCount();

  Reg.setValue("/Todo/Count", count_items);
  for (int i = 0; i < count_items; i++) {
    QString strText = getItemTodoText(i);
    QString strTime = getItemTime(i);
    int type = getItemType(i);
    Reg.setValue("/Todo/Item" + QString::number(i), strText);
    Reg.setValue("/Todo/Time" + QString::number(i), strTime);
    Reg.setValue("/Todo/Type" + QString::number(i), type);
  }

  int count1 = getCountRecycle();
  Reg.setValue("/Todo/Count1", count1);
  for (int i = 0; i < count1; i++) {
    QString doneTime = getItemTimeRecycle(i);
    QString str = getItemTodoTextRecycle(i);
    Reg.setValue("/Todo/ItemRecycle" + QString::number(i), str);
    Reg.setValue("/Todo/ItemRecycleDoneTime" + QString::number(i), doneTime);
  }
}

void dlgTodo::init_Todo() {
  mw_one->isSelf = true;
  clearAll();
  QSettings Reg(iniDir + "todo.ini", QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg.setIniCodec("utf-8");
#endif
  int count = Reg.value("/Todo/Count").toInt();
  for (int i = 0; i < count; i++) {
    QString str = Reg.value("/Todo/Item" + QString::number(i)).toString();
    QString strTime = Reg.value("/Todo/Time" + QString::number(i)).toString();
    int type = Reg.value("/Todo/Type" + QString::number(i)).toInt();

    addItem(strTime, type, str);
  }

  clearAllRecycle();
  int count1 = Reg.value("/Todo/Count1").toInt();
  for (int i = 0; i < count1; i++) {
    QString doneTime =
        Reg.value("/Todo/ItemRecycleDoneTime" + QString::number(i)).toString();
    QString str =
        Reg.value("/Todo/ItemRecycle" + QString::number(i)).toString();
    addItemRecycle(doneTime, 0, str);
  }

  refreshTableLists();
  refreshAlarm();
}

void dlgTodo::on_btnAdd_clicked() {
  QString str = mw_one->ui->editTodo->toPlainText().trimmed();
  if (str == "") return;

  int count = getCount();
  for (int i = 0; i < count; i++) {
    QString strTodo = getItemTodoText(i);

    if (str == strTodo) {
      setCurrentIndex(i);
      return;
    }
  }

  QString strTime = QDateTime::currentDateTime().toString();
  insertItem(strTime, 0, str, 0);

  setCurrentIndex(0);

  mw_one->ui->editTodo->setText("");
  refreshTableLists();
  isSave = true;
}

int dlgTodo::getEditTextHeight(QTextEdit* edit) {
  QTextDocument* doc = edit->document();
  doc->adjustSize();
  int mainHeight = doc->size().rheight() * 1.10;
  return mainHeight;
}

void dlgTodo::closeEvent(QCloseEvent* event) {
  Q_UNUSED(event);
  if (mw_one->isHardStepSensor == 1) mw_one->updateHardSensorSteps();
}

bool dlgTodo::eventFilter(QObject* watch, QEvent* evn) {
  if (evn->type() == QEvent::KeyPress) {
    QKeyEvent* keyEvent = static_cast<QKeyEvent*>(evn);
    if (keyEvent->key() == Qt::Key_Back) {
    }
  }

  return QWidget::eventFilter(watch, evn);
}

void dlgTodo::on_btnHigh_clicked() {
  int row = getCurrentIndex();
  if (row < 0) return;

  QString strTime = getItemTime(row);
  QString strText = getItemTodoText(row);

  delItem(row);
  insertItem(strTime, 1, strText, 0);
  setCurrentIndex(0);

  refreshAlarm();
  isSave = true;
}

void dlgTodo::on_btnLow_clicked() {
  int row = getCurrentIndex();
  if (row < 0) return;

  QString strTime = getItemTime(row);
  QString strTodoText = getItemTodoText(row);
  delItem(row);
  addItem(strTime, 0, strTodoText);
  setCurrentIndex(getCount() - 1);

  refreshAlarm();
  isSave = true;
}

void dlgTodo::on_btnOK_clicked() {
  int row = getCurrentIndex();
  if (row < 0) return;
  QString strTodoText = getItemTodoText(row);
  QString strTime;

  if (!mw_one->mymsgDlg->ui->chk1->isChecked() &&
      !mw_one->mymsgDlg->ui->chk2->isChecked() &&
      !mw_one->mymsgDlg->ui->chk3->isChecked() &&
      !mw_one->mymsgDlg->ui->chk4->isChecked() &&
      !mw_one->mymsgDlg->ui->chk5->isChecked() &&
      !mw_one->mymsgDlg->ui->chk6->isChecked() &&
      !mw_one->mymsgDlg->ui->chk7->isChecked()) {
    mw_one->mymsgDlg->ui->chkDaily->setChecked(false);
  }

  if (mw_one->mymsgDlg->ui->chkDaily->isChecked()) {
    QString str;
    if (mw_one->mymsgDlg->ui->chk1->isChecked()) str = str + "1";
    if (mw_one->mymsgDlg->ui->chk2->isChecked()) str = str + "2";
    if (mw_one->mymsgDlg->ui->chk3->isChecked()) str = str + "3";
    if (mw_one->mymsgDlg->ui->chk4->isChecked()) str = str + "4";
    if (mw_one->mymsgDlg->ui->chk5->isChecked()) str = str + "5";
    if (mw_one->mymsgDlg->ui->chk6->isChecked()) str = str + "6";
    if (mw_one->mymsgDlg->ui->chk7->isChecked()) str = str + "7";

    strTime = tr("Alarm") + "  " + str + "  " +
              mw_one->mymsgDlg->ui->dateTimeEdit->time().toString("HH:mm");

  } else {
    strTime = tr("Alarm") + "  " + mw_one->mymsgDlg->ui->dateTimeEdit->text();
  }

  delItem(row);
  insertItem(strTime, 0, strTodoText, row);
  setCurrentIndex(row);

  ui->frameSetTime->hide();
  mw_one->mymsgDlg->close();

  refreshTableLists();
  refreshAlarm();
  isSave = true;
}

bool dlgTodo::isWeekValid(QString lblDateTime, QString strDate) {
  if (!lblDateTime.contains("-")) {
    int week = QDate::fromString(strDate, "yyyy-M-d").dayOfWeek();

    QStringList list = lblDateTime.split(" ");
    QString str = list.at(0);

    for (int i = 0; i < str.length(); i++) {
      if (str.mid(i, 1) == QString::number(week)) {
        return true;
      }
    }
  }
  return false;
}

qlonglong dlgTodo::getSecond(QString strDateTime) {
  // 2022-8-22 18:18
  isTomorrow = false;
  QString strtime, sdt;
  sdt = strDateTime;
  if (!strDateTime.contains("-")) {
    int week = QDate::currentDate().dayOfWeek();
    QStringList list = strDateTime.split(" ");
    QString str = list.at(0);

    for (int i = 0; i < list.count(); i++) {
      QString st = list.at(i);
      if (st.contains(":")) {
        strtime = st;
        break;
      }
    }
    for (int i = 0; i < str.length(); i++) {
      if (str.mid(i, 1) == QString::number(week)) {
        strDateTime = QDate::currentDate().toString("yyyy-M-d") + " " + strtime;
      }
    }
  }

  strDateTime = strDateTime + ":00";
  QString strCur = QDateTime::currentDateTime().toString("yyyy-M-d HH:mm:ss");
  QDateTime timeCur = QDateTime::fromString(strCur, "yyyy-M-d HH:mm:ss");
  QDateTime timeAlarm = QDateTime::fromString(strDateTime, "yyyy-M-d HH:mm:ss");
  qlonglong seconds = timeCur.secsTo(timeAlarm);

  if (seconds <= 0) {
    if (!sdt.contains("-")) {
      QDateTime ctime = QDateTime::currentDateTime();
      QString strTmo = ctime.addDays(+1).toString("yyyy-M-d");

      if (isWeekValid(sdt.split(" ").at(0), strTmo)) {
        strDateTime = strTmo + " " + strtime;
        strDateTime = strDateTime + ":00";
        QString strCur =
            QDateTime::currentDateTime().toString("yyyy-M-d HH:mm:ss");
        QDateTime timeCur = QDateTime::fromString(strCur, "yyyy-M-d HH:mm:ss");
        QDateTime timeAlarm =
            QDateTime::fromString(strDateTime, "yyyy-M-d HH:mm:ss");
        seconds = timeCur.secsTo(timeAlarm);
        isTomorrow = true;
      }
    }
  }

  return seconds;
}

void dlgTodo::on_btnSetTime_clicked() {
  int row = getCurrentIndex();
  if (row < 0) return;

  QString str = getItemTime(row);
  QDate date;
  QTime time;
  mw_one->mymsgDlg->ui->chk1->setChecked(false);
  mw_one->mymsgDlg->ui->chk2->setChecked(false);
  mw_one->mymsgDlg->ui->chk3->setChecked(false);
  mw_one->mymsgDlg->ui->chk4->setChecked(false);
  mw_one->mymsgDlg->ui->chk5->setChecked(false);
  mw_one->mymsgDlg->ui->chk6->setChecked(false);
  mw_one->mymsgDlg->ui->chk7->setChecked(false);
  mw_one->mymsgDlg->ui->chkDaily->setChecked(false);

  if (str.contains(tr("Alarm"))) {
    str = str.replace(tr("Alarm"), "").trimmed();
    QStringList list = str.split(" ");
    if (str.contains("-")) {
      date = QDate::fromString(list.at(0), "yyyy-M-d");
      time = QTime::fromString(list.at(1), "HH:mm");
    } else {
      QString s1 = list.at(0);
      for (int i = 0; i < s1.length(); i++) {
        QString s2 = s1.mid(i, 1);
        if (s2 == "1") mw_one->mymsgDlg->ui->chk1->setChecked(true);
        if (s2 == "2") mw_one->mymsgDlg->ui->chk2->setChecked(true);
        if (s2 == "3") mw_one->mymsgDlg->ui->chk3->setChecked(true);
        if (s2 == "4") mw_one->mymsgDlg->ui->chk4->setChecked(true);
        if (s2 == "5") mw_one->mymsgDlg->ui->chk5->setChecked(true);
        if (s2 == "6") mw_one->mymsgDlg->ui->chk6->setChecked(true);
        if (s2 == "7") mw_one->mymsgDlg->ui->chk7->setChecked(true);
      }
      date = QDate::currentDate();

      for (int i = 0; i < list.count(); i++) {
        if (list.at(i).contains(":")) {
          time = QTime::fromString(list.at(i), "HH:mm");
          break;
        }
      }

      mw_one->mymsgDlg->ui->chkDaily->setChecked(true);
    }

    mw_one->mymsgDlg->ui->dateTimeEdit->setDate(date);
    mw_one->mymsgDlg->ui->dateTimeEdit->setTime(time);

  } else {
    QStringList list = str.split(" ");
    if (str.mid(0, 2) == "20" && str.contains("-")) {
      date = QDate::fromString(list.at(0), "yyyy-M-d");
      time = QTime::fromString(list.at(1), "HH:mm");
    }

    if (list.count() > 2) {
      date = QDate::currentDate();
      time = QTime::currentTime();
    }

    if ((str.mid(0, 1) == "1" || str.mid(0, 1) == "2" || str.mid(0, 1) == "3" ||
         str.mid(0, 1) == "4" || str.mid(0, 1) == "5" || str.mid(0, 1) == "6" ||
         str.mid(0, 1) == "7") &&
        !str.contains("-")) {
      QString s1 = list.at(0);
      for (int i = 0; i < s1.length(); i++) {
        QString s2 = s1.mid(i, 1);
        if (s2 == "1") mw_one->mymsgDlg->ui->chk1->setChecked(true);
        if (s2 == "2") mw_one->mymsgDlg->ui->chk2->setChecked(true);
        if (s2 == "3") mw_one->mymsgDlg->ui->chk3->setChecked(true);
        if (s2 == "4") mw_one->mymsgDlg->ui->chk4->setChecked(true);
        if (s2 == "5") mw_one->mymsgDlg->ui->chk5->setChecked(true);
        if (s2 == "6") mw_one->mymsgDlg->ui->chk6->setChecked(true);
        if (s2 == "7") mw_one->mymsgDlg->ui->chk7->setChecked(true);
      }
      date = QDate::currentDate();
      for (int i = 0; i < list.count(); i++) {
        if (list.at(i).contains(":")) {
          time = QTime::fromString(list.at(i), "HH:mm");
          break;
        }
      }

      mw_one->mymsgDlg->ui->chkDaily->setChecked(true);
    }

    mw_one->mymsgDlg->ui->dateTimeEdit->setDate(date);
    mw_one->mymsgDlg->ui->dateTimeEdit->setTime(time);
  }

  mw_one->mymsgDlg->initDlg();
  QString txt = tr("Todo") + " : " + getItemTodoText(row);
  txt = txt.replace("\n", " ");
  QFontMetrics fm(this->font());
  QString qsLine = fm.elidedText(txt, Qt::ElideRight, this->width() - 10);
  mw_one->mymsgDlg->ui->lblTodoText->setText(qsLine);
  mw_one->mymsgDlg->show();
}

void dlgTodo::on_btnCancel_clicked() {
  int row = getCurrentIndex();
  if (row < 0) return;

  QString str = getItemTime(row);
  if (str.contains(tr("Alarm"))) str = str.replace(tr("Alarm"), "");
  modifyTime(row, str);
  ui->frameSetTime->hide();
  mw_one->mymsgDlg->close();

  refreshTableLists();
  refreshAlarm();
  isSave = true;
}

void dlgTodo::startTimerAlarm(QString text) {
  Q_UNUSED(text);
#ifdef Q_OS_ANDROID

#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  QAndroidJniObject jo = QAndroidJniObject::fromString(text);
  // jo.callStaticMethod<int>("com.x/MyService", "startTimerAlarm", "()I");

  jo.callStaticMethod<int>("com.x/MyActivity", "startAlarm",
                           "(Ljava/lang/String;)I", jo.object<jstring>());

  jo.callStaticMethod<int>("com.x/ClockActivity", "setInfoText",
                           "(Ljava/lang/String;)I", jo.object<jstring>());
#else
  QJniObject jo = QJniObject::fromString(text);

  jo.callStaticMethod<int>("com.x/MyActivity", "startAlarm",
                           "(Ljava/lang/String;)I", jo.object<jstring>());

  jo.callStaticMethod<int>("com.x/ClockActivity", "setInfoText",
                           "(Ljava/lang/String;)I", jo.object<jstring>());
#endif

#endif
}

void dlgTodo::stopTimerAlarm() {
#ifdef Q_OS_ANDROID

#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  QAndroidJniObject jo = QAndroidJniObject::fromString("stopTimerAlarm");
  // jo.callStaticMethod<int>("com.x/MyService", "stopTimerAlarm", "()I");

  jo.callStaticMethod<int>("com.x/MyActivity", "stopAlarm", "()I");
#else
  QJniObject jo = QJniObject::fromString("stopTimerAlarm");

  jo.callStaticMethod<int>("com.x/MyActivity", "stopAlarm", "()I");
#endif

#endif
}

void dlgTodo::sendMsgAlarm(QString text) {
  Q_UNUSED(text);
#ifdef Q_OS_ANDROID
  QString strNotify = tr("Todo") + " : " + text;
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  QAndroidJniObject javaNotification = QAndroidJniObject::fromString(strNotify);
  QAndroidJniObject::callStaticMethod<void>(
      "com/x/MyService", "notifyTodoAlarm",
      "(Landroid/content/Context;Ljava/lang/String;)V",
      QtAndroid::androidContext().object(), javaNotification.object<jstring>());
#else
  QJniObject javaNotification = QJniObject::fromString(strNotify);
  QJniObject::callStaticMethod<void>(
      "com/x/MyService", "notifyTodoAlarm",
      "(Landroid/content/Context;Ljava/lang/String;)V",
      QNativeInterface::QAndroidApplication::context(),
      javaNotification.object<jstring>());
#endif

#endif
}

void dlgTodo::on_btnRecycle_clicked() {
  mw_one->ui->frameTodo->hide();
  mw_one->ui->frameRecycle->show();
}

void dlgTodo::on_btnReturn_clicked() {
  mw_one->ui->frameRecycle->hide();
  mw_one->ui->frameTodo->show();
}

void dlgTodo::on_btnClear_clicked() {
  clearAllRecycle();

  isSave = true;
}

void dlgTodo::on_btnRestore_clicked() {
  if (getCountRecycle() == 0) return;

  int row = getCurrentIndexRecycle();
  QString strTime = QDate::currentDate().toString("ddd MM dd yyyy") + "  " +
                    QTime::currentTime().toString();
  QString strText = getItemTodoTextRecycle(row);
  addItem(strTime, 0, strText);

  on_btnDel_clicked();

  isSave = true;
}

void dlgTodo::on_btnDel_clicked() {
  int row = getCurrentIndexRecycle();
  if (row < 0) return;
  delItemRecycle(row);

  isSave = true;
}

void dlgTodo::refreshTableLists() {
  tableLists.clear();
  int count_items = getCount();

  for (int i = 0; i < count_items; i++) {
    QString strTime = getItemTime(i);
    QString strText = getItemTodoText(i);

    tableLists.append(strTime + "|=|" + strText);
  }
}

void dlgTodo::refreshAlarm() {
  stopTimerAlarm();
  int count = 0;
  isToday = false;
  QString str;

  QString ini_file = "/data/data/com.x/files/msg.ini";
  QSettings Reg(ini_file, QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  Reg.setIniCodec("utf-8");
#endif

  QStringList listAlarm;
  QList<qlonglong> listTotalS;

  int count_items = getCount();

  if (count_items > 0) {
    for (int i = 0; i < count_items; i++) {
      str = getItemTime(i);

      if (str.contains(tr("Alarm"))) {
        modifyType(i, 3);
        str = str.replace(tr("Alarm"), "").trimmed();
        qlonglong totals = getSecond(str);

        if (totals > 0) {
          count++;

          QString todo_text = getItemTodoText(i);
          QString str1 = str + "|" + todo_text + "|" + QString::number(totals) +
                         "|" + tr("Close");

          listAlarm.append(str1);
          listTotalS.append(totals);

          // set time marks
          QString strDate = str.split(" ").at(0);
          QString strToday = QDate::currentDate().toString("yyyy-M-d");
          QDateTime ctime = QDateTime::currentDateTime();
          QString strTmo = ctime.addDays(+1).toString("yyyy-M-d");
          if (strDate.contains("-")) {
            if (strDate == strToday) {
              modifyType(i, 1);
              isToday = true;
            }

            if (strTmo == strDate) {
              modifyType(i, 2);
            }
          } else {
            if (isWeekValid(str, strToday) && !isTomorrow) {
              modifyType(i, 1);
              isToday = true;
            }

            if (isWeekValid(str, strTmo) && isTomorrow) {
              modifyType(i, 2);
            }
          }

        } else {
          if (str.contains("-")) {
            modifyTime(i, str);
            modifyType(i, 0);
          }

          if (!str.contains("-")) {
            modifyTime(i, tr("Alarm") + "  " + str);
            modifyType(i, 3);

            QDateTime ctime = QDateTime::currentDateTime();
            QString strTmo = ctime.addDays(+1).toString("yyyy-M-d");
            if (isWeekValid(str, strTmo)) {
              modifyType(i, 2);
            }
          }
        }
      }
    }
  } else {
    count_items = tableLists.count();

    for (int i = 0; i < count_items; i++) {
      QString strList = tableLists.at(i);
      QStringList list = strList.split("|=|");
      QString strTime = list.at(0);

      if (strTime.contains(tr("Alarm"))) {
        strTime = strTime.replace(tr("Alarm"), "").trimmed();
        qlonglong totals = getSecond(strTime);

        if (totals > 0) {
          count++;

          QString todo_text = list.at(1);
          QString str1 = strTime + "|" + todo_text + "|" +
                         QString::number(totals) + "|" + tr("Close");

          listAlarm.append(str1);
          listTotalS.append(totals);
        }
      }
    }
  }

  qlonglong minValue = 0;

  if (count > 0) {
    minValue = *std::min_element(listTotalS.begin(), listTotalS.end());
    for (int i = 0; i < listTotalS.count(); i++) {
      if (minValue == listTotalS.at(i)) {
        QString str1 = listAlarm.at(i);
        startTimerAlarm(str1);
        Reg.setValue("msg", str1);

        qDebug() << "Min Time: " << listTotalS << minValue << str1
                 << "curVol: ";

        // to top
        int listcount = getCount();
        for (int m = 0; m < listcount; m++) {
          QString date = getItemTime(m);
          int type = getItemType(m);
          QString text = getItemTodoText(m);
          if (str1.contains(text)) {
            delItem(m);
            insertItem(date, type, text, 0);
            break;
          }
        }

        break;
      }
    }
  }

  if (!isToday) {
    mw_one->ui->btnTodo->setIcon(QIcon(":/res/todo.png"));
  } else {
    mw_one->ui->btnTodo->setIcon(QIcon(":/res/todo1.png"));
  }

  Reg.setValue("count", count);
  QString strMute = "true";
  Reg.setValue("mute", strMute);

  if (!QFile(ini_file).exists())
    qDebug() << "ini no exists";
  else
    qDebug() << "ini ok";
}

void dlgTodo::on_editTodo_textChanged() {
  int h = getEditTextHeight(mw_one->ui->editTodo) + 2;
  mw_one->ui->editTodo->setFixedHeight(h);
}

void dlgTodo::insertItem(QString strTime, int type, QString strText,
                         int curIndex) {
  int itemheight = setItemHeight(strText);

  QQuickItem* root = mw_one->ui->qwTodo->rootObject();
  QMetaObject::invokeMethod(
      (QObject*)root, "insertItem", Q_ARG(QVariant, strTime),
      Q_ARG(QVariant, type), Q_ARG(QVariant, strText),
      Q_ARG(QVariant, itemheight), Q_ARG(QVariant, curIndex));
}

void dlgTodo::insertRecycle(QString strTime, int type, QString strText,
                            int curIndex) {
  int itemheight = setItemHeight(strText);

  QQuickItem* root = mw_one->ui->qwRecycle->rootObject();
  QMetaObject::invokeMethod(
      (QObject*)root, "insertRecycle", Q_ARG(QVariant, strTime),
      Q_ARG(QVariant, type), Q_ARG(QVariant, strText),
      Q_ARG(QVariant, itemheight), Q_ARG(QVariant, curIndex));
}

int dlgTodo::getCurrentIndex() {
  QQuickItem* root = mw_one->ui->qwTodo->rootObject();
  QVariant itemIndex;
  QMetaObject::invokeMethod((QObject*)root, "getCurrentIndex",
                            Q_RETURN_ARG(QVariant, itemIndex));
  return itemIndex.toInt();
}

int dlgTodo::getCurrentIndexRecycle() {
  QQuickItem* root = mw_one->ui->qwRecycle->rootObject();
  QVariant itemIndex;
  QMetaObject::invokeMethod((QObject*)root, "getCurrentIndex",
                            Q_RETURN_ARG(QVariant, itemIndex));
  return itemIndex.toInt();
}

QString dlgTodo::getItemTime(int index) {
  QQuickItem* root = mw_one->ui->qwTodo->rootObject();
  QVariant itemTime;
  QMetaObject::invokeMethod((QObject*)root, "getTime",
                            Q_RETURN_ARG(QVariant, itemTime),
                            Q_ARG(QVariant, index));
  return itemTime.toString();
}

QString dlgTodo::getItemTimeRecycle(int index) {
  QQuickItem* root = mw_one->ui->qwRecycle->rootObject();
  QVariant itemTime;
  QMetaObject::invokeMethod((QObject*)root, "getTime",
                            Q_RETURN_ARG(QVariant, itemTime),
                            Q_ARG(QVariant, index));
  return itemTime.toString();
}

int dlgTodo::getItemType(int index) {
  QQuickItem* root = mw_one->ui->qwTodo->rootObject();
  QVariant itemType;
  QMetaObject::invokeMethod((QObject*)root, "getType",
                            Q_RETURN_ARG(QVariant, itemType),
                            Q_ARG(QVariant, index));
  return itemType.toInt();
}

QString dlgTodo::getItemTodoText(int index) {
  QQuickItem* root = mw_one->ui->qwTodo->rootObject();
  QVariant itemTodoText;
  QMetaObject::invokeMethod((QObject*)root, "getTodoText",
                            Q_RETURN_ARG(QVariant, itemTodoText),
                            Q_ARG(QVariant, index));
  return itemTodoText.toString();
}

QString dlgTodo::getItemTodoTextRecycle(int index) {
  QQuickItem* root = mw_one->ui->qwRecycle->rootObject();
  QVariant itemTodoText;
  QMetaObject::invokeMethod((QObject*)root, "getTodoText",
                            Q_RETURN_ARG(QVariant, itemTodoText),
                            Q_ARG(QVariant, index));
  return itemTodoText.toString();
}

void dlgTodo::delItem(int index) {
  QQuickItem* root = mw_one->ui->qwTodo->rootObject();
  QMetaObject::invokeMethod((QObject*)root, "delItem", Q_ARG(QVariant, index));
}

void dlgTodo::delItemRecycle(int index) {
  QQuickItem* root = mw_one->ui->qwRecycle->rootObject();
  QMetaObject::invokeMethod((QObject*)root, "delItem", Q_ARG(QVariant, index));
}

void dlgTodo::addItem(QString strTime, int type, QString strText) {
  int itemheight = setItemHeight(strText);

  QQuickItem* root = mw_one->ui->qwTodo->rootObject();
  QMetaObject::invokeMethod((QObject*)root, "addItem", Q_ARG(QVariant, strTime),
                            Q_ARG(QVariant, type), Q_ARG(QVariant, strText),
                            Q_ARG(QVariant, itemheight));
}

void dlgTodo::addItemRecycle(QString strTime, int type, QString strText) {
  int itemheight = setItemHeight(strText);

  QQuickItem* root = mw_one->ui->qwRecycle->rootObject();
  QMetaObject::invokeMethod((QObject*)root, "addItem", Q_ARG(QVariant, strTime),
                            Q_ARG(QVariant, type), Q_ARG(QVariant, strText),
                            Q_ARG(QVariant, itemheight));
}

void dlgTodo::setCurrentIndex(int index) {
  QQuickItem* root = mw_one->ui->qwTodo->rootObject();
  QMetaObject::invokeMethod((QObject*)root, "setCurrentItem",
                            Q_ARG(QVariant, index));
}

void dlgTodo::setHighPriority(bool isBool) {
  QQuickItem* root = mw_one->ui->qwTodo->rootObject();
  QMetaObject::invokeMethod((QObject*)root, "setHighPriority",
                            Q_ARG(QVariant, isBool));
}

int dlgTodo::setItemHeight(QString strTodoText) {
  QFont font = this->font();
  font.setPointSize(fontSize);
  QFontMetrics fm(font);
  int fontHeight = fm.height();

  QTextEdit* edit = new QTextEdit;
  edit->append(strTodoText);
  int itemHeight = fontHeight * 2 + getEditTextHeight(edit);

  return itemHeight;
}

int dlgTodo::getCount() {
  QQuickItem* root = mw_one->ui->qwTodo->rootObject();
  QVariant itemCount;
  QMetaObject::invokeMethod((QObject*)root, "getItemCount",
                            Q_RETURN_ARG(QVariant, itemCount));
  return itemCount.toInt();
}

int dlgTodo::getCountRecycle() {
  QQuickItem* root = mw_one->ui->qwRecycle->rootObject();
  QVariant itemCount;
  QMetaObject::invokeMethod((QObject*)root, "getItemCount",
                            Q_RETURN_ARG(QVariant, itemCount));
  return itemCount.toInt();
}

void dlgTodo::modifyTime(int index, QString strTime) {
  QQuickItem* root = mw_one->ui->qwTodo->rootObject();
  QMetaObject::invokeMethod((QObject*)root, "modifyItemTime",
                            Q_ARG(QVariant, index), Q_ARG(QVariant, strTime));
}

void dlgTodo::modifyType(int index, int type) {
  QQuickItem* root = mw_one->ui->qwTodo->rootObject();
  QMetaObject::invokeMethod((QObject*)root, "modifyItemType",
                            Q_ARG(QVariant, index), Q_ARG(QVariant, type));
}

void dlgTodo::modifyTodoText(int index, QString strTodoText) {
  QQuickItem* root = mw_one->ui->qwTodo->rootObject();
  QMetaObject::invokeMethod((QObject*)root, "modifyItemText",
                            Q_ARG(QVariant, index),
                            Q_ARG(QVariant, strTodoText));
}

void dlgTodo::clearAll() {
  int count = getCount();
  for (int i = 0; i < count; i++) {
    delItem(0);
  }
}

void dlgTodo::clearAllRecycle() {
  int count = getCountRecycle();
  for (int i = 0; i < count; i++) {
    delItemRecycle(0);
  }
}

void dlgTodo::isAlarm(int index) {
  bool a = false;
  QString strTime = getItemTime(index);
  if (strTime.contains(tr("Alarm"))) a = true;
  qDebug() << "aabb" << a;
  setHighPriority(a);
}

void dlgTodo::reeditText() {
  int row = getCurrentIndex();
  if (row < 0) return;

  QDialog* dlg = new QDialog(this);
  QVBoxLayout* vbox0 = new QVBoxLayout;
  dlg->setLayout(vbox0);
  dlg->setModal(true);
  dlg->setWindowFlag(Qt::FramelessWindowHint);
  dlg->setAttribute(Qt::WA_TranslucentBackground);

  QFrame* frame = new QFrame(this);
  vbox0->addWidget(frame);
  frame->setStyleSheet(
      "QFrame{background-color: rgb(255, 255, 255);border-radius:10px; "
      "border:1px solid gray;}");

  QVBoxLayout* vbox = new QVBoxLayout;
  vbox->setContentsMargins(12, 12, 12, 12);
  vbox->setSpacing(12);
  frame->setLayout(vbox);
  vbox->setContentsMargins(3, 3, 3, 3);
  vbox->setSpacing(3);

  QLabel* lblTitle = new QLabel(this);
  lblTitle->adjustSize();
  lblTitle->setWordWrap(true);
  lblTitle->setText(tr("Editor"));
  vbox->addWidget(lblTitle);
  lblTitle->hide();

  QFrame* hframe = new QFrame(this);
  hframe->setFrameShape(QFrame::HLine);
  hframe->setStyleSheet("QFrame{background:red;min-height:2px}");
  vbox->addWidget(hframe);
  hframe->hide();

  QTextEdit* edit = new QTextEdit(this);
  vbox->addWidget(edit);
  edit->setPlainText(getItemTodoText(row));
  QScroller::grabGesture(edit, QScroller::LeftMouseButtonGesture);
  edit->horizontalScrollBar()->setHidden(true);
  edit->verticalScrollBar()->setStyleSheet(mw_one->vsbarStyleSmall);
  mw_one->setSCrollPro(edit);

  QToolButton* btnCancel = new QToolButton(this);
  QToolButton* btnCopy = new QToolButton(this);
  QToolButton* btnOk = new QToolButton(this);
  btnCancel->setText(tr("Cancel"));
  btnCopy->setText(tr("Copy"));
  btnOk->setText(tr("OK"));
  btnOk->setStyleSheet(
      "QToolButton {background-color: rgb(0, 0, 255);color: rgb(255, "
      "255, 255);border-radius:10px;border:1px solid gray;} "
      "QToolButton:pressed "
      "{ background-color: "
      "rgb(220,220,230);color: black}");

  btnCancel->setStyleSheet(mw_one->btnStyle);
  btnCopy->setStyleSheet(mw_one->btnStyle);
  btnOk->setStyleSheet(mw_one->btnStyle);
  btnOk->setFixedHeight(35);
  btnCancel->setFixedHeight(35);
  btnCopy->setFixedHeight(35);

  QHBoxLayout* hbox = new QHBoxLayout;
  hbox->addWidget(btnCancel);
  hbox->addWidget(btnCopy);
  hbox->addWidget(btnOk);
  btnCancel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  btnCopy->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  btnOk->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

  QSpacerItem* sparcer_item =
      new QSpacerItem(0, 60, QSizePolicy::Fixed, QSizePolicy::Expanding);
  vbox->addItem(sparcer_item);

  vbox->addLayout(hbox, 0);

  connect(btnCancel, &QToolButton::clicked, [=]() mutable { dlg->close(); });
  connect(dlg, &QDialog::rejected, [=]() mutable {});
  connect(btnCopy, &QToolButton::clicked, [=]() mutable {
    edit->selectAll();
    edit->copy();
    dlg->close();
  });
  connect(btnOk, &QToolButton::clicked, [=]() mutable {
    QString strTime = getItemTime(row);
    int type = getItemType(row);
    delItem(row);
    insertItem(strTime, type, edit->toPlainText().trimmed(), row);
    setCurrentIndex(row);
    isSave = true;
    dlg->close();
  });

  int x, y, w, h;
  w = mw_one->width() - 20;
  x = mw_one->geometry().x() + (mw_one->width() - w) / 2;
  h = mw_one->height() / 3;
  y = geometry().y() + (height() - h) / 4;
  dlg->setGeometry(x, y, w, h);

  dlg->setModal(true);
  dlg->show();
}

void dlgTodo::addToRecycle() {
  int row = getCurrentIndex();
  QString strTodoText = getItemTodoText(row);
  QString doneTime =
      QDateTime::currentDateTime().toString() + "  " + tr("Done");

  insertRecycle(doneTime, 0, strTodoText, 0);

  isSave = true;
}
