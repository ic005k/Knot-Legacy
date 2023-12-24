#include "PageIndicator.h"

#include "src/MainWindow.h"
#include "ui_MainWindow.h"
#include "ui_PageIndicator.h"

extern MainWindow* mw_one;

PageIndicator::PageIndicator(QWidget* parent)
    : QDialog(parent), ui(new Ui::PageIndicator) {
  ui->setupUi(this);
  ui->lblPageNumber->setStyleSheet("color:#ff6600;");
  ui->lblPic->setHidden(true);
  ui->lblPic->setFixedSize(100, 100);

  this->setContentsMargins(1, 1, 1, 1);
  this->layout()->setContentsMargins(1, 1, 1, 1);

  setWindowFlags(Qt::WindowStaysOnTopHint);

#ifdef Q_OS_ANDROID
#else
  setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint |
                 Qt::WindowDoesNotAcceptFocus);
#endif

  setPicRight();
}

void PageIndicator::setPicLeft() {
  QPixmap* pixmap = new QPixmap(":/res/sleft.svg");
  pixmap->scaled(ui->lblPic->size(), Qt::KeepAspectRatio,
                 Qt::SmoothTransformation);
  ui->lblPic->setScaledContents(true);
  ui->lblPic->setPixmap(*pixmap);

  showPageNumber("left");
}

void PageIndicator::setPicRight() {
  QPixmap* pixmap = new QPixmap(":/res/sright.svg");
  pixmap->scaled(ui->lblPic->size(), Qt::KeepAspectRatio,
                 Qt::SmoothTransformation);
  ui->lblPic->setScaledContents(true);
  ui->lblPic->setPixmap(*pixmap);

  showPageNumber("right");
}

void PageIndicator::init() {
  int w = mw_one->geometry().width() - 20;
  setFixedWidth(w);
  this->setGeometry(mw_one->geometry().x() + (mw_one->width() - w) / 2,
                    mw_one->geometry().y() + 5, this->width(), this->height());

  this->show();
}

void PageIndicator::showPageNumber(QString page) {
  sn = 0;
  cn = mw_one->ui->btnPages->text().split("\n").at(1).toInt();
  tn = mw_one->ui->btnPages->text().split("\n").at(2).toInt();
  if (page == "left") {
    if (cn + 1 < tn) {
      sn = cn + 1;

    } else {
      sn = tn;
    }
    ui->lblPageNumber->setText(QString::number(sn));
  }

  if (page == "right") {
    if (cn - 1 > 0) {
      sn = cn - 1;

    } else {
      sn = 1;
    }
    ui->lblPageNumber->setText(QString::number(sn));
  }
}

void PageIndicator::setY(int y) {
  this->setGeometry(
      mw_one->geometry().x() + mw_one->width() - this->width() - 10, y,
      this->width(), this->height());
}

bool PageIndicator::eventFilter(QObject* watch, QEvent* evn) {
  if (evn->type() == QEvent::KeyRelease) {
    QKeyEvent* keyEvent = static_cast<QKeyEvent*>(evn);
    if (keyEvent->key() == Qt::Key_Back) {
      return true;
    }
  }

  return QWidget::eventFilter(watch, evn);
}

void PageIndicator::closeEvent(QCloseEvent* event) { Q_UNUSED(event); }

void PageIndicator::paintEvent(QPaintEvent* event) {
  Q_UNUSED(event);
  return;

  QPainter painter(this);
  painter.fillRect(this->rect(), QColor(0, 0, 0, 0));
  QWidget::paintEvent(event);
}

PageIndicator::~PageIndicator() { delete ui; }
