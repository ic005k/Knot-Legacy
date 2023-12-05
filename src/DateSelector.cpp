#include "DateSelector.h"

#include "mainwindow.h"
#include "ui_DateSelector.h"
#include "ui_mainwindow.h"

extern MainWindow *mw_one;

DateSelector::DateSelector(QWidget *parent)
    : QDialog(parent), ui(new Ui::DateSelector) {
  ui->setupUi(this);
  this->installEventFilter(this);

  ui->hsMonth->setStyleSheet(ui->hsYear->styleSheet());
  ui->hsDay->setStyleSheet(ui->hsYear->styleSheet());

  int cy = QDate::currentDate().year();
  ui->hsYear->setMaximum(cy + 2);

  setModal(true);
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
  setFixedWidth(mw_one->width() - 40);
  int x, y, w, h;
  x = mw_one->geometry().x() + 20;
  y = mw_one->geometry().y() + (mw_one->height() - height()) / 2;
  w = width();
  h = height();
  setGeometry(x, y, w, h);
  show();
}

void DateSelector::on_hsYear_valueChanged(int value) {
  ui->gboxYear->setTitle(tr("Year") + " : " + QString::number(value));
}

void DateSelector::on_hsMonth_valueChanged(int value)

{
  if (value == 13)
    ui->gboxMonth->setTitle(tr("Month") + " : " + tr("Year-Round"));
  else
    ui->gboxMonth->setTitle(tr("Month") + " : " + QString::number(value));
}

void DateSelector::on_hsDay_valueChanged(int value) {
  ui->gboxDay->setTitle(tr("Day") + " : " + QString::number(value));
}

void DateSelector::on_btnOk_clicked() {
  mw_one->ui->btnYear->setText(QString::number(ui->hsYear->value()));
  if (ui->hsMonth->value() == 13)
    mw_one->ui->btnMonth->setText(tr("Year-Round"));
  else
    mw_one->ui->btnMonth->setText(QString::number(ui->hsMonth->value()));
  close();
}
