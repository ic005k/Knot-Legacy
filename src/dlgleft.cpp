#include "dlgleft.h"

#include "ui_dlgleft.h"

dlgLeft::dlgLeft(QWidget *parent) : QDialog(parent), ui(new Ui::dlgLeft) {
  ui->setupUi(this);
}

dlgLeft::~dlgLeft() { delete ui; }

void dlgLeft::init(int x, int y) {
  setWindowFlags(Qt::WindowStaysOnTopHint);

#ifdef Q_OS_MAC
  setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint |
                 Qt::WindowDoesNotAcceptFocus);
#endif

  this->setContentsMargins(1, 1, 1, 1);

  ui->btnLeft->setStyleSheet("border:none");

  this->setGeometry(x - width(), y, this->width(), this->height());
  show();
  ;
}
