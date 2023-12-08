#include "DateSelector.h"

#include "mainwindow.h"
#include "ui_DateSelector.h"
#include "ui_mainwindow.h"

extern MainWindow *mw_one;

DateSelector::DateSelector(QWidget *parent)
    : QDialog(parent), ui(new Ui::DateSelector) {
  ui->setupUi(this);
  this->installEventFilter(this);

  rboxYear = new RollingBox(this);
  rboxMonth = new RollingBox(this);
  rboxDay = new RollingBox(this);

  int rw = 260;
  initRBox(rboxYear, rw);
  initRBox(rboxMonth, rw);
  initRBox(rboxDay, rw);

  ui->gboxYear->layout()->addWidget(rboxYear);
  ui->gboxMonth->layout()->addWidget(rboxMonth);
  ui->gboxDay->layout()->addWidget(rboxDay);

  setModal(true);
}

void DateSelector::initRBox(RollingBox *rbox, int w) {
  rbox->setFixedHeight(50);
  rbox->setFixedWidth(w);
}

DateSelector::~DateSelector() { delete ui; }

bool DateSelector::eventFilter(QObject *watch, QEvent *evn) {
  if (evn->type() == QEvent::KeyRelease) {
    QKeyEvent *keyEvent = static_cast<QKeyEvent *>(evn);
    if (keyEvent->key() == Qt::Key_Back) {
      close();
      return true;
    }
  }

  return QWidget::eventFilter(watch, evn);
}

void DateSelector::init() {
  int cy = QDate::currentDate().year();
  rboxYear->setRange(2022, cy);

  if (dateFlag == 1) {
    ui->gboxMonth->hide();
    ui->gboxDay->hide();
    ui->gboxYear->setHidden(false);

    ui->lblYear->setHidden(false);
    ui->lblMonth->hide();
    ui->lblDay->hide();
    ui->lblFlag->hide();

    setFixedHeight(200);
  }

  if (dateFlag == 2) {
    ui->gboxYear->hide();
    ui->gboxDay->hide();
    ui->gboxMonth->setHidden(false);

    ui->lblMonth->setHidden(false);
    ui->lblYear->hide();
    ui->lblDay->hide();
    ui->lblFlag->hide();

    rboxMonth->setRange(1, 13);
    setFixedHeight(200);
  }

  if (dateFlag == 3 || dateFlag == 4) {
    ui->lblYear->setHidden(false);
    ui->lblMonth->setHidden(false);
    ui->lblDay->setHidden(false);
    if (dateFlag == 3) ui->lblFlag->setText(tr("Start Date"));
    if (dateFlag == 4) ui->lblFlag->setText(tr("End Date"));
    ui->lblFlag->setHidden(false);

    rboxMonth->setRange(1, 12);
    rboxDay->setRange(1, 31);
    setFixedHeight(490);
  }

  setFixedWidth(mw_one->width() - 10);
  int x, y, w, h;
  x = mw_one->geometry().x() + 5;
  y = mw_one->geometry().y() + (mw_one->height() - height()) / 2;
  w = width();
  h = height();
  setGeometry(x, y, w, h);
  show();
}

void DateSelector::on_hsYear_valueChanged(int value) {
  ui->lblYear->setText(QString::number(value) + "  " + tr("Year"));
}

void DateSelector::on_hsMonth_valueChanged(int value)

{
  if (value == 13)
    ui->lblMonth->setText(QString::number(rboxMonth->readValue()) + "  " +
                          tr("Year-Round"));
  else {
    ui->lblMonth->setText(QString::number(value) + "  " + tr("Month"));
  }
}

void DateSelector::on_hsDay_valueChanged(int value) {
  ui->lblDay->setText(QString::number(value) + "  " + tr("Day"));
}

void DateSelector::on_btnOk_clicked() {
  QString y, m, d;
  y = QString::number(rboxYear->readValue());
  m = QString::number(rboxMonth->readValue());
  d = QString::number(rboxDay->readValue());
  if (m.length() == 1) m = "0" + m;
  if (d.length() == 1) d = "0" + d;

  if (dateFlag == 1) mw_one->ui->btnYear->setText(y);

  if (dateFlag == 2) {
    int value = rboxMonth->readValue();
    if (value == 13)
      mw_one->ui->btnMonth->setText(tr("Year-Round"));
    else {
      mw_one->ui->btnMonth->setText(m);
    }
  }

  if (dateFlag == 1 || dateFlag == 2) mw_one->mydlgReport->startReport1();

  if (dateFlag == 3) {
    mw_one->ui->btnStartDate->setText(y + "  " + m + "  " + d);
  }

  if (dateFlag == 4) {
    mw_one->ui->btnEndDate->setText(y + "  " + m + "  " + d);
  }

  if (dateFlag == 3 || dateFlag == 4) mw_one->mydlgReport->startReport2();

  mw_one->mydlgReport->saveYMD();
  close();
}

void DateSelector::initStartEndDate(QString flag) {
  QString str;
  if (flag == "start") {
    str = mw_one->ui->btnStartDate->text();
    dateFlag = 3;
  }

  if (flag == "end") {
    str = mw_one->ui->btnEndDate->text();
    dateFlag = 4;
  }

  QStringList list = str.split("  ");
  int y, m, d;
  y = list.at(0).toInt();
  m = list.at(1).toInt();
  d = list.at(2).toInt();
  rboxYear->setValue(y);
  rboxMonth->setValue(m);
  rboxDay->setValue(d);

  ui->gboxYear->setHidden(false);
  ui->gboxMonth->setHidden(false);
  ui->gboxDay->setHidden(false);

  init();
}
