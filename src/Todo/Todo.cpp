#include "Todo.h"

#include "src/MainWindow.h"
#include "ui_MainWindow.h"
#include "ui_Todo.h"

QString highLblStyle = "color:rgb(212,35,122)";
int highCount;
QString orgLblStyle;

extern MainWindow* mw_one;
extern Method* m_Method;
extern QString iniFile, iniDir;
extern bool loading, isBreak, zh_cn, isDark;
extern int fontSize;

Todo::Todo(QWidget* parent) : QDialog(parent), ui(new Ui::Todo) {
  ui->setupUi(this);

  mw_one->set_ToolButtonStyle(this);

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

  QFont f = this->font();
  f.setPointSize(12);
  mw_one->ui->btnAddTodo->setFont(f);
  mw_one->ui->btnBackTodo->setFont(f);
  mw_one->ui->btnHigh->setFont(f);
  mw_one->ui->btnLow->setFont(f);
  mw_one->ui->btnModify->setFont(f);
  mw_one->ui->btnSetTime->setFont(f);
  mw_one->ui->btnRecycle->setFont(f);

  mw_one->ui->btnPasteTodo->hide();

  mw_one->ui->editTodo->setFixedHeight(getEditTextHeight(mw_one->ui->editTodo) +
                                       4);
}

Todo::~Todo() { delete ui; }

void Todo::keyReleaseEvent(QKeyEvent* event) { Q_UNUSED(event); }

void Todo::saveTodo() {
  if (!isNeedSave) return;

  mw_one->isSelf = true;
  isNeedSave = false;

  mw_one->isNeedAutoBackup = true;
  mw_one->strLatestModify = tr("Modi Todo");

  highCount = 0;

  int count_items = getCount();

  iniTodo = new QSettings(iniDir + "todo.ini", QSettings::IniFormat, this);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  iniTodo->setIniCodec("utf-8");
#endif

  iniTodo->setValue("/Todo/Count", count_items);
  for (int i = 0; i < count_items; i++) {
    QString strText = getItemTodoText(i);
    QString strTime = getItemTime(i);
    int type = getItemType(i);
    iniTodo->setValue("/Todo/Item" + QString::number(i), strText);
    iniTodo->setValue("/Todo/Time" + QString::number(i), strTime);
    iniTodo->setValue("/Todo/Type" + QString::number(i), type);
  }

  int count1 = getCountRecycle();
  iniTodo->setValue("/Todo/Count1", count1);
  for (int i = 0; i < count1; i++) {
    QString doneTime = getItemTimeRecycle(i);
    QString str = getItemTodoTextRecycle(i);
    iniTodo->setValue("/Todo/ItemRecycle" + QString::number(i), str);
    iniTodo->setValue("/Todo/ItemRecycleDoneTime" + QString::number(i),
                      doneTime);
  }
}

void Todo::init_Todo() {
  mw_one->isSelf = true;
  clearAll();

  iniTodo = new QSettings(iniDir + "todo.ini", QSettings::IniFormat, this);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  iniTodo->setIniCodec("utf-8");
#endif

  int count = iniTodo->value("/Todo/Count").toInt();
  for (int i = 0; i < count; i++) {
    QString str = iniTodo->value("/Todo/Item" + QString::number(i)).toString();
    QString strTime =
        iniTodo->value("/Todo/Time" + QString::number(i)).toString();
    int type = iniTodo->value("/Todo/Type" + QString::number(i)).toInt();

    addItem(strTime, type, str);
  }

  clearAllRecycle();
  int count1 = iniTodo->value("/Todo/Count1").toInt();
  for (int i = 0; i < count1; i++) {
    QString doneTime =
        iniTodo->value("/Todo/ItemRecycleDoneTime" + QString::number(i))
            .toString();
    QString str =
        iniTodo->value("/Todo/ItemRecycle" + QString::number(i)).toString();
    addItemRecycle(doneTime, 0, str);
  }

  refreshTableLists();
  refreshAlarm();
}

void Todo::on_btnAdd_clicked() {
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
  isNeedSave = true;
  saveTodo();
}

int Todo::getEditTextHeight(QTextEdit* edit) {
  QTextDocument* doc = edit->document();
  doc->adjustSize();
  int textHeight = doc->size().rheight() * 1.10;

  return textHeight;
}

void Todo::closeEvent(QCloseEvent* event) {
  Q_UNUSED(event);
  if (mw_one->isHardStepSensor == 1) mw_one->updateHardSensorSteps();
}

bool Todo::eventFilter(QObject* watch, QEvent* evn) {
  if (evn->type() == QEvent::KeyRelease) {
    QKeyEvent* keyEvent = static_cast<QKeyEvent*>(evn);
    if (keyEvent->key() == Qt::Key_Back) {
    }
  }

  return QWidget::eventFilter(watch, evn);
}

void Todo::on_btnHigh_clicked() {
  int row = getCurrentIndex();
  if (row < 0) return;

  QString strTime = getItemTime(row);

  if (getTimeStr(strTime) != "") return;

  QString strText = getItemTodoText(row);

  delItem(row);
  insertItem(strTime, 1, strText, 0);
  setCurrentIndex(0);

  refreshAlarm();
  isNeedSave = true;
  saveTodo();
}

void Todo::on_btnLow_clicked() {
  int row = getCurrentIndex();
  if (row < 0) return;

  QString strTime = getItemTime(row);

  if (getTimeStr(strTime) != "") return;

  QString strTodoText = getItemTodoText(row);
  delItem(row);
  addItem(strTime, 0, strTodoText);

  refreshAlarm();

  setCurrentIndex(getCount() - 1);

  isNeedSave = true;
  saveTodo();
}

void Todo::on_SetAlarm() {
  int row = getCurrentIndex();
  if (row < 0) return;
  QString strTodoText = getItemTodoText(row);
  QString strTime;

  if (!mw_one->m_TodoAlarm->ui->chk1->isChecked() &&
      !mw_one->m_TodoAlarm->ui->chk2->isChecked() &&
      !mw_one->m_TodoAlarm->ui->chk3->isChecked() &&
      !mw_one->m_TodoAlarm->ui->chk4->isChecked() &&
      !mw_one->m_TodoAlarm->ui->chk5->isChecked() &&
      !mw_one->m_TodoAlarm->ui->chk6->isChecked() &&
      !mw_one->m_TodoAlarm->ui->chk7->isChecked()) {
    mw_one->m_TodoAlarm->ui->chkDaily->setChecked(false);
  }

  if (mw_one->m_TodoAlarm->ui->chkDaily->isChecked()) {
    QString str;
    if (mw_one->m_TodoAlarm->ui->chk1->isChecked()) str = str + "1";
    if (mw_one->m_TodoAlarm->ui->chk2->isChecked()) str = str + "2";
    if (mw_one->m_TodoAlarm->ui->chk3->isChecked()) str = str + "3";
    if (mw_one->m_TodoAlarm->ui->chk4->isChecked()) str = str + "4";
    if (mw_one->m_TodoAlarm->ui->chk5->isChecked()) str = str + "5";
    if (mw_one->m_TodoAlarm->ui->chk6->isChecked()) str = str + "6";
    if (mw_one->m_TodoAlarm->ui->chk7->isChecked()) str = str + "7";

    strTime = tr("Alarm") + "  " + str + "  " +
              mw_one->m_TodoAlarm->ui->dateTimeEdit->time().toString("HH:mm");

  } else {
    strTime =
        tr("Alarm") + "  " + mw_one->m_TodoAlarm->ui->dateTimeEdit->text();
  }

  delItem(row);
  insertItem(strTime, 0, strTodoText, row);
  setCurrentIndex(row);

  ui->frameSetTime->hide();
  mw_one->m_TodoAlarm->close();

  refreshTableLists();
  refreshAlarm();
  isNeedSave = true;
  saveTodo();
}

bool Todo::isWeekValid(QString lblDateTime, QString strDate) {
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

qlonglong Todo::getSecond(QString strDateTime) {
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

void Todo::on_btnSetTime_clicked() {
  int row = getCurrentIndex();
  if (row < 0) return;

  delete mw_one->m_TodoAlarm;
  mw_one->m_TodoAlarm = new TodoAlarm(this);

  QString str = getItemTime(row);
  QDate date;
  QTime time;
  mw_one->m_TodoAlarm->ui->chk1->setChecked(false);
  mw_one->m_TodoAlarm->ui->chk2->setChecked(false);
  mw_one->m_TodoAlarm->ui->chk3->setChecked(false);
  mw_one->m_TodoAlarm->ui->chk4->setChecked(false);
  mw_one->m_TodoAlarm->ui->chk5->setChecked(false);
  mw_one->m_TodoAlarm->ui->chk6->setChecked(false);
  mw_one->m_TodoAlarm->ui->chk7->setChecked(false);
  mw_one->m_TodoAlarm->ui->chkDaily->setChecked(false);

  str = getTimeStr(str);

  if (str != "") {
    QStringList list = str.split(" ");
    if (str.contains("-")) {
      date = QDate::fromString(list.at(0), "yyyy-M-d");
      time = QTime::fromString(list.at(1), "HH:mm");
    } else {
      QString s1 = list.at(0);
      for (int i = 0; i < s1.length(); i++) {
        QString s2 = s1.mid(i, 1);
        if (s2 == "1") mw_one->m_TodoAlarm->ui->chk1->setChecked(true);
        if (s2 == "2") mw_one->m_TodoAlarm->ui->chk2->setChecked(true);
        if (s2 == "3") mw_one->m_TodoAlarm->ui->chk3->setChecked(true);
        if (s2 == "4") mw_one->m_TodoAlarm->ui->chk4->setChecked(true);
        if (s2 == "5") mw_one->m_TodoAlarm->ui->chk5->setChecked(true);
        if (s2 == "6") mw_one->m_TodoAlarm->ui->chk6->setChecked(true);
        if (s2 == "7") mw_one->m_TodoAlarm->ui->chk7->setChecked(true);
      }
      date = QDate::currentDate();

      for (int i = 0; i < list.count(); i++) {
        if (list.at(i).contains(":")) {
          time = QTime::fromString(list.at(i), "HH:mm");
          break;
        }
      }

      mw_one->m_TodoAlarm->ui->chkDaily->setChecked(true);
    }

    mw_one->m_TodoAlarm->ui->dateTimeEdit->setDate(date);
    mw_one->m_TodoAlarm->ui->dateTimeEdit->setTime(time);

  } else {
    str = getItemTime(row);
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
        if (s2 == "1") mw_one->m_TodoAlarm->ui->chk1->setChecked(true);
        if (s2 == "2") mw_one->m_TodoAlarm->ui->chk2->setChecked(true);
        if (s2 == "3") mw_one->m_TodoAlarm->ui->chk3->setChecked(true);
        if (s2 == "4") mw_one->m_TodoAlarm->ui->chk4->setChecked(true);
        if (s2 == "5") mw_one->m_TodoAlarm->ui->chk5->setChecked(true);
        if (s2 == "6") mw_one->m_TodoAlarm->ui->chk6->setChecked(true);
        if (s2 == "7") mw_one->m_TodoAlarm->ui->chk7->setChecked(true);
      }
      date = QDate::currentDate();
      for (int i = 0; i < list.count(); i++) {
        if (list.at(i).contains(":")) {
          time = QTime::fromString(list.at(i), "HH:mm");
          break;
        }
      }

      mw_one->m_TodoAlarm->ui->chkDaily->setChecked(true);
    }

    mw_one->m_TodoAlarm->ui->dateTimeEdit->setDate(date);
    mw_one->m_TodoAlarm->ui->dateTimeEdit->setTime(time);
  }

  mw_one->m_TodoAlarm->initDlg();
  QString txt = tr("Todo") + " : " + getItemTodoText(row);
  txt = txt.replace("\n", " ");
  QFontMetrics fm(this->font());
  QString qsLine = fm.elidedText(txt, Qt::ElideRight, mw_one->width() - 10);
  mw_one->m_TodoAlarm->ui->lblTodoText->setText(qsLine);
  mw_one->m_TodoAlarm->show();
}

void Todo::on_DelAlarm() {
  int row = getCurrentIndex();
  if (row < 0) return;

  QString str = getItemTime(row);
  QString str1 = str;
  str = getTimeStr(str);
  if (str != "") str1 = str;
  modifyTime(row, str1);
  ui->frameSetTime->hide();
  mw_one->m_TodoAlarm->close();

  refreshTableLists();
  refreshAlarm();
  isNeedSave = true;
  saveTodo();
}

void Todo::startTimerAlarm(QString text) {
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

void Todo::stopTimerAlarm() {
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

void Todo::sendMsgAlarm(QString text) {
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

void Todo::on_btnRecycle_clicked() {
  mw_one->ui->frameTodo->hide();
  mw_one->ui->frameTodoRecycle->show();
}

void Todo::on_btnReturn_clicked() {
  mw_one->ui->frameTodoRecycle->hide();
  mw_one->ui->frameTodo->show();
}

void Todo::on_btnClear_clicked() {
  clearAllRecycle();

  isNeedSave = true;
  saveTodo();
}

void Todo::on_btnRestore_clicked() {
  if (getCountRecycle() == 0) return;

  int row = getCurrentIndexRecycle();
  QString strTime = QDate::currentDate().toString("ddd MM dd yyyy") + "  " +
                    QTime::currentTime().toString();
  QString strText = getItemTodoTextRecycle(row);
  addItem(strTime, 0, strText);

  on_btnDel_clicked();

  setCurrentIndex(getCount() - 1);

  isNeedSave = true;
  saveTodo();
}

void Todo::on_btnDel_clicked() {
  int row = getCurrentIndexRecycle();
  if (row < 0) return;
  delItemRecycle(row);

  isNeedSave = true;
  saveTodo();
}

void Todo::refreshTableLists() {
  tableLists.clear();
  int count_items = getCount();

  for (int i = 0; i < count_items; i++) {
    QString strTime = getItemTime(i);
    QString strText = getItemTodoText(i);

    tableLists.append(strTime + "|=|" + strText);
  }
}

void Todo::refreshTableListsFromIni() {
  tableLists.clear();

  iniTodo = new QSettings(iniDir + "todo.ini", QSettings::IniFormat, this);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  iniTodo->setIniCodec("utf-8");
#endif

  int count_items = iniTodo->value("/Todo/Count", 0).toInt();

  for (int i = 0; i < count_items; i++) {
    QString strTime =
        iniTodo->value("/Todo/Time" + QString::number(i)).toString();
    QString strText =
        iniTodo->value("/Todo/Item" + QString::number(i)).toString();

    tableLists.append(strTime + "|=|" + strText);
  }
}

QString Todo::getTimeStr(QString str) {
  bool isTime = false;
  if (str.contains("定时提醒")) {
    str = str.replace("定时提醒", "").trimmed();
    isTime = true;
  }

  if (str.contains("Alarm")) {
    str = str.replace("Alarm", "").trimmed();
    isTime = true;
  }

  if (isTime) return str;

  return "";
}

void Todo::refreshAlarm() {
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

      str = getTimeStr(str);

      if (str != "") {
        modifyType(i, 3);

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

      strTime = getTimeStr(strTime);

      if (strTime != "") {
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

  changeTodoIcon(isToday);

  Reg.setValue("count", count);
  QString strMute = "true";
  Reg.setValue("mute", strMute);

  if (!QFile(ini_file).exists())
    qDebug() << "ini no exists";
  else
    qDebug() << "ini ok";
}

void Todo::changeTodoIcon(bool isToday) {
  if (!isToday) {
    if (isDark)
      mw_one->ui->btnTodo->setIcon(QIcon(":/res/todo_l.png"));
    else
      mw_one->ui->btnTodo->setIcon(QIcon(":/res/todo.png"));
  } else {
    mw_one->ui->btnTodo->setIcon(QIcon(":/res/todo1.png"));
  }
}

void Todo::on_editTodo_textChanged() {
  int h = getEditTextHeight(mw_one->ui->editTodo) + 4;
  mw_one->ui->editTodo->setFixedHeight(h);
}

void Todo::insertItem(QString strTime, int type, QString strText,
                      int curIndex) {
  int itemheight = setItemHeight(strText);

  QQuickItem* root = mw_one->ui->qwTodo->rootObject();
  QMetaObject::invokeMethod(
      (QObject*)root, "insertItem", Q_ARG(QVariant, strTime),
      Q_ARG(QVariant, type), Q_ARG(QVariant, strText),
      Q_ARG(QVariant, itemheight), Q_ARG(QVariant, curIndex));
}

void Todo::insertRecycle(QString strTime, int type, QString strText,
                         int curIndex) {
  int itemheight = setItemHeight(strText);

  QQuickItem* root = mw_one->ui->qwRecycle->rootObject();
  QMetaObject::invokeMethod(
      (QObject*)root, "insertRecycle", Q_ARG(QVariant, strTime),
      Q_ARG(QVariant, type), Q_ARG(QVariant, strText),
      Q_ARG(QVariant, itemheight), Q_ARG(QVariant, curIndex));
}

int Todo::getCurrentIndex() {
  QQuickItem* root = mw_one->ui->qwTodo->rootObject();
  QVariant itemIndex;
  QMetaObject::invokeMethod((QObject*)root, "getCurrentIndex",
                            Q_RETURN_ARG(QVariant, itemIndex));
  return itemIndex.toInt();
}

int Todo::getCurrentIndexRecycle() {
  QQuickItem* root = mw_one->ui->qwRecycle->rootObject();
  QVariant itemIndex;
  QMetaObject::invokeMethod((QObject*)root, "getCurrentIndex",
                            Q_RETURN_ARG(QVariant, itemIndex));
  return itemIndex.toInt();
}

QString Todo::getItemTime(int index) {
  QQuickItem* root = mw_one->ui->qwTodo->rootObject();
  QVariant itemTime;
  QMetaObject::invokeMethod((QObject*)root, "getTime",
                            Q_RETURN_ARG(QVariant, itemTime),
                            Q_ARG(QVariant, index));
  return itemTime.toString();
}

QString Todo::getItemTimeRecycle(int index) {
  QQuickItem* root = mw_one->ui->qwRecycle->rootObject();
  QVariant itemTime;
  QMetaObject::invokeMethod((QObject*)root, "getTime",
                            Q_RETURN_ARG(QVariant, itemTime),
                            Q_ARG(QVariant, index));
  return itemTime.toString();
}

int Todo::getItemType(int index) {
  QQuickItem* root = mw_one->ui->qwTodo->rootObject();
  QVariant itemType;
  QMetaObject::invokeMethod((QObject*)root, "getType",
                            Q_RETURN_ARG(QVariant, itemType),
                            Q_ARG(QVariant, index));
  return itemType.toInt();
}

QString Todo::getItemTodoText(int index) {
  QQuickItem* root = mw_one->ui->qwTodo->rootObject();
  QVariant itemTodoText;
  QMetaObject::invokeMethod((QObject*)root, "getTodoText",
                            Q_RETURN_ARG(QVariant, itemTodoText),
                            Q_ARG(QVariant, index));
  return itemTodoText.toString();
}

QString Todo::getItemTodoTextRecycle(int index) {
  QQuickItem* root = mw_one->ui->qwRecycle->rootObject();
  QVariant itemTodoText;
  QMetaObject::invokeMethod((QObject*)root, "getTodoText",
                            Q_RETURN_ARG(QVariant, itemTodoText),
                            Q_ARG(QVariant, index));
  return itemTodoText.toString();
}

void Todo::delItem(int index) {
  QQuickItem* root = mw_one->ui->qwTodo->rootObject();
  QMetaObject::invokeMethod((QObject*)root, "delItem", Q_ARG(QVariant, index));
}

void Todo::delItemRecycle(int index) {
  QQuickItem* root = mw_one->ui->qwRecycle->rootObject();
  QMetaObject::invokeMethod((QObject*)root, "delItem", Q_ARG(QVariant, index));
}

void Todo::addItem(QString strTime, int type, QString strText) {
  int itemheight = setItemHeight(strText);

  QQuickItem* root = mw_one->ui->qwTodo->rootObject();
  QMetaObject::invokeMethod((QObject*)root, "addItem", Q_ARG(QVariant, strTime),
                            Q_ARG(QVariant, type), Q_ARG(QVariant, strText),
                            Q_ARG(QVariant, itemheight));
}

void Todo::addItemRecycle(QString strTime, int type, QString strText) {
  int itemheight = setItemHeight(strText);

  QQuickItem* root = mw_one->ui->qwRecycle->rootObject();
  QMetaObject::invokeMethod((QObject*)root, "addItem", Q_ARG(QVariant, strTime),
                            Q_ARG(QVariant, type), Q_ARG(QVariant, strText),
                            Q_ARG(QVariant, itemheight));
}

void Todo::setCurrentIndex(int index) {
  QQuickItem* root = mw_one->ui->qwTodo->rootObject();
  QMetaObject::invokeMethod((QObject*)root, "setCurrentItem",
                            Q_ARG(QVariant, index));
}

void Todo::setHighPriority(bool isBool) {
  QQuickItem* root = mw_one->ui->qwTodo->rootObject();
  QMetaObject::invokeMethod((QObject*)root, "setHighPriority",
                            Q_ARG(QVariant, isBool));
}

int Todo::setItemHeight(QString strTodoText) {
  QFont font = this->font();
  font.setPointSize(fontSize - 2);

  QFontMetrics fm(font);
  int fontHeight = fm.height();

  return fontHeight;

  QTextEdit* edit = new QTextEdit;
  edit->append(strTodoText);
  int itemHeight = fontHeight * 2 + getEditTextHeight(edit);

  return itemHeight;
}

int Todo::getCount() {
  QQuickItem* root = mw_one->ui->qwTodo->rootObject();
  QVariant itemCount;
  QMetaObject::invokeMethod((QObject*)root, "getItemCount",
                            Q_RETURN_ARG(QVariant, itemCount));
  return itemCount.toInt();
}

int Todo::getCountRecycle() {
  QQuickItem* root = mw_one->ui->qwRecycle->rootObject();
  QVariant itemCount;
  QMetaObject::invokeMethod((QObject*)root, "getItemCount",
                            Q_RETURN_ARG(QVariant, itemCount));
  return itemCount.toInt();
}

void Todo::modifyTime(int index, QString strTime) {
  QQuickItem* root = mw_one->ui->qwTodo->rootObject();
  QMetaObject::invokeMethod((QObject*)root, "modifyItemTime",
                            Q_ARG(QVariant, index), Q_ARG(QVariant, strTime));
}

void Todo::modifyType(int index, int type) {
  QQuickItem* root = mw_one->ui->qwTodo->rootObject();
  QMetaObject::invokeMethod((QObject*)root, "modifyItemType",
                            Q_ARG(QVariant, index), Q_ARG(QVariant, type));
}

void Todo::modifyTodoText(int index, QString strTodoText) {
  QQuickItem* root = mw_one->ui->qwTodo->rootObject();
  QMetaObject::invokeMethod((QObject*)root, "modifyItemText",
                            Q_ARG(QVariant, index),
                            Q_ARG(QVariant, strTodoText));
}

void Todo::clearAll() {
  int count = getCount();
  for (int i = 0; i < count; i++) {
    delItem(0);
  }
}

void Todo::clearAllRecycle() {
  int count = getCountRecycle();
  for (int i = 0; i < count; i++) {
    delItemRecycle(0);
  }
}

void Todo::isAlarm(int index) {
  bool a = false;
  QString strTime = getItemTime(index);

  strTime = getTimeStr(strTime);
  if (strTime != "") a = true;
  qDebug() << "aabb" << a;
  setHighPriority(a);
}

void Todo::reeditText() {
  int row = getCurrentIndex();
  if (row < 0) return;

  QDialog* dlg = new QDialog(this);
  QVBoxLayout* vbox0 = new QVBoxLayout;
  dlg->setLayout(vbox0);
  vbox0->setContentsMargins(5, 5, 5, 5);
  dlg->setModal(true);
  dlg->setWindowFlag(Qt::FramelessWindowHint);

  QFrame* frame = new QFrame(this);
  vbox0->addWidget(frame);

  QVBoxLayout* vbox = new QVBoxLayout;

  frame->setLayout(vbox);
  vbox->setContentsMargins(6, 6, 6, 10);
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
  edit->verticalScrollBar()->setStyleSheet(
      mw_one->ui->editDetails->verticalScrollBar()->styleSheet());
  m_Method->setSCrollPro(edit);

  QToolButton* btnCancel = new QToolButton(this);
  QToolButton* btnCopy = new QToolButton(this);
  QToolButton* btnOk = new QToolButton(this);
  btnCancel->setText(tr("Cancel"));
  btnCopy->setText(tr("Copy"));
  btnOk->setText(tr("OK"));

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
  connect(dlg, &QDialog::rejected,
          [=]() mutable { m_Method->closeGrayWindows(); });
  connect(dlg, &QDialog::accepted,
          [=]() mutable { m_Method->closeGrayWindows(); });
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
    isNeedSave = true;
    saveTodo();
    dlg->close();
  });

  int x, y, w, h;
  w = mw_one->width() - 20;
  x = mw_one->geometry().x() + (mw_one->width() - w) / 2;
  h = mw_one->height() / 3;
  y = geometry().y() + (height() - h) / 4;
  dlg->setGeometry(x, y, w, h);
  dlg->setModal(true);
  m_Method->showGrayWindows();
  mw_one->set_ToolButtonStyle(dlg);
  dlg->show();
}

void Todo::addToRecycle() {
  int row = getCurrentIndex();
  QString strTodoText = getItemTodoText(row);
  QString doneTime =
      QDateTime::currentDateTime().toString() + "  " + tr("Done");

  insertRecycle(doneTime, 0, strTodoText, 0);

  isNeedSave = true;
}
