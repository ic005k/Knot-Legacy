#include "dlgsettime.h"

#include <QKeyEvent>

#include "mainwindow.h"
#include "ui_dlgsettime.h"
#include "ui_mainwindow.h"
extern MainWindow* mw_one;

dlgSetTime::dlgSetTime(QWidget* parent)
    : QDialog(parent), ui(new Ui::dlgSetTime) {
  ui->setupUi(this);
}

dlgSetTime::~dlgSetTime() { delete ui; }

void dlgSetTime::on_btnBack_clicked() { close(); }

void dlgSetTime::keyReleaseEvent(QKeyEvent* event) { event->accept(); }

void dlgSetTime::on_btnOk_clicked() {
  if (!mw_one->isAdd) {
    mw_one->set_Time();
  } else {
    mw_one->add_Data(mw_one->get_tw(mw_one->ui->tabWidget->currentIndex()),
                     ui->timeEdit->text(), ui->lineEdit->text().trimmed());
  }
  close();
}
