#include "msgdialog.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_msgdialog.h"

extern MainWindow* mw_one;
QString hsStyle;

msgDialog::msgDialog(QWidget* parent) : QDialog(parent), ui(new Ui::msgDialog) {
  ui->setupUi(this);

  this->layout()->setMargin(0);
  this->installEventFilter(this);
  this->layout()->setMargin(5);
  initDlg();
  ui->frameDate->hide();
  ui->frameTime->hide();
  ui->dateTimeEdit->hide();
  ui->lblTodoText->setWordWrap(true);
  ui->lblTodoText->adjustSize();
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
  ui->hsHour->setStyleSheet(hsStyle);
  ui->hsMinute->setStyleSheet(hsStyle);
  ui->hsYear->setStyleSheet(hsStyle);
  ui->hsMonth->setStyleSheet(hsStyle);
  ui->hsDay->setStyleSheet(hsStyle);

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
  on_btnHour_clicked();
  ui->btnSetDT->setFocus();
}

void msgDialog::setBtnTitle() {
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

void msgDialog::on_hsHour_valueChanged(int value) {
  QString strH;

  QString strTime = QString("%1").arg(ui->hsHour->value(), 2, 10, QChar('0')) +
                    ":" +
                    QString("%1").arg(ui->hsMinute->value(), 2, 10, QChar('0'));
  ui->dateTimeEdit->setTime(QTime::fromString(strTime, "HH:mm"));
}

void msgDialog::on_btnHour_clicked() { addBtn(0, 24, 5, tr("Hour")); }

void msgDialog::on_btnMinute_clicked() { addBtn(0, 60, 6, tr("Minute")); }

void msgDialog::on_btnDay_clicked() { addBtn(1, 31, 5, tr("Day")); }

void msgDialog::on_btnMonth_clicked() { addBtn(1, 12, 3, tr("Month")); }

void msgDialog::on_btnYear_clicked() {
  addBtn(QDate::currentDate().year(), 15, 3, tr("Year"));
}

void msgDialog::addBtn(int start, int total, int col, QString flag) {
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
    btn->setText(QString::number(count + i + start));
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
      if (w->text() == d) {
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

  QString strDT;
  if (flag == tr("Year")) {
    y = title;
  }
  if (flag == tr("Month")) {
    m = title;
  }
  if (flag == tr("Day")) {
    d = title;
  }
  if (flag == tr("Hour")) {
    h = title;
  }
  if (flag == tr("Minute")) {
    mm = title;
  }

  strDT = y + "-" + m + "-" + d + " " + h + ":" + mm;
  ui->dateTimeEdit->setDateTime(QDateTime::fromString(strDT, "yyyy-M-d HH:mm"));
  setBtnTitle();
}

void msgDialog::on_btnCancelDT_clicked() {
  mw_one->mydlgTodo->on_btnCancel_clicked();
}

void msgDialog::on_btnSetDT_clicked() { mw_one->mydlgTodo->on_btnOK_clicked(); }
