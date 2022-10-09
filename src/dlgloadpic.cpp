#include "dlgloadpic.h"

#include "src/mainwindow.h"
#include "ui_dlgloadpic.h"
#include "ui_mainwindow.h"
extern MainWindow* mw_one;
extern QString picfile;

dlgLoadPic::dlgLoadPic(QWidget* parent)
    : QDialog(parent), ui(new Ui::dlgLoadPic) {
  ui->setupUi(this);

  ui->graphicsView->setObjectName("myframe");
  ui->graphicsView->setStyleSheet(
      "QFrame#myframe{border-image:url(:/res/b.png)}");

  this->installEventFilter(this);
  ui->lblPic->installEventFilter(this);
  ui->graphicsView->viewport()->installEventFilter(this);
  ui->scrollArea->hide();
  ui->hsZoom->hide();
  ui->lblRatio->hide();

  ui->hsZoom->setMaximum(100);

  int a = 500;
  int b = 50;
  ui->btnZoom->setAutoRepeat(true);
  ui->btnZoom->setAutoRepeatDelay(a);
  ui->btnZoom->setAutoRepeatInterval(b);

  ui->btnReduce->setAutoRepeat(true);
  ui->btnReduce->setAutoRepeatDelay(a);
  ui->btnReduce->setAutoRepeatInterval(b);

  this->layout()->setMargin(1);
  this->layout()->setContentsMargins(1, 1, 1, 1);
  this->layout()->setSpacing(3);
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
      isMousePress = true;
      isMouseRelease = false;
      press_x = event->globalX();
      press_y = event->globalY();

      qDebug() << "Press:" << press_x << press_y;

      m_startPos = event->pos();
    }

    if (event->type() == QEvent::MouseButtonRelease) {
      isMousePress = false;
      isMouseRelease = true;
      relea_x = event->globalX();
      relea_y = event->globalY();
    }

    if (event->type() == QEvent::MouseMove) {
      relea_x = event->globalX();
      relea_y = event->globalY();

      if (isMousePress) {
        // QPointF point = (event->pos() - m_startPos) * m_pImage->m_scaleValue;
        // m_pImage->setPos(point);

        int mx = qAbs(relea_x - press_x) / 10;
        //判断滑动方向（右滑）
        if ((relea_x - press_x) > 0) {
          ui->scrollArea->horizontalScrollBar()->setSliderPosition(
              ui->scrollArea->horizontalScrollBar()->sliderPosition() - mx);
        }

        //判断滑动方向（左滑）
        if ((relea_x - press_x) < 0) {
          ui->scrollArea->horizontalScrollBar()->setSliderPosition(
              ui->scrollArea->horizontalScrollBar()->sliderPosition() + mx);
        }

        int my = qAbs(relea_y - press_y) / 10;
        //判断滑动方向（上滑）
        if ((relea_y - press_y) < 0) {
          ui->scrollArea->verticalScrollBar()->setSliderPosition(
              ui->scrollArea->verticalScrollBar()->sliderPosition() + my);
        }

        //判断滑动方向（下滑）
        if ((relea_y - press_y) > 0) {
          ui->scrollArea->verticalScrollBar()->setSliderPosition(
              ui->scrollArea->verticalScrollBar()->sliderPosition() - my);
        }
      }
    }
  }

  return QWidget::eventFilter(watch, evn);
}

void dlgLoadPic::on_btnBack_clicked() {
  close();
  k = 0;
  ui->hsZoom->setValue(0);
  mw_one->clearSelectBox();
}

void dlgLoadPic::on_btnZoom_clicked() {
  // k = k + 10;
  // loadPic(picfile, k);
  // ui->hsZoom->setValue(k);

  m_pImage->scaleImg(true);
}

void dlgLoadPic::on_btnReduce_clicked() {
  // k = k - 10;
  // loadPic(picfile, k);
  // ui->hsZoom->setValue(k);

  m_pImage->scaleImg(false);
}

void dlgLoadPic::loadPic(QString picfile, int k) {
  qDebug() << "file exists=" << QFile(picfile).exists();
  QImage img(picfile);

  /*QPixmap pixmap;
  int sx, sy;
  sx = ui->scrollArea->width() + k -
       ui->scrollArea->verticalScrollBar()->width() - 12;
  sy = ui->scrollArea->height() + k;
  pixmap = QPixmap::fromImage(img);
  pixmap = pixmap.scaled(sx, sy, Qt::KeepAspectRatio, Qt::SmoothTransformation);
  ui->lblPic->setPixmap(pixmap);

  showRatio(img.width(), pixmap.width());*/

  recvShowPicSignal(img);
}

void dlgLoadPic::showRatio(double w0, double w1) {
  double r0;
  r0 = w1 / w0 * 100;
  QString str = QString::number(r0);
  ui->lblRatio->setText(str.split(".").at(0) + "%");
}

void dlgLoadPic::on_hsZoom_valueChanged(int value) {
  Q_UNUSED(value);
  k = value;
  m_pImage->scaleImg(value);
}

void dlgLoadPic::on_hsZoom_sliderReleased() {
  loadPic(picfile, ui->hsZoom->value());
}

void dlgLoadPic::recvShowPicSignal(QImage image) {
  QPixmap ConvertPixmap = QPixmap::fromImage(image);
  QGraphicsScene* qgraphicsScene = new QGraphicsScene;
  m_pImage = new QImageWidget(&ConvertPixmap);
  int nwith = ui->graphicsView->width() - 2;
  int nheight = ui->graphicsView->height() - 2;
  m_pImage->setQGraphicsViewWH(nwith, nheight);
  qgraphicsScene->addItem(m_pImage);
  ui->graphicsView->setSceneRect(
      QRectF(-(nwith / 2), -(nheight / 2), nwith, nheight));

  ui->graphicsView->setScene(qgraphicsScene);
  ui->graphicsView->setFocus();
}
