#include "dlgfloatfun.h"

#include "src/mainwindow.h"
#include "ui_dlgfloatfun.h"
#include "ui_mainwindow.h"

extern MainWindow* mw_one;

dlgFloatFun::dlgFloatFun(QWidget* parent)
    : QDialog(parent), ui(new Ui::dlgFloatFun) {
  ui->setupUi(this);
  setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint |
                 Qt::WindowDoesNotAcceptFocus);
  this->setContentsMargins(1, 1, 1, 1);
  this->ui->gridLayout->setMargin(1);
  this->ui->gridLayout->setContentsMargins(1, 1, 1, 1);

  setAttribute(Qt::WA_TranslucentBackground);
  ui->btnAdd->setStyleSheet("border:none");
  ui->btnDel->setStyleSheet("border:none");
}

void dlgFloatFun::init() {
  this->setGeometry(
      mw_one->geometry().x() + mw_one->width() - this->width() - 10,
      mw_one->geometry().y() + mw_one->ui->frameMenu->height() +
          mw_one->ui->tabWidget->height() - this->height(),
      this->width(), this->height());
  this->show();

  qDebug() << "floatfun=" << this->x(), this->y();
}

dlgFloatFun::~dlgFloatFun() { delete ui; }

void dlgFloatFun::on_btnAdd_clicked() { mw_one->on_btnPlus_clicked(); }

void dlgFloatFun::on_btnDel_clicked() { mw_one->on_btnLess_clicked(); }
