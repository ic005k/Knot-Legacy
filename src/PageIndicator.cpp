#include "PageIndicator.h"

#include "src/mainwindow.h"
#include "ui_PageIndicator.h"
#include "ui_mainwindow.h"

extern MainWindow* mw_one;

dlgFloatFun::dlgFloatFun(QWidget* parent)
    : QDialog(parent), ui(new Ui::dlgFloatFun) {
  ui->setupUi(this);
  ui->lblPageNumber->setStyleSheet("color:#ff6600;");
  this->setContentsMargins(1, 1, 1, 1);
  setWindowFlags(Qt::WindowStaysOnTopHint);

#ifdef Q_OS_ANDROID
#else
  setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint |
                 Qt::WindowDoesNotAcceptFocus);
#endif

  setPicRight();
}

void dlgFloatFun::setPicLeft() {
  QPixmap* pixmap = new QPixmap(":/res/sleft.png");
  pixmap->scaled(ui->lblPic->size(), Qt::KeepAspectRatio,
                 Qt::SmoothTransformation);
  ui->lblPic->setScaledContents(false);
  ui->lblPic->setPixmap(*pixmap);

  showPageNumber("left");
}

void dlgFloatFun::setPicRight() {
  QPixmap* pixmap = new QPixmap(":/res/sright.png");
  pixmap->scaled(ui->lblPic->size(), Qt::KeepAspectRatio,
                 Qt::SmoothTransformation);
  ui->lblPic->setScaledContents(false);
  ui->lblPic->setPixmap(*pixmap);

  showPageNumber("right");
}

void dlgFloatFun::init() {
  this->setGeometry(
      mw_one->geometry().x() + (mw_one->width() - this->width()) / 2,
      mw_one->geometry().y() + (mw_one->height() - this->height()) / 4,
      this->width(), this->height());

  this->show();
}

void dlgFloatFun::showPageNumber(QString page) {
  int sn = 0;
  int cn = mw_one->ui->btnPages->text().split("\n").at(1).toInt();
  int tn = mw_one->ui->btnPages->text().split("\n").at(2).toInt();
  if (page == "left") {
    if (cn + 1 < tn)
      sn = cn + 1;
    else
      sn = tn;
  }

  if (page == "right") {
    if (cn - 1 > 0)
      sn = cn - 1;
    else
      sn = 1;
  }

  ui->lblPageNumber->setText(QString::number(sn));
}

void dlgFloatFun::setY(int y) {
  this->setGeometry(
      mw_one->geometry().x() + mw_one->width() - this->width() - 10, y,
      this->width(), this->height());
}

bool dlgFloatFun::eventFilter(QObject* watch, QEvent* evn) {
  if (evn->type() == QEvent::KeyPress) {
    QKeyEvent* keyEvent = static_cast<QKeyEvent*>(evn);
    if (keyEvent->key() == Qt::Key_Back) {
      return true;
    }
  }

  return QWidget::eventFilter(watch, evn);
}

void dlgFloatFun::closeEvent(QCloseEvent* event) { Q_UNUSED(event); }

void dlgFloatFun::paintEvent(QPaintEvent* event) {
  Q_UNUSED(event);
  return;

  QPainter painter(this);
  painter.fillRect(this->rect(), QColor(0, 0, 0, 0));
  QWidget::paintEvent(event);
}

dlgFloatFun::~dlgFloatFun() { delete ui; }
