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
QListWidget *listTodo, *listRecycle;
extern MainWindow* mw_one;
extern QString iniFile, iniDir;
extern bool loading, isBreak, isImport;
extern int fontSize;

dlgTodo::dlgTodo(QWidget* parent) : QDialog(parent), ui(new Ui::dlgTodo) {
  ui->setupUi(this);
  this->installEventFilter(this);
  this->setContentsMargins(1, 5, 1, 5);
  ui->gridLayout_2->setContentsMargins(1, 1, 1, 1);
  ui->frameSetTime->hide();
  ui->frameRecycle->hide();
  ui->listRecycle->hide();
  ui->lblRecycle->hide();
  this->setModal(true);

  QString strTar = "/data/data/com.x/files/msg.mp3";
  QFile::copy(":/res/msg.mp3", strTar);

  listTodo = new QListWidget;
  listTodo = ui->listWidget;
  listRecycle = new QListWidget;
  listRecycle = ui->listRecycle;
  // ui->listRecycle->setStyleSheet("text-decoration: line-through;");
  ui->listRecycle->setWordWrap(true);

  listTodo->setVerticalScrollMode(QListWidget::ScrollPerPixel);
  QScroller::grabGesture(listTodo, QScroller::LeftMouseButtonGesture);
  listTodo->horizontalScrollBar()->setHidden(true);

  ui->listRecycle->setVerticalScrollMode(QListWidget::ScrollPerPixel);
  QScroller::grabGesture(ui->listRecycle, QScroller::LeftMouseButtonGesture);
  ui->listRecycle->horizontalScrollBar()->setHidden(true);
  ui->listRecycle->verticalScrollBar()->setStyleSheet(mw_one->vsbarStyleSmall);

  /*QScrollerProperties sp;
  sp.setScrollMetric(QScrollerProperties::DragStartDistance, 0.00001);
  sp.setScrollMetric(QScrollerProperties::ScrollingCurve, QEasingCurve::Linear);
  QScroller* qs = QScroller::scroller(listTodo);
  QScroller* qs1 = QScroller::scroller(ui->listRecycle);
  qs->setScrollerProperties(sp);
  qs1->setScrollerProperties(sp);*/
  mw_one->setSCrollPro(listTodo);
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
  int count = listTodo->count();
  Reg.setValue("/Todo/Count", count);
  for (int i = 0; i < count; i++) {
    if (isBreak) break;
    QListWidgetItem* item = listTodo->item(i);
    QWidget* w = listTodo->itemWidget(item);
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
  listTodo->clear();
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
    QListWidgetItem* item = listTodo->item(index);
    QWidget* w = listTodo->itemWidget(item);
    QLabel* lbl = (QLabel*)w->children().at(2);
    lbl->setStyleSheet(highLblStyle);
  }

  refreshAlarm();
}

void dlgTodo::on_btnAdd_clicked() {
  QString str = ui->textEdit->toPlainText().trimmed();
  for (int i = 0; i < listTodo->count(); i++) {
    QLabel* lbl = getMainLabel(i);
    if (lbl->text() == str) {
      listTodo->setCurrentRow(i);
      return;
    }
  }
  add_Item(str,
           QDate::currentDate().toString("ddd MM dd yyyy") + "  " +
               QTime::currentTime().toString(),
           true);
  ui->textEdit->setText("");
  listTodo->verticalScrollBar()->setSliderPosition(0);
}

void dlgTodo::add_Item(QString str, QString time, bool insert) {
  if (str == "") return;
  QListWidgetItem* pItem = new QListWidgetItem;
  pItem->setText("");

  if (insert)
    listTodo->insertItem(0, pItem);
  else
    listTodo->addItem(pItem);

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

    listTodo->setCurrentItem(pItem);
    int row = listTodo->currentRow();
    QString str = getMainLabel(row)->text().trimmed();
    QListWidgetItem* item = new QListWidgetItem;
    item->setSizeHint(QSize(listTodo->width() - 16, 80));
    item->setText(QDateTime::currentDateTime().toString() + "  " + tr("Done") +
                  "\n" + str);
    ui->listRecycle->insertItem(0, item);
    listTodo->takeItem(row);
    int index = listTodo->currentRow();
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
      pItem->setSizeHint(QSize(listTodo->width() - 20, itemHeight));
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

  listTodo->setItemWidget(pItem, w);

  edit->clear();
  edit->setPlainText(str);
  int itemHeight = fontHeight * 2 + getEditTextHeight(edit);
  pItem->setSizeHint(QSize(listTodo->width() - 20, itemHeight));
  qDebug() << fontHeight << label->height() << itemHeight;

  add_ItemSn(0);
}

void dlgTodo::add_ItemSn(int index) {
  for (int i = 0; i < listTodo->count(); i++) {
    QListWidgetItem* item = listTodo->item(i);
    QWidget* w = listTodo->itemWidget(item);
    QLabel* lbl = (QLabel*)w->children().at(1);

    lbl->setText(QString::number(i + 1) + ". ");
  }
  listTodo->setCurrentRow(index);
}

int dlgTodo::getEditTextHeight(QTextEdit* edit) {
  QTextDocument* doc = edit->document();
  doc->adjustSize();
  int mainHeight = doc->size().rheight() * 1.06;
  return mainHeight;
}

void dlgTodo::on_listWidget_itemClicked(QListWidgetItem* item) {
  QWidget* w = listTodo->itemWidget(item);
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
  // if (editItem != NULL) listTodo->closePersistentEditor(editItem);
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
  int row = listTodo->currentRow();
  if (row < 0) return;

  if (ui->btnModify->text() == tr("Modify")) {
    QListWidgetItem* item = listTodo->currentItem();
    QWidget* w = listTodo->itemWidget(item);
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
  int row = listTodo->currentRow();
  if (row < 0) return;
  QListWidgetItem* item = listTodo->currentItem();
  QWidget* w = listTodo->itemWidget(item);
  QLabel* lbl = (QLabel*)w->children().at(2)->children().at(2);
  listTodo->takeItem(row);
  QString str = lbl->text();
  QLabel* lblTime = (QLabel*)w->children().at(2)->children().at(1);
  add_Item(str, lblTime->text(), true);
  QListWidgetItem* item1 = listTodo->currentItem();
  QWidget* w1 = listTodo->itemWidget(item1);
  QFrame* frame = (QFrame*)w1->children().at(2);
  frame->setStyleSheet(highLblStyle);
  listTodo->scrollToTop();

  refreshAlarm();
  setAlartTop(minAlartItem);
}

void dlgTodo::on_btnLow_clicked() {
  int row = listTodo->currentRow();
  if (row < 0) return;
  QListWidgetItem* item = listTodo->currentItem();
  QWidget* w = listTodo->itemWidget(item);
  QLabel* lbl = (QLabel*)w->children().at(2)->children().at(2);
  listTodo->takeItem(row);
  QString str = lbl->text();
  QLabel* lblTime = (QLabel*)w->children().at(2)->children().at(1);
  add_Item(str, lblTime->text(), false);
  listTodo->setCurrentRow(listTodo->count() - 1);
  listTodo->scrollToBottom();

  refreshAlarm();
  setAlartTop(minAlartItem);
}

QLabel* dlgTodo::getTimeLabel(int index) {
  QListWidgetItem* item = listTodo->item(index);
  QWidget* w = listTodo->itemWidget(item);
  QLabel* lbl = (QLabel*)w->children().at(2)->children().at(1);
  return lbl;
}

QLabel* dlgTodo::getMainLabel(int index) {
  QListWidgetItem* item = listTodo->item(index);
  QWidget* w = listTodo->itemWidget(item);
  QLabel* lbl = (QLabel*)w->children().at(2)->children().at(2);
  return lbl;
}

QLabel* dlgTodo::getTimeLabel(QListWidgetItem* item) {
  QWidget* w = listTodo->itemWidget(item);
  QLabel* lbl = (QLabel*)w->children().at(2)->children().at(1);
  return lbl;
}

QLabel* dlgTodo::getMainLabel(QListWidgetItem* item) {
  QWidget* w = listTodo->itemWidget(item);
  QLabel* lbl = (QLabel*)w->children().at(2)->children().at(2);
  return lbl;
}

void dlgTodo::on_btnOK_clicked() {
  QLabel* lbl = getTimeLabel(listTodo->currentRow());
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

    lbl->setText(tr("Alarm") + "  " + str + "  " +
                 mw_one->mymsgDlg->ui->dateTimeEdit->time().toString("HH:mm"));
  } else {
    lbl->setText(tr("Alarm") + "  " +
                 mw_one->mymsgDlg->ui->dateTimeEdit->text());
  }

  lbl->setStyleSheet(alarmStyle);
  QFont f = lbl->font();
  f.setBold(true);
  lbl->setFont(f);
  ui->frameSetTime->hide();
  mw_one->mymsgDlg->close();
  refreshAlarm();
  setAlartTop(minAlartItem);
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
  if (!listTodo->currentIndex().isValid()) return;

  QLabel* lblMain = getMainLabel(listTodo->currentRow());
  QLabel* lbl = getTimeLabel(listTodo->currentRow());
  QString str = lbl->text().trimmed();
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
  QString txt = tr("Todo") + " : " + lblMain->text();
  txt = txt.replace("\n", " ");
  QFontMetrics fm(this->font());
  QString qsLine = fm.elidedText(txt, Qt::ElideRight, this->width() - 10);
  mw_one->mymsgDlg->ui->lblTodoText->setText(qsLine);
  mw_one->mymsgDlg->show();
}

void dlgTodo::on_btnCancel_clicked() {
  QLabel* lbl = getTimeLabel(listTodo->currentRow());
  QString str = lbl->text().trimmed();
  if (str.contains(tr("Alarm"))) str = str.replace(tr("Alarm"), "");
  lbl->setText(str.trimmed());
  lbl->setStyleSheet(getMainLabel(listTodo->currentRow())->styleSheet());
  ui->frameSetTime->hide();
  mw_one->mymsgDlg->close();
  refreshAlarm();
  setAlartTop(minAlartItem);
}

void dlgTodo::on_Alarm() {
  int count = 0;
  for (int i = 0; i < listTodo->count(); i++) {
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
  listTodo->hide();
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
  listTodo->show();
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
  isToday = false;
  QString str;
  QLabel* lbl;

  QString ini_file;
  ini_file = "/data/data/com.x/files/msg.ini";
  QSettings Reg(ini_file, QSettings::IniFormat);
  Reg.setIniCodec("utf-8");

  QStringList listAlarm;
  QList<qlonglong> listTotalS;
  QList<QListWidgetItem*> listAlarmItems;
  for (int i = 0; i < listTodo->count(); i++) {
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
        listAlarmItems.append(listTodo->item(i));

        // set time marks
        QString strDate = str.split(" ").at(0);
        QString strToday = QDate::currentDate().toString("yyyy-M-d");
        QDateTime ctime = QDateTime::currentDateTime();
        QString strTmo = ctime.addDays(+1).toString("yyyy-M-d");
        if (strDate.contains("-")) {
          if (strDate == strToday) {
            lbl->setStyleSheet(alarmStyleToday);
            isToday = true;
          }

          if (strTmo == strDate) {
            lbl->setStyleSheet(alarmStyleTomorrow);
          }
        } else {
          if (isWeekValid(str, strToday) && !isTomorrow) {
            lbl->setStyleSheet(alarmStyleToday);
            isToday = true;
          }

          if (isWeekValid(str, strTmo) && isTomorrow) {
            lbl->setStyleSheet(alarmStyleTomorrow);
          }
        }

      } else {
        if (str.contains("-")) {
          lbl->setText(str);
          lbl->setStyleSheet(getMainLabel(i)->styleSheet());
        }

        if (!str.contains("-")) {
          lbl->setText(tr("Alarm") + "  " + str);
          lbl->setStyleSheet(alarmStyle);

          QDateTime ctime = QDateTime::currentDateTime();
          QString strTmo = ctime.addDays(+1).toString("yyyy-M-d");
          if (isWeekValid(str, strTmo)) lbl->setStyleSheet(alarmStyleTomorrow);
        }
      }
    }
  }

  qlonglong minValue = 0;
  minAlartItem = NULL;
  if (count > 0) {
    minValue = *std::min_element(listTotalS.begin(), listTotalS.end());
    for (int i = 0; i < listTotalS.count(); i++) {
      if (minValue == listTotalS.at(i)) {
        QString str1 = listAlarm.at(i);
        startTimerAlarm(str1);
        Reg.setValue("msg", str1);

        qDebug() << "Min Time: " << listTotalS << minValue << str1
                 << "curVol: ";
        minAlartItem = listAlarmItems.at(i);

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
  QString strMute = "false";
  if (mw_one->mydlgPre->ui->chkMute->isChecked()) strMute = "true";
  Reg.setValue("mute", strMute);

  if (!QFile(ini_file).exists())
    qDebug() << "ini no exists";
  else
    qDebug() << "ini ok";
}

void dlgTodo::setAlartTop(QListWidgetItem* item) {
  // item is min alarm
  if (item == NULL) return;

  QLabel* lblTime = getTimeLabel(item);
  bool isTop = false;
  if (isToday) {
    for (int m = 0; m < listTodo->count(); m++) {
      if (item == listTodo->item(m)) {
        if (m != 0) {
          isTop = true;
          break;
        }
      }
    }
    if (isTop) {
      add_Item(getMainLabel(item)->text(), lblTime->text(), true);
      getTimeLabel(listTodo->item(0))->setStyleSheet(alarmStyleToday);

      for (int m = 0; m < listTodo->count(); m++) {
        if (item == listTodo->item(m)) {
          if (m != 0) {
            listTodo->takeItem(m);
            break;
          }
        }
      }

      for (int m = 0; m < listTodo->count(); m++) {
        add_ItemSn(m);
      }

      listTodo->setCurrentRow(0);
      listTodo->verticalScrollBar()->setSliderPosition(0);
    }
  }
}

void dlgTodo::on_textEdit_textChanged() {
  int h = getEditTextHeight(ui->textEdit) + 2;
  ui->textEdit->setFixedHeight(h);
}
