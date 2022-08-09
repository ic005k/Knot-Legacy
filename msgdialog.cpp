#include "msgdialog.h"

#include "mainwindow.h"
#include "ui_msgdialog.h"

extern MainWindow* mw_one;

msgDialog::msgDialog(QWidget* parent) : QDialog(parent), ui(new Ui::msgDialog) {
  ui->setupUi(this);

  ui->lblText->setWordWrap(true);
  ui->lblText->adjustSize();

  this->setFixedHeight(240);
  this->setFixedWidth(mw_one->width());
  this->setGeometry(mw_one->geometry().x(),
                    (mw_one->height() - this->height()) / 2, this->width(),
                    this->height());
  this->setModal(true);
  // this->show();
}

msgDialog::~msgDialog() { delete ui; }

void msgDialog::on_btnOK_clicked() { close(); }
