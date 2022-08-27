#include "dlgloadpic.h"

#include "src/mainwindow.h"
#include "ui_dlgloadpic.h"
extern MainWindow* mw_one;
extern QString picfile;

dlgLoadPic::dlgLoadPic(QWidget* parent)
    : QDialog(parent), ui(new Ui::dlgLoadPic) {
  ui->setupUi(this);
  this->installEventFilter(this);

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
      return true;
    }
  }

  return QWidget::eventFilter(watch, evn);
}

void dlgLoadPic::on_btnBack_clicked() {
  close();
  k = 0;
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
