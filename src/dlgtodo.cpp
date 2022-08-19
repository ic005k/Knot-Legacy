#include "dlgtodo.h"

#include "mainwindow.h"
#include "smoothscrollbar.h"
#include "ui_dlgtodo.h"
#include "ui_mainwindow.h"

QString highLblStyle = "color:rgb(212,35,122)";
// "color:rgb(212,35,122)";
// "background-color: rgb(254,234,112);color:rgb(39,36,17)";
// "background-color: rgb(255,255,255);color:rgb(212,35,122)";
int highCount;
QString orgLblStyle;
QListWidget *mylist, *listRecycle;
extern MainWindow* mw_one;
extern QString iniFile, iniDir;
extern bool loading, isBreak, isImport;
extern int fontSize;

dlgTodo::dlgTodo(QWidget* parent) : QDialog(parent), ui(new Ui::dlgTodo) {
  ui->setupUi(this);
  this->installEventFilter(this);
  ui->frameSetTime->hide();
  ui->frameRecycle->hide();
  ui->listRecycle->hide();
  ui->lblRecycle->hide();

  QString strTar = "/data/data/com.x/files/msg.mp3";
  QFile::copy(":/res/msg.mp3", strTar);

  mylist = new QListWidget;
  mylist = ui->listWidget;
  listRecycle = new QListWidget;
  listRecycle = ui->listRecycle;
  // ui->listRecycle->setStyleSheet("text-decoration: line-through;");
  ui->listRecycle->setWordWrap(true);

  ui->listWidget->setVerticalScrollMode(QListWidget::ScrollPerPixel);
  QScroller::grabGesture(ui->listWidget, QScroller::LeftMouseButtonGesture);
  ui->listWidget->horizontalScrollBar()->setHidden(true);

  ui->listRecycle->setVerticalScrollMode(QListWidget::ScrollPerPixel);
  QScroller::grabGesture(ui->listRecycle, QScroller::LeftMouseButtonGesture);
  ui->listRecycle->horizontalScrollBar()->setHidden(true);
  ui->listRecycle->verticalScrollBar()->setStyleSheet(mw_one->vsbarStyleSmall);

  /*QScrollerProperties sp;
  sp.setScrollMetric(QScrollerProperties::DragStartDistance, 0.00001);
  sp.setScrollMetric(QScrollerProperties::ScrollingCurve, QEasingCurve::Linear);
  QScroller* qs = QScroller::scroller(ui->listWidget);
  QScroller* qs1 = QScroller::scroller(ui->listRecycle);
  qs->setScrollerProperties(sp);
  qs1->setScrollerProperties(sp);*/
  mw_one->setSCrollPro(ui->listWidget);
  mw_one->setSCrollPro(ui->listRecycle);

  ui->btnAdd->setStyleSheet("border:none");
  ui->btnBack->setStyleSheet("border:none");
  ui->btnHigh->setStyleSheet("border:none");
  ui->btnLow->setStyleSheet("border:none");
  ui->btnModify->setStyleSheet("border:none");
  ui->btnSetTime->setStyleSheet("border:none");
  ui->btnRecycle->setStyleSheet("border:none");

  ui->textEdit->setFixedHeight(getEditTextHeight(ui->textEdit) + 2);
}

dlgTodo::~dlgTodo() { delete ui; }

void dlgTodo::keyReleaseEvent(QKeyEvent* event) {
  Q_UNUSED(event);
  // event->accept();
}

void dlgTodo::on_btnBack_clicked() { close(); }

void dlgTodo::saveTodo() {
  highCount = 0;
  QSettings Reg(iniDir + "todo.ini", QSettings::IniFormat);
  Reg.setIniCodec("utf-8");
  int count = mylist->count();
  Reg.setValue("/Todo/Count", count);
  for (int i = 0; i < count; i++) {
    if (isBreak) break;
    QListWidgetItem* item = mylist->item(i);
    QWidget* w = mylist->itemWidget(item);
    QLabel* lbl = (QLabel*)w->children().at(2)->children().at(2);
    QString str = lbl->text();
    QLabel* lblTime = (QLabel*)w->children().at(2)->children().at(1);
    QString strTime = lblTime->text();
    Reg.setValue("/Todo/Item" + QString::number(i), str);
    Reg.setValue("/Todo/Time" + QString::number(i), strTime);

    QLabel* lblSn = (QLabel*)w->children().at(2);
    if (orgLblStyle != lblSn->styleSheet()) {
      highCount++;
      Reg.setValue("/Todo/HighLightSn" + QString::number(highCount), i);
    }
  }

  int count1 = listRecycle->count();
  Reg.setValue("/Todo/Count1", count1);
  for (int i = 0; i < count1; i++) {
    QString str = listRecycle->item(i)->text().trimmed();
    Reg.setValue("/Todo/ItemRecycle" + QString::number(i), str);
  }

  Reg.setValue("/Todo/HighCount", highCount);
}

void dlgTodo::init_Items() {
  ui->listWidget->clear();
  ui->listRecycle->clear();
  QString ini_file;
  if (isImport)
    ini_file = iniFile;
  else
    ini_file = iniDir + "todo.ini";
  QSettings Reg(ini_file, QSettings::IniFormat);
  Reg.setIniCodec("utf-8");
  int count = Reg.value("/Todo/Count").toInt();
  for (int i = 0; i < count; i++) {
    QString str = Reg.value("/Todo/Item" + QString::number(i)).toString();
    QString strTime = Reg.value("/Todo/Time" + QString::number(i)).toString();
    add_Item(str, strTime, false);
  }

  int count1 = Reg.value("/Todo/Count1").toInt();
  for (int i = 0; i < count1; i++) {
    QString str =
        Reg.value("/Todo/ItemRecycle" + QString::number(i)).toString();
    ui->listRecycle->addItem(str);
  }

  highCount = Reg.value("/Todo/HighCount").toInt();
  for (int i = 0; i < highCount; i++) {
    int index = Reg.value("/Todo/HighLightSn" + QString::number(i + 1)).toInt();
    QListWidgetItem* item = ui->listWidget->item(index);
    QWidget* w = ui->listWidget->itemWidget(item);
    QLabel* lbl = (QLabel*)w->children().at(2);
    lbl->setStyleSheet(highLblStyle);
  }

  refreshAlarm();
}

void dlgTodo::on_btnAdd_clicked() {
  QString str = ui->textEdit->toPlainText().trimmed();
  for (int i = 0; i < ui->listWidget->count(); i++) {
    QLabel* lbl = getMainLabel(i);
    if (lbl->text() == str) {
      ui->listWidget->setCurrentRow(i);
      return;
    }
  }
  add_Item(str,
           QDate::currentDate().toString("ddd MM dd yyyy") + "  " +
               QTime::currentTime().toString(),
           true);
  ui->textEdit->setText("");
  ui->listWidget->verticalScrollBar()->setSliderPosition(0);
}

void dlgTodo::add_Item(QString str, QString time, bool insert) {
  if (str == "") return;
  QListWidgetItem* pItem = new QListWidgetItem;
  pItem->setText("");

  if (insert)
    ui->listWidget->insertItem(0, pItem);
  else
    ui->listWidget->addItem(pItem);

  QWidget* w = new QWidget;
  w->setContentsMargins(0, 0, 0, 0);

  QHBoxLayout* layout = new QHBoxLayout;
  layout->setMargin(0);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);
  QToolButton* btn = new QToolButton(this);
  btn->setStyleSheet("border:none");

  btn->setIconSize(QSize(25, 25));
  btn->setIcon(QIcon(":/res/done.png"));

  connect(btn, &QToolButton::clicked, [=]() {
    if (isModi) {
      lblModi->setHidden(false);
      editModi->setHidden(true);
      ui->btnModify->setText(tr("Modify"));
      isModi = false;
    }
    btn->setIcon(QIcon(":/res/done1.png"));

    mw_one->Sleep(400);

    ui->listWidget->setCurrentItem(pItem);
    int row = ui->listWidget->currentRow();
    QString str = getMainLabel(row)->text().trimmed();
    QListWidgetItem* item = new QListWidgetItem;
    item->setSizeHint(QSize(ui->listWidget->width() - 16, 80));
    item->setText(QDateTime::currentDateTime().toString() + "  " + tr("Done") +
                  "\n" + str);
    ui->listRecycle->insertItem(0, item);
    ui->listWidget->takeItem(row);
    int index = ui->listWidget->currentRow();
    add_ItemSn(index);

    refreshAlarm();
  });

  QFont font = this->font();
  font.setPointSize(fontSize);
  QFontMetrics fm(font);
  int fontHeight = fm.height();

  QLabel* lblSn = new QLabel(this);
  lblSn->setFont(font);
  lblSn->setFixedWidth(fontSize * 1.5);

  QLabel* label = new QLabel(this);
  label->setFont(font);
  label->installEventFilter(this);
  label->adjustSize();
  label->setWordWrap(true);
  label->setText(str);
  orgLblStyle = label->styleSheet();

  QTextEdit* edit = new QTextEdit(this);
  QScroller::grabGesture(edit, QScroller::LeftMouseButtonGesture);
  mw_one->setSCrollPro(edit);
  edit->setWordWrapMode(QTextOption::WordWrap);
  edit->setHidden(true);
  connect(edit, &QTextEdit::textChanged, [=]() {
    if (isModi) {
      label->setText(edit->toPlainText().trimmed());

      int itemHeight = fontHeight * 2 + getEditTextHeight(edit);
      pItem->setSizeHint(QSize(ui->listWidget->width() - 20, itemHeight));
    }
  });

  QFrame* frame = new QFrame(this);
  QHBoxLayout* hbox = new QHBoxLayout();
  hbox->addWidget(label);
  hbox->addWidget(edit);
  QVBoxLayout* vbox = new QVBoxLayout();
  QLabel* lblTime = new QLabel(this);
  lblTime->setFixedHeight(fontHeight);
  QFont f;
  int fsize = fontSize * 0.9;
  if (fsize < 13) fsize = 13;
  f.setPointSize(fsize);
  lblTime->setFont(f);
  lblTime->setText(time);
  lblTime->setStyleSheet("QLabel{color:rgb(80,80,80);}");
  if (time.contains(tr("Alarm"))) {
    lblTime->setStyleSheet(alarmStyle);
    // lblTime->setStyleSheet(mw_one->mydlgSetTime->ui->lblTitle->styleSheet());
    f.setBold(true);
    lblTime->setFont(f);
  }
  vbox->addWidget(lblTime);
  vbox->addLayout(hbox);
  frame->setLayout(vbox);
  frame->layout()->setMargin(0);
  frame->layout()->setContentsMargins(0, 0, 0, 0);

  layout->addWidget(lblSn);
  layout->addWidget(frame);
  layout->addWidget(btn);
  w->setLayout(layout);

  ui->listWidget->setItemWidget(pItem, w);

  edit->clear();
  edit->setPlainText(str);
  int itemHeight = fontHeight * 2 + getEditTextHeight(edit);
  pItem->setSizeHint(QSize(ui->listWidget->width() - 20, itemHeight));
  qDebug() << fontHeight << label->height() << itemHeight;

  add_ItemSn(0);
}

void dlgTodo::add_ItemSn(int index) {
  for (int i = 0; i < ui->listWidget->count(); i++) {
    QListWidgetItem* item = ui->listWidget->item(i);
    QWidget* w = ui->listWidget->itemWidget(item);
    QLabel* lbl = (QLabel*)w->children().at(1);

    lbl->setText(QString::number(i + 1) + ". ");
  }
  ui->listWidget->setCurrentRow(index);
}

int dlgTodo::getEditTextHeight(QTextEdit* edit) {
  QTextDocument* doc = edit->document();
  doc->adjustSize();
  int mainHeight = doc->size().rheight() * 1.06;
  return mainHeight;
}

void dlgTodo::on_listWidget_itemClicked(QListWidgetItem* item) {
  QWidget* w = ui->listWidget->itemWidget(item);
  qDebug() << w->children();

  if (isModi) {
    lblModi->setHidden(false);
    editModi->setHidden(true);
    ui->btnModify->setText(tr("Modify"));
    isModi = false;
  }
}

void dlgTodo::on_listWidget_itemDoubleClicked(QListWidgetItem* item) {
  Q_UNUSED(item);
  on_btnModify_clicked();
}

void dlgTodo::on_listWidget_currentRowChanged(int currentRow) {
  Q_UNUSED(currentRow);
  // if (editItem != NULL) ui->listWidget->closePersistentEditor(editItem);
}

void dlgTodo::closeEvent(QCloseEvent* event) {
  Q_UNUSED(event);

  mw_one->startSave("todo");
  refreshAlarm();
}

bool dlgTodo::eventFilter(QObject* watch, QEvent* evn) {
  if (loading) return QWidget::eventFilter(watch, evn);

  if (evn->type() == QEvent::KeyPress) {
    QKeyEvent* keyEvent = static_cast<QKeyEvent*>(evn);
    if (keyEvent->key() == Qt::Key_Back) {
      if (ui->listRecycle->isHidden())
        on_btnBack_clicked();
      else
        on_btnReturn_clicked();

      return true;
    }
  }

  return QWidget::eventFilter(watch, evn);
}

void dlgTodo::on_btnModify_clicked() {
  int row = ui->listWidget->currentRow();
  if (row < 0) return;

  if (ui->btnModify->text() == tr("Modify")) {
    QListWidgetItem* item = ui->listWidget->currentItem();
    QWidget* w = ui->listWidget->itemWidget(item);
    QLabel* lbl = (QLabel*)w->children().at(2)->children().at(2);
    QTextEdit* edit = (QTextEdit*)w->children().at(2)->children().at(3);
    lblModi = lbl;
    editModi = edit;
    edit->setPlainText(lbl->text());
    lbl->setHidden(true);
    edit->setHidden(false);
    ui->btnModify->setText(tr("Finish Editing"));
    isModi = true;
  } else if (ui->btnModify->text() == tr("Finish Editing")) {
    lblModi->setHidden(false);
    editModi->setHidden(true);
    ui->btnModify->setText(tr("Modify"));
    isModi = false;
  }
}

void dlgTodo::on_btnHigh_clicked() {
  int row = ui->listWidget->currentRow();
  if (row < 0) return;
  QListWidgetItem* item = ui->listWidget->currentItem();
  QWidget* w = ui->listWidget->itemWidget(item);
  QLabel* lbl = (QLabel*)w->children().at(2)->children().at(2);
  ui->listWidget->takeItem(row);
  QString str = lbl->text();
  QLabel* lblTime = (QLabel*)w->children().at(2)->children().at(1);
  add_Item(str, lblTime->text(), true);
  QListWidgetItem* item1 = ui->listWidget->currentItem();
  QWidget* w1 = ui->listWidget->itemWidget(item1);
  QFrame* frame = (QFrame*)w1->children().at(2);
  frame->setStyleSheet(highLblStyle);
  ui->listWidget->scrollToTop();
}

void dlgTodo::on_btnLow_clicked() {
  int row = ui->listWidget->currentRow();
  if (row < 0) return;
  QListWidgetItem* item = ui->listWidget->currentItem();
  QWidget* w = ui->listWidget->itemWidget(item);
  QLabel* lbl = (QLabel*)w->children().at(2)->children().at(2);
  ui->listWidget->takeItem(row);
  QString str = lbl->text();
  QLabel* lblTime = (QLabel*)w->children().at(2)->children().at(1);
  add_Item(str, lblTime->text(), false);
  ui->listWidget->setCurrentRow(ui->listWidget->count() - 1);
  ui->listWidget->scrollToBottom();
}

QLabel* dlgTodo::getTimeLabel(int index) {
  QListWidgetItem* item = ui->listWidget->item(index);
  QWidget* w = ui->listWidget->itemWidget(item);
  QLabel* lbl = (QLabel*)w->children().at(2)->children().at(1);
  return lbl;
}

QLabel* dlgTodo::getMainLabel(int index) {
  QListWidgetItem* item = ui->listWidget->item(index);
  QWidget* w = ui->listWidget->itemWidget(item);
  QLabel* lbl = (QLabel*)w->children().at(2)->children().at(2);
  return lbl;
}

void dlgTodo::on_btnOK_clicked() {
  QLabel* lbl = getTimeLabel(ui->listWidget->currentRow());
  lbl->setText(tr("Alarm") + "  " + mw_one->mymsgDlg->ui->dateTimeEdit->text());
  lbl->setStyleSheet(alarmStyle);
  // lbl->setStyleSheet(mw_one->mydlgSetTime->ui->lblTitle->styleSheet());
  QFont f = lbl->font();
  f.setBold(true);
  lbl->setFont(f);
  ui->frameSetTime->hide();
  mw_one->mymsgDlg->close();
  refreshAlarm();
}

qlonglong dlgTodo::getSecond(QString strDateTime) {
  // 2022-8-22 18:18
  strDateTime = strDateTime + ":00";
  QString strCur = QDateTime::currentDateTime().toString("yyyy-M-d HH:mm:ss");
  QDateTime timeCur = QDateTime::fromString(strCur, "yyyy-M-d HH:mm:ss");
  QDateTime timeAlarm = QDateTime::fromString(strDateTime, "yyyy-M-d HH:mm:ss");
  qlonglong seconds = timeCur.secsTo(timeAlarm);
  // qDebug() << strCur << strDateTime << seconds << timeCur << timeAlarm;
  return seconds;
}

void dlgTodo::on_btnSetTime_clicked() {
  if (!ui->listWidget->currentIndex().isValid()) return;

  QLabel* lblMain = getMainLabel(ui->listWidget->currentRow());
  QLabel* lbl = getTimeLabel(ui->listWidget->currentRow());
  QString str = lbl->text().trimmed();
  if (str.contains(tr("Alarm")) || str.mid(0, 2) == "20") {
    str = str.replace(tr("Alarm"), "").trimmed();
    QStringList list = str.split(" ");
    QDate date;
    QTime time;
    date = QDate::fromString(list.at(0), "yyyy-M-d");
    time = QTime::fromString(list.at(1), "HH:mm");
    mw_one->mymsgDlg->ui->dateTimeEdit->setDate(date);
    mw_one->mymsgDlg->ui->dateTimeEdit->setTime(time);

  }

  else {
    mw_one->mymsgDlg->ui->dateTimeEdit->setDate(QDate::currentDate());
    mw_one->mymsgDlg->ui->dateTimeEdit->setTime(QTime::currentTime());
  }

  mw_one->mymsgDlg->initDlg();
  QString txt = tr("Todo") + " : " + lblMain->text();
  txt = txt.replace("\n", " ");
  QFontMetrics fm(this->font());
  QString qsLine = fm.elidedText(txt, Qt::ElideRight, this->width() - 10);
  mw_one->mymsgDlg->ui->lblTodoText->setText(qsLine);
  mw_one->mymsgDlg->show();
}

void dlgTodo::on_btnCancel_clicked() {
  QLabel* lbl = getTimeLabel(ui->listWidget->currentRow());
  QString str = lbl->text().trimmed();
  if (str.contains(tr("Alarm"))) str = str.replace(tr("Alarm"), "");
  lbl->setText(str.trimmed());
  lbl->setStyleSheet(getMainLabel(ui->listWidget->currentRow())->styleSheet());
  ui->frameSetTime->hide();
  mw_one->mymsgDlg->close();
  refreshAlarm();
}

void dlgTodo::on_Alarm() {
  int count = 0;
  for (int i = 0; i < ui->listWidget->count(); i++) {
    QLabel* lbl = getTimeLabel(i);
    QString str = lbl->text().trimmed();
    if (str.contains(tr("Alarm"))) {
      count++;
      str = str.replace(tr("Alarm"), "").trimmed();
      QStringList list = str.split(" ");
      if (list.count() == 2) {
        QString date = list.at(0);
        QString time = list.at(1);

        if (QDate::currentDate().toString("yyyy-M-d") == date) {
          int total_m, total_cur_m, h1, m1, cur_h, cur_m;

          QStringList list_m = time.split(":");
          h1 = list_m.at(0).toInt();
          m1 = list_m.at(1).toInt();
          total_m = h1 * 60 + m1;

          cur_h = QTime::currentTime().hour();
          cur_m = QTime::currentTime().minute();
          total_cur_m = cur_h * 60 + cur_m;

          if (total_cur_m >= total_m - 1) {
            // if (QTime::currentTime().toString("HH:mm") >= time) {

            lbl->setText(str);
            saveTodo();
            QString text = getMainLabel(i)->text().trimmed();
            sendMsgAlarm(text);
            lbl->setStyleSheet(getMainLabel(i)->styleSheet());
            QMessageBox msgBox;
            msgBox.setText(tr("Todo"));
            msgBox.setInformativeText(str + "\n\n" + text);
            QPushButton* btnOk =
                msgBox.addButton(tr("Ok"), QMessageBox::AcceptRole);
            btnOk->setFocus();
            // msgBox.exec();

            // msgDlg = new msgDialog(mw_one);
            // msgDlg->initDlg();
            // msgDlg->ui->lblText->setText(tr("Todo") + " : " + str + "\n\n" +
            //                              text);
            // msgDlg->show();

            mw_one->ui->lblInfo->setText(
                tr("Todo") + " : " + str + "\n\n" + text + "\n\n" +
                QDateTime::currentDateTime().toString());
            if (mw_one->mydlgTodo->isHidden()) {
              // mw_one->ui->frameTip->show();
            }

            break;
          }
        }
      }
    }
  }
  if (count == 0) {
    stopTimerAlarm();
    mw_one->ui->btnTodo->setIcon(QIcon(":/res/todo.png"));
  } else {
    mw_one->ui->btnTodo->setIcon(QIcon(":/res/todo1.png"));
  }
}

void dlgTodo::startTimerAlarm(QString text) {
  /*if (mw_one->isHardStepSensor == 1) {
    mw_one->mydlgSteps->releaseWakeLock();
    mw_one->mydlgSteps->acquireWakeLock();
  }*/
#ifdef Q_OS_ANDROID
  QAndroidJniObject jo = QAndroidJniObject::fromString(text);
  // jo.callStaticMethod<int>("com.x/MyService", "startTimerAlarm", "()I");

  jo.callStaticMethod<int>("com.x/MyActivity", "startAlarm",
                           "(Ljava/lang/String;)I", jo.object<jstring>());

  jo.callStaticMethod<int>("com.x/ClockActivity", "setInfoText",
                           "(Ljava/lang/String;)I", jo.object<jstring>());

#endif
}

void dlgTodo::stopTimerAlarm() {
#ifdef Q_OS_ANDROID
  QAndroidJniObject jo = QAndroidJniObject::fromString("stopTimerAlarm");
  // jo.callStaticMethod<int>("com.x/MyService", "stopTimerAlarm", "()I");

  jo.callStaticMethod<int>("com.x/MyActivity", "stopAlarm", "()I");
#endif

  // if (mw_one->isHardStepSensor == 1) mw_one->mydlgSteps->releaseWakeLock();
}

void dlgTodo::sendMsgAlarm(QString text) {
#ifdef Q_OS_ANDROID
  QString strNotify = tr("Todo") + " : " + text;
  QAndroidJniObject javaNotification = QAndroidJniObject::fromString(strNotify);
  QAndroidJniObject::callStaticMethod<void>(
      "com/x/MyService", "notifyAlarm",
      "(Landroid/content/Context;Ljava/lang/String;)V",
      QtAndroid::androidContext().object(), javaNotification.object<jstring>());
#endif
}

void dlgTodo::on_btnRecycle_clicked() {
  ui->lblRecycle->show();
  ui->listRecycle->show();
  ui->listWidget->hide();
  ui->frameRecycle->show();
  ui->frameToolBar->hide();
  ui->frameSetTime->hide();
  ui->textEdit->hide();
  if (ui->listRecycle->count() > 0) {
    ui->listRecycle->setFocus();
    ui->listRecycle->setCurrentRow(0);
  }
}

void dlgTodo::on_btnReturn_clicked() {
  ui->lblRecycle->hide();
  ui->listRecycle->hide();
  ui->listWidget->show();
  ui->frameRecycle->hide();
  ui->frameToolBar->show();
  ui->textEdit->show();
}

void dlgTodo::on_btnClear_clicked() { ui->listRecycle->clear(); }

void dlgTodo::on_btnRestore_clicked() {
  if (ui->listRecycle->count() == 0) return;
  QString str1 = ui->listRecycle->currentItem()->text();
  QStringList list0 = str1.split("\n");
  if (list0.count() > 0) str1 = str1.replace(list0.at(0) + "\n", "");

  add_Item(str1.trimmed(),
           QDate::currentDate().toString("ddd MM dd yyyy") + "  " +
               QTime::currentTime().toString(),
           false);
  ui->listRecycle->takeItem(ui->listRecycle->currentRow());
}

void dlgTodo::on_btnDel_clicked() {
  if (ui->listRecycle->currentIndex().isValid()) {
    int row = ui->listRecycle->currentRow();
    ui->listRecycle->takeItem(row);
  }
}

void dlgTodo::refreshAlarm() {
  stopTimerAlarm();
  int count = 0;
  QString str;
  QLabel* lbl;

  QString ini_file;
  ini_file = "/data/data/com.x/files/msg.ini";
  QSettings Reg(ini_file, QSettings::IniFormat);
  Reg.setIniCodec("utf-8");

  QStringList listAlarm;
  QList<qlonglong> listTotalS;
  for (int i = 0; i < ui->listWidget->count(); i++) {
    lbl = getTimeLabel(i);
    str = lbl->text().trimmed();
    if (str.contains(tr("Alarm"))) {
      str = str.replace(tr("Alarm"), "").trimmed();
      qlonglong totals = getSecond(str);

      if (totals > 0) {
        count++;
        QLabel* lblMain = getMainLabel(i);
        QString str1 = str + "|" + lblMain->text() + "|" +
                       QString::number(totals) + "|" + tr("Close");

        listAlarm.append(str1);
        listTotalS.append(totals);

        // set time marks
        QString strDate = str.split(" ").at(0);
        QString strYear = strDate.split("-").at(0);
        QString strMonth = strDate.split("-").at(1);
        QString strDay = strDate.split("-").at(2);
        if (strDate == QDate::currentDate().toString("yyyy-M-d"))
          lbl->setStyleSheet(alarmStyleToday);
        if (strYear.toInt() == QDate::currentDate().year() &&
            strMonth.toInt() == QDate::currentDate().month()) {
          if (strDay.toInt() - 1 == QDate::currentDate().day())
            lbl->setStyleSheet(alarmStyleTomorrow);
        }

      } else {
        lbl->setText(str);
        lbl->setStyleSheet(getMainLabel(i)->styleSheet());
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

        qDebug() << "Min Time: " << listTotalS << minValue << str1;
        break;
      }
    }
  }

  if (count == 0) {
    mw_one->ui->btnTodo->setIcon(QIcon(":/res/todo.png"));
  } else {
    mw_one->ui->btnTodo->setIcon(QIcon(":/res/todo1.png"));
  }

  Reg.setValue("count", count);
  QString strMute = "false";
  if (mw_one->mydlgPre->ui->chkMute->isChecked()) strMute = "true";
  Reg.setValue("mute", strMute);

  if (!QFileInfo(ini_file).exists())
    qDebug() << "ini no exists";
  else
    qDebug() << "ini ok";
}

void dlgTodo::on_textEdit_textChanged() {
  int h = getEditTextHeight(ui->textEdit) + 2;
  ui->textEdit->setFixedHeight(h);
}
