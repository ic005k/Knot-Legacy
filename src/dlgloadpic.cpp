#include "dlgloadpic.h"

#include "src/mainwindow.h"
#include "ui_dlgloadpic.h"
extern MainWindow* mw_one;
extern QString picfile;

dlgLoadPic::dlgLoadPic(QWidget* parent)
    : QDialog(parent), ui(new Ui::dlgLoadPic) {
  ui->setupUi(this);
  this->installEventFilter(this);
  ui->lblPic->installEventFilter(this);
  ui->btnReduce->hide();
  ui->btnZoom->hide();
  ui->hsZoom->setMaximum(500);

  this->layout()->setMargin(1);
  ui->scrollArea->setContentsMargins(1, 1, 1, 1);

  initMain();

  QPixmap pixmap(":/res/icon.png");
  int sx, sy;
  sx = ui->scrollArea->width();
  sy = ui->scrollArea->height();
  pixmap = pixmap.scaled(sx, sy, Qt::KeepAspectRatio, Qt::SmoothTransformation);
  ui->lblPic->setPixmap(pixmap);

  setModal(true);
  show();
  close();
}

dlgLoadPic::~dlgLoadPic() { delete ui; }

void dlgLoadPic::initMain() {
  this->setGeometry(mw_one->geometry().x(), mw_one->geometry().y(),
                    mw_one->width(), mw_one->height());
}

bool dlgLoadPic::eventFilter(QObject* watch, QEvent* evn) {
  if (evn->type() == QEvent::KeyPress) {
    QKeyEvent* keyEvent = static_cast<QKeyEvent*>(evn);
    if (keyEvent->key() == Qt::Key_Back) {
      on_btnBack_clicked();
      return true;
    }
  }

  QMouseEvent* event = static_cast<QMouseEvent*>(evn);
  if (watch == ui->lblPic) {
    static int press_x;
    static int press_y;
    static int relea_x;
    static int relea_y;

    if (event->type() == QEvent::MouseButtonPress) {
      press_x = event->globalX();
      press_y = event->globalY();

      // qDebug() << "Press:" << press_x << press_y;
    }

    if (event->type() == QEvent::MouseButtonRelease) {
      relea_x = event->globalX();
      relea_y = event->globalY();
      // qDebug() << "Release:" << relea_x << relea_y;
    }

    if (event->type() == QEvent::MouseMove) {
      relea_x = event->globalX();
      relea_y = event->globalY();
      // qDebug() << "Release:" << relea_x << relea_y;
    }

    int mx = qAbs(relea_x - press_x) / 10;
    //判断滑动方向（右滑）
    if ((relea_x - press_x) > 0 && event->type() == QEvent::MouseMove) {
      ui->scrollArea->horizontalScrollBar()->setSliderPosition(
          ui->scrollArea->horizontalScrollBar()->sliderPosition() - mx);
    }

    //判断滑动方向（左滑）
    if ((relea_x - press_x) < 0 && event->type() == QEvent::MouseMove) {
      ui->scrollArea->horizontalScrollBar()->setSliderPosition(
          ui->scrollArea->horizontalScrollBar()->sliderPosition() + mx);
    }

    int my = qAbs(relea_y - press_y) / 10;
    //判断滑动方向（上滑）
    if (event->type() == QEvent::MouseMove && (relea_y - press_y) < 0) {
      ui->scrollArea->verticalScrollBar()->setSliderPosition(
          ui->scrollArea->verticalScrollBar()->sliderPosition() + my);
    }

    //判断滑动方向（下滑）
    if (event->type() == QEvent::MouseMove && (relea_y - press_y) > 0) {
      ui->scrollArea->verticalScrollBar()->setSliderPosition(
          ui->scrollArea->verticalScrollBar()->sliderPosition() - my);
    }
  }

  return QWidget::eventFilter(watch, evn);
}

void dlgLoadPic::on_btnBack_clicked() {
  close();
  k = 0;
  ui->hsZoom->setValue(0);
  mw_one->mydlgReader->on_btnPageUp_clicked();
  mw_one->mydlgReader->on_btnPageNext_clicked();
}

void dlgLoadPic::on_btnZoom_clicked() {
  k = k + 10;
  loadPic(picfile, k);
}

void dlgLoadPic::on_btnReduce_clicked() {
  k = k - 10;
  loadPic(picfile, k);
}

void dlgLoadPic::loadPic(QString picfile, int k) {
  QPixmap pixmap(picfile);
  int sx, sy;
  sx = ui->scrollArea->width() + k -
       ui->scrollArea->verticalScrollBar()->width() - 12;
  sy = ui->scrollArea->height() + k;
  pixmap = pixmap.scaled(sx, sy, Qt::KeepAspectRatio, Qt::SmoothTransformation);
  ui->lblPic->setPixmap(pixmap);
}

void dlgLoadPic::on_hsZoom_valueChanged(int value) { loadPic(picfile, value); }
