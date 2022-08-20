#include "msgdialog.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_msgdialog.h"

extern MainWindow* mw_one;
QString hsStyle;

msgDialog::msgDialog(QWidget* parent) : QDialog(parent), ui(new Ui::msgDialog) {
  ui->setupUi(this);

  this->layout()->setMargin(1);
  this->layout()->setSpacing(1);
  this->installEventFilter(this);
  initDlg();

  ui->dateTimeEdit->hide();
  // ui->lblTodoText->setWordWrap(true);
  // ui->lblTodoText->adjustSize();
  btnNorStyle = ui->btnCancelDT->styleSheet();
  ui->dateTimeEdit->setReadOnly(true);
  ui->lblTodoText->setStyleSheet(
      mw_one->mydlgSetTime->ui->lblTitle->styleSheet());
  QFont font = this->font();
  font.setBold(true);
  ui->btnYear->setFont(font);
  ui->btnMonth->setFont(font);
  ui->btnDay->setFont(font);
  ui->btnHour->setFont(font);
  ui->btnMinute->setFont(font);

  // this->show();
}

msgDialog::~msgDialog() { delete ui; }

void msgDialog::initDlg() {
  QString str = ui->dateTimeEdit->text();
  QStringList list = str.split(" ");
  QString strDate = list.at(0);
  QString strTime = list.at(1);
  QStringList list1, list2;
  list1 = strDate.split("-");
  y = list1.at(0);
  m = list1.at(1);
  d = list1.at(2);
  list2 = strTime.split(":");
  h = list2.at(0);
  mm = list2.at(1);

  setBtnTitle();

  this->setGeometry(mw_one->geometry().x(), mw_one->geometry().y(),
                    mw_one->width(), mw_one->height());
  this->setModal(true);
  on_btnYear_clicked();
  ui->btnSetDT->setFocus();
}

void msgDialog::setBtnTitle() {
  QString strDT;
  strDT = y + "-" + m + "-" + d + " " + h + ":" + mm;
  ui->dateTimeEdit->setDateTime(QDateTime::fromString(strDT, "yyyy-M-d HH:mm"));

  ui->btnYear->setText(y + "\n" + tr("Year"));
  ui->btnMonth->setText(m + "\n" + tr("Month"));
  ui->btnDay->setText(d + "\n" + tr("Day"));
  ui->btnHour->setText(h + "\n" + tr("Hour"));
  ui->btnMinute->setText(mm + "\n" + tr("Minute"));
}

bool msgDialog::eventFilter(QObject* obj, QEvent* evn) {
  if (evn->type() == QEvent::KeyPress) {
    QKeyEvent* keyEvent = static_cast<QKeyEvent*>(evn);
    if (keyEvent->key() == Qt::Key_Back) {
      on_btnOK_clicked();
      return true;
    }
  }

  return QWidget::eventFilter(obj, evn);
}

void msgDialog::on_btnOK_clicked() { close(); }

void msgDialog::on_btnYear_clicked() {
  addBtn(QDate::currentDate().year(), 15, 3, tr("Year"), false);
}

void msgDialog::on_btnMonth_clicked() { addBtn(1, 12, 3, tr("Month"), false); }

void msgDialog::on_btnDay_clicked() {
  int maxDay = 0;
  QString sy = ui->btnYear->text().split("\n").at(0);
  QString sm = ui->btnMonth->text().split("\n").at(0);
  for (int i = 0; i < 50; i++) {
    QString strDate = sy + "-" + sm + "-" + QString::number(i + 1);
    QDate date = QDate::fromString(strDate, "yyyy-M-d");
    if (date.dayOfWeek() == 0) {
      maxDay = i;
      break;
    }
  }

  addBtn(1, maxDay, 6, tr("Day"), true);
}

void msgDialog::on_btnHour_clicked() {
  // addBtn(0, 24, 5, tr("Hour"));
  addDial(0, 23, tr("Hour"));
}

void msgDialog::on_btnMinute_clicked() {
  // addBtn(0, 60, 6, tr("Minute"));
  addDial(0, 59, tr("Minute"));
}

void msgDialog::addBtn(int start, int total, int col, QString flag, bool week) {
  QObjectList lstOfChildren0 =
      mw_one->getAllToolButton(mw_one->getAllUIControls(ui->frameDT));
  for (int i = 0; i < lstOfChildren0.count(); i++) {
    QToolButton* w = (QToolButton*)lstOfChildren0.at(i);
    w->setStyleSheet(btnNorStyle);
    QStringList list = w->text().split("\n");
    if (list.at(1) == flag) w->setStyleSheet(btnSelStyle);
  }

  qDeleteAll(ui->frameSel->findChildren<QObject*>());

  int row = 0;
  int count = 0;

  QGridLayout* gl = new QGridLayout(this);
  gl->setMargin(5);
  gl->setSpacing(5);
  ui->frameSel->setLayout(gl);

  for (int i = 0; i < total; i++) {
    for (int j = 0; j < col; j++) {
      QToolButton* btn = new QToolButton(ui->frameSel);
      btn->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
      btn->setObjectName("btn" + QString::number(count));
      QString str = QString::number(count + start);
      if (flag == tr("Hour") || flag == tr("Minute")) {
        if (str.length() == 1) str = "0" + str;
      }

      if (week) {
        QString strWeek;
        QString sy = ui->btnYear->text().split("\n").at(0);
        QString sm = ui->btnMonth->text().split("\n").at(0);
        QString strDate = sy + "-" + sm + "-" + str;
        QDate date = QDate::fromString(strDate, "yyyy-M-d");
        int we = date.dayOfWeek();
        if (we == 1) strWeek = tr("Mon");
        if (we == 2) strWeek = tr("Tue");
        if (we == 3) strWeek = tr("Wed");
        if (we == 4) strWeek = tr("Thur");
        if (we == 5) strWeek = tr("Fri");
        if (we == 6) strWeek = tr("Sat");
        if (we == 7) strWeek = tr("Sun");

        str = str + "\n" + strWeek;
      }
      btn->setText(str);

      connect(btn, &QToolButton::clicked, [=]() { onBtnClick(btn, flag); });

      gl->addWidget(btn, row, j, 1, 1);
      count++;
      total--;
    }
    row++;
  }

  for (int i = 0; i < total; i++) {
    QToolButton* btn = new QToolButton(ui->frameSel);
    btn->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    btn->setObjectName("btn" + QString::number(count + i));
    QString str = QString::number(count + i + start);
    if (week) {
      QString strWeek;
      QString sy = ui->btnYear->text().split("\n").at(0);
      QString sm = ui->btnMonth->text().split("\n").at(0);
      QString strDate = sy + "-" + sm + "-" + str;
      QDate date = QDate::fromString(strDate, "yyyy-M-d");
      int we = date.dayOfWeek();
      if (we == 1) strWeek = tr("Mon");
      if (we == 2) strWeek = tr("Tue");
      if (we == 3) strWeek = tr("Wed");
      if (we == 4) strWeek = tr("Thur");
      if (we == 5) strWeek = tr("Fri");
      if (we == 6) strWeek = tr("Sat");
      if (we == 7) strWeek = tr("Sun");

      str = str + "\n" + strWeek;
    }
    btn->setText(str);
    connect(btn, &QToolButton::clicked, [=]() { onBtnClick(btn, flag); });
    gl->addWidget(btn, row + 1, i, 1, 1);
  }

  QObjectList lstOfChildren =
      mw_one->getAllToolButton(mw_one->getAllUIControls(ui->frameSel));
  for (int i = 0; i < lstOfChildren.count(); i++) {
    QToolButton* w = (QToolButton*)lstOfChildren.at(i);
    if (flag == tr("Year")) {
      if (w->text() == y) {
        w->setStyleSheet(btnSelStyle);
      }
    }
    if (flag == tr("Month")) {
      if (w->text() == m) {
        w->setStyleSheet(btnSelStyle);
      }
    }
    if (flag == tr("Day")) {
      if (w->text().split("\n").at(0) == d) {
        w->setStyleSheet(btnSelStyle);
      }
    }
    if (flag == tr("Hour")) {
      if (w->text() == h) {
        w->setStyleSheet(btnSelStyle);
      }
    }
    if (flag == tr("Minute")) {
      if (w->text() == mm) {
        w->setStyleSheet(btnSelStyle);
      }
    }
  }
}

void msgDialog::onBtnClick(QToolButton* btn, QString flag) {
  QObjectList lstOfChildren =
      mw_one->getAllToolButton(mw_one->getAllUIControls(ui->frameSel));
  for (int i = 0; i < lstOfChildren.count(); i++) {
    QToolButton* w = (QToolButton*)lstOfChildren.at(i);
    w->setStyleSheet(btnNorStyle);
  }
  btn->setStyleSheet(btnSelStyle);

  QString title = btn->text();

  if (flag == tr("Year")) {
    y = title;
  }
  if (flag == tr("Month")) {
    m = title;
  }
  if (flag == tr("Day")) {
    d = title.split("\n").at(0);
  }
  if (flag == tr("Hour")) {
    h = title;
  }
  if (flag == tr("Minute")) {
    mm = title;
  }

  setBtnTitle();
}

void msgDialog::on_btnCancelDT_clicked() {
  mw_one->mydlgTodo->on_btnCancel_clicked();
}

void msgDialog::on_btnSetDT_clicked() { mw_one->mydlgTodo->on_btnOK_clicked(); }

void msgDialog::addDial(int min, int max, QString flag) {
  QObjectList lstOfChildren0 =
      mw_one->getAllToolButton(mw_one->getAllUIControls(ui->frameDT));
  for (int i = 0; i < lstOfChildren0.count(); i++) {
    QToolButton* w = (QToolButton*)lstOfChildren0.at(i);
    w->setStyleSheet(btnNorStyle);
    QStringList list = w->text().split("\n");
    if (list.at(1) == flag) w->setStyleSheet(btnSelStyle);
  }

  qDeleteAll(ui->frameSel->findChildren<QObject*>());

  QGridLayout* gl = new QGridLayout(this);
  gl->setMargin(5);
  gl->setSpacing(5);
  ui->frameSel->setLayout(gl);

  QDial* btn = new QDial(ui->frameSel);
  btn->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
  btn->setObjectName("btn");
  btn->setMinimum(min);
  btn->setMaximum(max);
  btn->setNotchTarget(1.00);
  btn->setWrapping(false);
  btn->setNotchesVisible(true);
  btn->setPageStep(5);
  if (flag == tr("Hour")) btn->setPageStep(2);

  connect(btn, &QDial::valueChanged, [=]() { onDial(btn, flag); });

  gl->addWidget(btn, 0, 0, 1, 1);

  if (flag == tr("Minute")) {
    btn->setSliderPosition(mm.toInt());
  }
  if (flag == tr("Hour")) {
    btn->setSliderPosition(h.toInt());
  }
}

void msgDialog::onDial(QDial* btn, QString flag) {
  if (flag == tr("Hour")) {
    h = QString::number(btn->sliderPosition());
    if (h.length() == 1) h = "0" + h;
  }

  if (flag == tr("Minute")) {
    mm = QString::number(btn->sliderPosition());
    if (mm.length() == 1) mm = "0" + mm;
  }
  setBtnTitle();
}

void msgDialog::on_btnToday_clicked() {
  int day = QDate::currentDate().day();
  y = QString::number(QDate::currentDate().year());
  m = QString::number(QDate::currentDate().month());
  d = QString::number(day);

  setBtnTitle();
  on_btnDay_clicked();
}

void msgDialog::on_btnTomorrow_clicked() {
  QDateTime time = QDateTime::currentDateTime();
  QString str = time.addDays(+1).toString("yyyy-M-d");
  y = str.split("-").at(0);
  m = str.split("-").at(1);
  d = str.split("-").at(2);

  setBtnTitle();
  on_btnDay_clicked();
}

void msgDialog::on_btnNextWeek_clicked() {
  int week = QDate::currentDate().dayOfWeek();
  int x = 8 - week;

  QDateTime time = QDateTime::currentDateTime();
  QString str = time.addDays(+x).toString("yyyy-M-d");

  y = str.split("-").at(0);
  m = str.split("-").at(1);
  d = str.split("-").at(2);

  setBtnTitle();
  on_btnDay_clicked();
}
