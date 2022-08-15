#include "msgdialog.h"

#include "mainwindow.h"
#include "ui_msgdialog.h"

extern MainWindow* mw_one;
QString hsStyle;

msgDialog::msgDialog(QWidget* parent) : QDialog(parent), ui(new Ui::msgDialog) {
  ui->setupUi(this);

  this->layout()->setMargin(0);
  this->installEventFilter(this);
  initDlg();
  ui->frameDate->hide();
  ui->frameTime->hide();
  // gl = new QGridLayout(this);
  // gl->setMargin(10);
  // gl->setSpacing(10);
  // ui->frameSel->setLayout(gl);

  this->show();
}

msgDialog::~msgDialog() { delete ui; }

void msgDialog::initDlg() {
  ui->hsHour->setStyleSheet(hsStyle);
  ui->hsMinute->setStyleSheet(hsStyle);
  ui->hsYear->setStyleSheet(hsStyle);
  ui->hsMonth->setStyleSheet(hsStyle);
  ui->hsDay->setStyleSheet(hsStyle);

  this->setGeometry(mw_one->geometry().x(), mw_one->geometry().y(),
                    mw_one->width(), mw_one->height());
  this->setModal(true);
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

void msgDialog::on_btnHour_clicked() { addBtn(0, 24, 5, "Hour"); }

void msgDialog::on_btnMinute_clicked() { addBtn(0, 60, 6, "Minute"); }

void msgDialog::on_btnDay_clicked() { addBtn(1, 31, 5, "Day"); }

void msgDialog::on_btnMonth_clicked() { addBtn(1, 12, 3, "Month"); }

void msgDialog::on_btnYear_clicked() { addBtn(2022, 15, 5, "Year"); }

void msgDialog::addBtn(int start, int total, int col, QString flag) {
  qDeleteAll(ui->frameSel->findChildren<QObject*>());

  int row = 0;
  int count = 0;

  QGridLayout* gl = new QGridLayout(this);
  gl->setMargin(10);
  gl->setSpacing(10);
  ui->frameSel->setLayout(gl);

  for (int i = 0; i < total; i++) {
    for (int j = 0; j < col; j++) {
      QToolButton* btn = new QToolButton(ui->frameSel);
      btn->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
      btn->setObjectName("btn" + QString::number(count));
      btn->setText(QString::number(count + start));

      connect(btn, &QToolButton::clicked, [=]() { onBtnClick(btn, flag); });

      gl->addWidget(btn, row, j, 1, 1);
      count++;
      total--;
    }
    row++;
  }

  for (int i = 0; i < total; i++) {
    QToolButton* btn = new QToolButton(ui->frameSel);
    btn->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    btn->setObjectName("btn" + QString::number(count + i));
    btn->setText(QString::number(count + i + start));
    connect(btn, &QToolButton::clicked, [=]() { onBtnClick(btn, flag); });
    gl->addWidget(btn, row + 1, i, 1, 1);
  }
}

void msgDialog::onBtnClick(QToolButton* btn, QString flag) {
  QString title = btn->text();

  QString strDT;
  if (flag == "Year") {
    y = title;
  }
  if (flag == "Month") {
    m = title;
  }
  if (flag == "Day") {
    d = title;
  }
  if (flag == "Hour") {
    h = title;
  }
  if (flag == "Minute") {
    mm = title;
  }

  strDT = y + "-" + m + "-" + d + " " + h + ":" + mm;
  qDebug() << strDT;
}
