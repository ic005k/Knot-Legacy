#include "dlgfloatfun.h"

#include "src/mainwindow.h"
#include "ui_dlgfloatfun.h"
#include "ui_mainwindow.h"

extern MainWindow* mw_one;

dlgFloatFun::dlgFloatFun(QWidget* parent)
    : QDialog(parent), ui(new Ui::dlgFloatFun) {
  ui->setupUi(this);
  this->setContentsMargins(1, 1, 1, 1);
  setWindowFlags(Qt::WindowStaysOnTopHint);

#ifdef Q_OS_MAC
  setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint |
                 Qt::WindowDoesNotAcceptFocus);
#endif

  setPicRight();
}

void dlgFloatFun::setPicLeft() {
  QPixmap* pixmap = new QPixmap(":/res/sleft.png");
  pixmap->scaled(ui->lblPic->size(), Qt::KeepAspectRatio,
                 Qt::SmoothTransformation);
  ui->lblPic->setScaledContents(true);
  ui->lblPic->setPixmap(*pixmap);
}

void dlgFloatFun::setPicRight() {
  QPixmap* pixmap = new QPixmap(":/res/sright.png");
  pixmap->scaled(ui->lblPic->size(), Qt::KeepAspectRatio,
                 Qt::SmoothTransformation);
  ui->lblPic->setScaledContents(true);
  ui->lblPic->setPixmap(*pixmap);
}

void dlgFloatFun::init() {
  this->setGeometry(
      mw_one->geometry().x() + (mw_one->width() - this->width()) / 2,
      mw_one->geometry().y() + (mw_one->height() - this->height()) / 2,
      this->width(), this->height());

  this->show();
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
