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

msgDialog* msgDlg;

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

  msgDlg = new msgDialog(mw_one);
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
  QString str = ui->lineEdit->text().trimmed();
  for (int i = 0; i < ui->listWidget->count(); i++) {
    QListWidgetItem* item = ui->listWidget->item(i);
    QWidget* w = ui->listWidget->itemWidget(item);
    // (QHBoxLayout(0x915ca030), QLabel(0x90885060), QToolButton(0x8fdf4200))
    QLabel* lbl = (QLabel*)w->children().at(1);
    if (lbl->text() == str) {
      ui->listWidget->setCurrentRow(i);
      return;
    }
  }
  add_Item(str,
           QDate::currentDate().toString("ddd MM dd yyyy") + "  " +
               QTime::currentTime().toString(),
           true);
  ui->lineEdit->setText("");
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
    btn->setIcon(QIcon(":/res/done1.png"));

    mw_one->Sleep(400);

    ui->listWidget->setCurrentItem(pItem);
    int row = ui->listWidget->currentRow();
    QString str = getMainLabel(row)->text().trimmed();
    QListWidgetItem* item = new QListWidgetItem;
    item->setSizeHint(QSize(ui->listWidget->width() - 16, 80));
    item->setText(QDateTime::currentDateTime().toString() + "\n" + str);
    ui->listRecycle->insertItem(0, item);
    ui->listWidget->takeItem(row);
    int index = ui->listWidget->currentRow();
    add_ItemSn(index);
  });

  QFont font;
  font.setPointSize(fontSize);
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
  edit->setPlainText(str);
  edit->setHidden(true);
  connect(edit, &QTextEdit::textChanged,
          [=]() { label->setText(edit->toPlainText().trimmed()); });

  QFrame* frame = new QFrame(this);
  QHBoxLayout* hbox = new QHBoxLayout();
  hbox->addWidget(label);
  hbox->addWidget(edit);
  QVBoxLayout* vbox = new QVBoxLayout();
  QLabel* lblTime = new QLabel(this);
  lblTime->setFixedHeight(17);
  QFont f;
  f.setPointSize(13);
  lblTime->setFont(f);
  lblTime->setText(time);
  if (time.contains(tr("Alarm"))) {
    lblTime->setStyleSheet("QLabel{background:blue;color:white}");
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
    edit->setPlainText(lbl->text());
    lbl->setHidden(true);
    edit->setHidden(false);
    lblModi = lbl;
    editModi = edit;
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
  lbl->setText(tr("Alarm") + "  " + ui->dateTimeEdit->text());
  lbl->setStyleSheet("QLabel{background:blue;color:white}");
  // lbl->setStyleSheet(mw_one->mydlgSetTime->ui->lblTitle->styleSheet());
  QFont f = lbl->font();
  f.setBold(true);
  lbl->setFont(f);
  ui->frameSetTime->hide();
  refreshAlarm();
}

qlonglong dlgTodo::getSecond(QString strDateTime) {
  // 2022-8-22 18:18
  int y = QDateTime::currentDateTime().date().year();
  int m = QDateTime::currentDateTime().date().month();
  int d = QDateTime::currentDateTime().date().day();
  int h = QDateTime::currentDateTime().time().hour();
  int mm = QDateTime::currentDateTime().time().minute();
  int s = QDateTime::currentDateTime().time().second();

  QStringList list = strDateTime.split(" ");
  QString strDate = list.at(0);
  QString strTime = list.at(1);

  QStringList listD = strDate.split("-");
  int y1 = listD.at(0).toInt();
  int m1 = listD.at(1).toInt();
  int d1 = listD.at(2).toInt();

  QStringList listT = strTime.split(":");
  int h1 = listT.at(0).toInt();
  int mm1 = listT.at(1).toInt();

  qlonglong totalS = (y1 - y) * 365 * 24 * 60 * 60 +
                     (m1 - m) * 30 * 24 * 60 * 60 + (d1 - d) * 24 * 60 * 60 +
                     (h1 - h) * 60 * 60 + (mm1 - mm) * 60 - s;

  return totalS;
}

void dlgTodo::on_btnSetTime_clicked() {
  if (!ui->listWidget->currentIndex().isValid()) return;

  QLabel* lbl = getTimeLabel(ui->listWidget->currentRow());
  if (lbl->text().trimmed().contains(tr("Alarm"))) {
    QString str = lbl->text().trimmed();
    str = str.replace(tr("Alarm"), "").trimmed();
    QStringList list = str.split(" ");
    QDate date;
    QTime time;
    date = QDate::fromString(list.at(0), "yyyy-M-d");
    time = QTime::fromString(list.at(1), "HH:mm");
    ui->dateTimeEdit->setDate(date);
    ui->dateTimeEdit->setTime(time);

  } else {
    ui->dateTimeEdit->setDate(QDate::currentDate());
    ui->dateTimeEdit->setTime(QTime::currentTime());
  }
  if (ui->frameSetTime->isHidden())
    ui->frameSetTime->show();
  else
    ui->frameSetTime->hide();
}

void dlgTodo::on_btnCancel_clicked() {
  QLabel* lbl = getTimeLabel(ui->listWidget->currentRow());
  QString str = lbl->text().trimmed();
  if (str.contains(tr("Alarm"))) str = str.replace(tr("Alarm"), "");
  lbl->setText(str.trimmed());
  lbl->setStyleSheet(getMainLabel(ui->listWidget->currentRow())->styleSheet());
  ui->frameSetTime->hide();
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
  ui->lineEdit->hide();
  ui->frameSetTime->hide();
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
  ui->lineEdit->show();
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

  for (int i = 0; i < ui->listWidget->count(); i++) {
    lbl = getTimeLabel(i);
    str = lbl->text().trimmed();
    if (str.contains(tr("Alarm"))) {
      count++;
      str = str.replace(tr("Alarm"), "").trimmed();
      qlonglong totals = getSecond(str);
      if (totals > 0) {
        QLabel* lblMain = getMainLabel(ui->listWidget->currentRow());
        QString str1 = str + "|" + lblMain->text() + "|" +
                       QString::number(totals) + "|" + tr("Close");

        startTimerAlarm(str1);
        Reg.setValue("msg" + QString::number(count), str1);
      } else {
        lbl->setText(str);
        lbl->setStyleSheet(getMainLabel(i)->styleSheet());
      }
    }
  }

  if (count == 0) {
    mw_one->ui->btnTodo->setIcon(QIcon(":/res/todo.png"));
  } else {
    mw_one->ui->btnTodo->setIcon(QIcon(":/res/todo1.png"));
  }

  Reg.setValue("count", count);
  if (!QFileInfo(ini_file).exists())
    qDebug() << "ini no exists";
  else
    qDebug() << "ini ok";
}
